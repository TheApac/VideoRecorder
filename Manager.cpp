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
#include "Watchdog.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>

Manager::Manager() {
    //deamonize();
    if (isRunningManager()) {
        exit(0);
    }
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    name = "", log = "", password = "", url = "", path = "";
    ID = -1;
    nbdays = -1;
    bool firstCamera = true; // Prevent a bug for the start of the first camera
    string location = "";
    int enregistrable = -1;
    int nbLinesRead = 0; // Keep the number of the current line to send it as detail if an error is encountered

    ifstream file(directoryOfFiles + "/ConfigFiles/cameras.ini");
    if (!file.is_open()) {
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
                    string toCompare = parameterValue;
                    if (isOnlyNumeric(toCompare)) { // check if the ID is a positive integer
                        if (ID == -1) { // if no ID is set for the current camera
                            ID = atoi(parameterValue.c_str());
                        } else { // two IDs are defined for the same camera
                            throw DuplicateField("ID (" + to_string(ID) + ")");
                        }
                    } else { // The id is not a positive integer
                        throw InvalidID(toCompare.substr(0, toCompare.size() - 1));
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
                    if (enregistrable != 0 && enregistrable != 1) {// if field not equal to 0 or 1, throw error
                        throw InvalidLine(to_string(nbLinesRead) + " - enregistrable must be 0 or 1");
                    }
                } else if (parameterName == "DossierEnreg") {
                    if (path == "") {
                        path = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { //only one saving directory can be specified
                        throw DuplicateField("Path");
                    }
                } else if (parameterName == "NbJourStock") {
                    string toCompare = parameterValue;
                    if (isOnlyNumeric(toCompare)) { //check if the number of days to keep is a positive integer
                        if (nbdays == -1) {
                            nbdays = atoi(parameterValue.c_str());
                        } else { //throw error if two number of days are specified
                            throw DuplicateField("Number of days to stock");
                        }
                    } else {
                        throw InvalidNbDays(toCompare.substr(0, toCompare.size() - 1));
                    }
                } else if (parameterName == "Site") {
                    if (location == "") {
                        location = parameterValue;
                    } else { //only one location can be specified
                        throw DuplicateField("Site");
                    }
                } else if (parameterName == "nbSecondsRecord") {
                    if (isOnlyNumeric(parameterValue)) {
                        if (!Camera::setSecondsToRecord(atoi(parameterValue.c_str()))) {
                            throw DuplicateField("Number of seconds to record");
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
    } catch (CustomException &e) {
        // If an error is thrown, print it on the terminal
        cerr << e.what() << " on line : " << nbLinesRead << endl; // If any error is encountered, display the error and the line
        string error = "Error in config file given as parameter for the Manager\n"
                "The file : " + string(pw->pw_dir) + "/.VideoRecorder/cameras.ini" + " returned an " + string(e.what()) + " on line " + to_string(nbLinesRead) + "\n\n";
        // And send it by email
        sendEmail(error);
        // Quit the constructor
        exit(EXIT_FAILURE);
    }
}

void Manager::CameraOver(int &enregistrable) {
    if (ID != -1 && name != "" && log != "" && password != "" && url != "" && enregistrable != -1) {
        // Make sure there is not two cameras with the same ID
        for (Camera* camera : CameraList) {
            if (camera->GetID() == ID) {
                throw DuplicateID(to_string(ID));
            }
        }
        // If all the fields are completed and the camera can be recorded
        if (enregistrable == 1) {
            CameraList.push_back(new Camera(path, nbdays, ID, name, log, password, url, this));
        }
        // Reset all fields
        ID = -1;
        name = "";
        log = "";
        password = "";
        url = "";
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
        }
    }
}

Manager::~Manager() {

    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string toRemove = directoryOfFiles + "/.RunningVideoRecorder";
    remove(toRemove.c_str());
}

void Manager::startRecords() {
    for (Camera *camera : CameraList) {
        pid_t pid = fork();
        if (pid == 0) {
            camera->record();
            exit(0);
        }
    }
    sleep(30);
}

void Manager::run() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string file = directoryOfFiles + "/.RunningVideoRecorder";
    bool isPresent = true;
    while (1) {
        removeOldCrashedCameras();
        int indexCamera = 0;
        //vector<Camera*> CameraListCopy = CameraList;
        for (Camera *camera : CameraList) {
            cout << "test manager : " << getTest() << endl;
            //cout << getRunningCameraSize() << endl;
            //isPresent = IsInRunningList(to_string(camera->GetID()));
            //cout << boolalpha << isPresent << endl;
            if (!isPresent) {
                int index = 0;
                int IDToDelete = CameraList.at(0)->GetID();
                while (IDToDelete != camera->GetID()) {
                    ++index;
                    IDToDelete = CameraList.at(index)->GetID();
                }
                path = camera->GetDirectory();
                nbdays = camera->GetNbdays();
                ID = camera->GetID();
                name = camera->GetName();
                log = camera->GetLog();
                password = camera->GetPassword();
                url = camera->GetUrl();
                Camera* tempCamera = new Camera(path, nbdays, ID, name, log, password, url, this);
                CameraList.push_back(tempCamera);
                CameraList.erase(CameraList.begin() + index);
                pid_t pid = fork();
                if (pid == 0) {
                    tempCamera->record();
                    exit(0);
                }
                if (!didCameraCrash(camera->GetID())) {
                    CrashedCameraList.push_back(to_string(camera->GetID()) + "-" + currentDate());
                    sendEmail("The recording of the camera of ID " + to_string(camera->GetID()) + " crashed.\nThe video recorder tried to reboot it");
                }
            } else {
                deleteNode(to_string(camera->GetID()));
            }
            ++indexCamera;
        }
        remove(file.c_str());
        ofstream runfile(file);
        runfile << currentDate() << endl;
        runfile.close();
        sleep(30);
    }
}

bool Manager::isRunningManager() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    if (fileExists(directoryOfFiles + "/.RunningVideoRecorder")) {
        return true;
    }
    return false;
}

void Manager::removeOldCrashedCameras() {
    vector<string> CrashedCameraListCopy = CrashedCameraList;
    int index = 0;
    for (string cameraInfo : CrashedCameraList) {
        if (secondsSinceDate(cameraInfo.substr(cameraInfo.find_first_of('-') + 1)) > 10 * 60) {
            CrashedCameraListCopy.erase(CrashedCameraListCopy.begin() + index);
        }
        ++index;
    }
    CrashedCameraList = CrashedCameraListCopy;
}

bool Manager::didCameraCrash(int ID) {
    int index = 0;
    if (CrashedCameraList.size() > 0) {
        while (index != CrashedCameraList.size()) {
            if (CrashedCameraList[index].substr(0, CrashedCameraList[index].find_first_of('-')) == to_string(ID)) {
                return true;
            }
            ++index;
        }
    }
    return false;
}