/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Manager.cpp
 * Author: Alexandre
 *
 * Created on 24 janvier 2018, 14:27
 */

#include "Utility.h"
#include "Manager.h"
#include "CustomException.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <boost/thread.hpp>
#include <sys/stat.h>

using namespace boost;

static mutex mutex_camlist;

Manager::Manager() {
    if (isRunningManager()) { //If a manager is Running, don't start a new one
        exit(0);
    }
    setLocation(""); //Reinit location
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string runfileName = directoryOfFiles + "/.RunningVideoRecorder"; // Save the path to the file that show it's still running
    remove(runfileName.c_str());
    ofstream runfile(runfileName);
    runfile << currentDate() << endl;
    name = "", log = "", password = "", url = "", path = "", tempPath = "";
    ID = -1, nbdays = -1, nbMinBetweenMoveBuffer = -1;
    int enregistrable = -1;
    bool firstCamera = true; // Prevent a bug for the start of the first camera
    int nbLinesRead = 0; // Keep the number of the current line to send it as detail if an error is encountered
    ifstream file(directoryOfFiles + "/ConfigFiles/cameras.ini");
    if (!file.is_open()) { // Check if the config file is at the right place
        string error = "File " + directoryOfFiles + "/ConfigFiles/cameras.ini was not found on the server : ";
        char hostname[128] = "";
        gethostname(hostname, sizeof (hostname));
        error += string(hostname) + "\n";
        sendEmail(error);
        exit(EXIT_FAILURE);
    }
    string line;
    regex ChangeCam("^\\[CAMERA");
    try {
        while (std::getline(file, line)) { // iterate through each line of the configuration file
            ++nbLinesRead; // used to display which line has a problem
            if (line.find_first_of("=") != string::npos) { // Dealing differently with separation lines
                string parameterName = line.substr(0, line.find_first_of("="));
                string parameterValue = line.substr(line.find_first_of("=") + 1);
                if (parameterName == "ID") {
                    if (isOnlyNumeric(parameterValue)) { // check if the ID is a positive integer
                        if (ID == -1) { // if no ID is set for the current camera
                            ID = atoi(parameterValue.c_str());
                        } else { // two IDs are defined for the same camera
                            throw DuplicateField("ID (" + to_string(ID) + ")");
                        }
                    } else { // The id is not a positive integer
                        throw InvalidID(parameterValue);
                    }
                } else if (parameterName == "Nom") {
                    if (name == "") { // if no name is set for the current camera
                        name = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { // two names are defined for the same camera
                        throw DuplicateField("Name (" + name.substr(0, name.size() - 1) + ")");
                    }
                } else if (parameterName == "Login") {
                    if (log == "") { // if no log is set for the current camera
                        log = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { // two logs are defined for the same camera
                        throw DuplicateField("Log (" + log.substr(0, log.size() - 1) + ")");
                    }
                } else if (parameterName == "Mdp") {
                    if (password == "") { // if no password is set for the current camera
                        password = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { // two passwords are defined for the same camera
                        throw DuplicateField("Password (" + password.substr(0, log.size() - 1) + ")");
                    }
                } else if (parameterName == "URLCamera") {
                    if (url == "") { // if no url is set for the current camera
                        url = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { // two URLs are defined for the same camera
                        throw DuplicateField("URL (" + url.substr(0, log.size() - 1) + ")");
                    }
                } else if (parameterName == "Enregistrable") {
                    enregistrable = atoi(parameterValue.c_str());
                    if (enregistrable != 0 && enregistrable != 1) { //if field not equal to 0 or 1, throw error
                        throw InvalidLine(to_string(nbLinesRead) + " - enregistrable must be 0 or 1");
                    }
                } else if (parameterName == "DossierEnreg") {
                    if (path == "") {
                        path = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { //only one saving directory can be specified
                        throw DuplicateField("Path");
                    }
                } else if (parameterName == "NbJourStock") {
                    if (isOnlyNumeric(parameterValue)) { //check if the number of days to keep is a positive integer
                        if (nbdays == -1) {
                            nbdays = atoi(parameterValue.c_str());
                        } else { //throw error if two number of days are specified
                            throw DuplicateField("Number of days to stock");
                        }
                    } else {
                        throw InvalidNbDays(parameterValue.substr(0, parameterValue.size() - 1));
                    }
                } else if (parameterName == "Site") {
                    if (!setLocation(parameterValue)) { // Check if the location hasn't been changed yet
                        throw DuplicateField("Site");
                    }
                } else if (parameterName == "nbSecondsRecord") {
                    if (isOnlyNumeric(parameterValue)) { //check if the number of seconds to record is a positive integer
                        if (!Camera::setSecondsToRecord(atoi(parameterValue.c_str()))) {
                            throw DuplicateField("Number of seconds to record");
                        }
                    }
                } else if (parameterName == "nbSecondsBetweenRecord") {
                    if (isOnlyNumeric(parameterValue)) { //check if the number of seconds between each record is a positive integer
                        if (this->nbSecBetweenRecords != -1) { //check if the number of seconds between each record hasn't been changed yet
                            throw DuplicateField("Number of seconds between record");
                        } else {
                            this->nbSecBetweenRecords = atoi(parameterValue.c_str());
                        }
                    } else {
                        throw InvalidNbMin(parameterValue); // Throw an error if the number is invalid
                    }
                } else if (parameterName == "TempDirectory") {
                    if (this->tempPath != "") { // Check if ther is not two buffer directory declared for one camera
                        throw DuplicateField("Buffer directory");
                    } else {
                        tempPath = parameterValue.substr(0, parameterValue.length() - 1);
                    }
                } else if (parameterName == "NbMinBuffer") {
                    if (isOnlyNumeric(parameterValue)) { //check if the number of minutes between each move from buffer to final directory  is a positive integer
                        if (this->nbMinBetweenMoveBuffer != -1) {
                            throw DuplicateField("Time between moves from buffer");
                        } else {
                            this->nbMinBetweenMoveBuffer = atoi(parameterValue.c_str());
                        }
                    }
                }
            } else if (regex_search(line, ChangeCam)) { //When changing camera config, check if previous is ok
                if (!firstCamera) { // no check for start of first camera
                    CameraOver(enregistrable);
                } else {
                    firstCamera = false;
                }
            } else if (!regex_search(line, regex("^\\[PARAM]"))) { //Do nothin for line declaring start of params
                if (line.substr(0, line.size() - 1).empty()) { //if the line is empty
                    throw EmptyLine(to_string(nbLinesRead));
                } else { // the line is invalid
                    throw InvalidLine(line.substr(0, line.size() - 1));
                }
            }
        }
        CameraOver(enregistrable); // check for the last camera of the file
    } catch (CustomException &e) { // If an error is thrown, print it on the terminal
        cerr << e.what() << " on line : " << nbLinesRead << endl; // If any error is encountered, display the error and the line
        string error = "Error in config file given as parameter for the Manager\nThe file : " + string(pw->pw_dir) + "/.VideoRecorder/cameras.ini" + " returned an " + string(e.what()) + " on line " + to_string(nbLinesRead) + "\n\n";
        sendEmail(error); // And send it by email
        exit(EXIT_FAILURE); // Quit the constructor
    }
}

void Manager::CameraOver(int &enregistrable) {
    if (nbdays == -1) {
        nbdays = DEFAULT_DAYS_TO_KEEP;
    }
    if (ID != -1 && name != "" && log != "" && password != "" && url != "" && enregistrable != -1) {
        for (Camera* camera : CameraList) {
            if (camera->GetID() == ID) { // Make sure there is not two cameras with the same ID
                throw DuplicateID(to_string(ID));
            }
        }
        if (enregistrable == 1 && path != "") { // If all the fields are completed and the camera can be recorded
            mutex_camlist.lock();
            if (tempPath != "") {
                CameraList.push_back(new Camera(tempPath, ID, name, log, password, url));
                if (nbMinBetweenMoveBuffer == -1) {
                    nbMinBetweenMoveBuffer = DEFAULT_TIME_BUFFER_MOVE; // If there is no number of minute between each move from buffer memory, default (60) is applied
                }
                addBufferDir(nbMinBetweenMoveBuffer, path, tempPath, ID);
            } else {
                CameraList.push_back(new Camera(path, nbdays, ID, name, log, password, url)); // Create a list of camera
            }
            mutex_camlist.unlock();
        } else if (enregistrable == 1 && path != "") {
            throw UndefinedField("Path");
        }
        // Reset all fields
        ID = -1;
        name = "";
        log = "";
        password = "";
        url = "";
        path = "";
        tempPath = "";
        enregistrable = -1;
    } else {
        // At least one field is not complete
        if (ID == -1) {
            throw UndefinedField("ID");
        } else if (name == "") {
            throw UndefinedField("Nom");
        } else if (log == "") {
            throw UndefinedField("Login");
        } else if (password == "") {
            throw UndefinedField("Mdp");
        } else if (url == "") {
            throw UndefinedField("URL");
        } else if (enregistrable == -1) {
            throw UndefinedField("Enregistrable");
        } else if (path == "") {
            throw UndefinedField("Path");
        }
    }
}

Manager::~Manager() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string toRemove = directoryOfFiles + "/.RunningVideoRecorder";
    remove(toRemove.c_str()); // Remove the file telling that a Manager is running
    for (Camera* camera : CameraList) {
        delete camera;
    }
    CleanUpNodes();
}

/* Start to record each cameras
 Check if they are still running, reboot them if not */
void Manager::startRecords() {
    //deamonize();
    if (nbSecBetweenRecords == -1) {
        nbSecBetweenRecords = DEFAULT_TIME_BETWEEN_RECORDS;
    }
    thread(&Manager::updateTime, this);
    for (Camera *camera : CameraList) {
        thread(&Camera::record, camera);
        sleep(nbSecBetweenRecords);
    }
    thread(startMoveFromBuffer, nbdays);
    while (1) {
        removeOldCrashedCameras(); // remove the cameras that crashed over 10min ago
        for (Camera *camera : CameraList) {
            if (!IsInRunningList(to_string(camera->GetID()))) {
                thread(&Camera::record, camera);
                if (!didCameraCrash(camera->GetID())) {
                    sendEmail("The recording of the camera of ID " + to_string(camera->GetID()) + " crashed.\nThe video recorder tried to reboot it");
                    addCrashedCamera(camera->GetID());
                } else {
                    if (timeSinceCrashCamera(camera->GetID()) > 10 * 60) { // Don't send 2 mails in less than 10 min
                        sendEmail("The recording of the camera of ID " + to_string(camera->GetID()) + " crashed.\nThe video recorder tried to reboot it");
                    }
                }
                sleep(nbSecBetweenRecords);
            } else {
                deleteNode(to_string(camera->GetID())); // Remove the camera from the list
            }
        }
        sleep(60);
    }
}

void Manager::updateTime() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    if (!fileExists(directoryOfFiles)) {
        mkdir(directoryOfFiles.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
    string file = directoryOfFiles + "/.RunningVideoRecorder";
    while (1) { // Write the current time in the file every 30 seconds
        cout << "changed time : " << currentDate() << endl;
        remove(file.c_str());
        ofstream runfile(file);
        runfile << currentDate() << endl;
        runfile.close();
        sleep(30);
    }
}