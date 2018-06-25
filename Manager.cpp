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

#include "Manager.h"
#include "Utility.h"
#include "CustomException.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <signal.h>
#include <pwd.h>
#include <boost/thread.hpp>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <unistd.h>

static boost::mutex mutex_camlist;
static boost::mutex m_ip;

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
        sendEmail("File " + directoryOfFiles + "/ConfigFiles/cameras.ini was not found");
        exit(EXIT_FAILURE);
    }
    string line;
    regex ChangeCam("^\\[CAMERA");
    try {
        string parameterName = "", parameterValue = "";
        while (std::getline(file, line)) { // iterate through each line of the configuration file
            ++nbLinesRead; // used to display which line has a problem
            if (line.find_first_of("=") != string::npos) { // Dealing differently with separation lines
                parameterName = line.substr(0, line.find_first_of("="));
                parameterValue = line.substr(line.find_first_of("=") + 1);
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
                } else if (parameterName == "Mdps") {
                    if (password == "") { // if no password is set for the current camera
                        password = parameterValue.substr(0, parameterValue.length() - 1);
                    } else { // two passwords are defined for the same camera
                        throw DuplicateField("Password (" + password.substr(0, password.size() - 1) + ")");
                    }
                } else if (parameterName == "IP") {
                    if (url.find(".") == string::npos) { // if no url is set for the current camera
                        url = parameterValue.substr(0, parameterValue.length() - 1) + url;
                    } else { // two URLs are defined for the same camera
                        throw DuplicateField("IP (" + url.substr(0, url.size() - 1) + ")");
                    }
                } else if (parameterName == "PortHTTP") {
                    if (url.find(":") == string::npos) {
                        url = url + ":" + parameterValue.substr(0, parameterValue.length() - 1);
                    } else {
                        throw DuplicateField("PortHTTP (" + url.substr(0, url.size() - 1) + ")");
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
        for (const auto &camera : CameraList) {
            if (camera->GetID() == ID) { // Make sure there is not two cameras with the same ID
                throw DuplicateID(to_string(ID));
            }
        }
        if (enregistrable == 1 && path != "") { // If all the fields are completed and the camera can be recorded
            mutex_camlist.lock();
            if (tempPath != "") {
                CameraList.push_back(make_shared<Camera>(tempPath, ID, name, log, password, url));
                if (nbMinBetweenMoveBuffer == -1) {
                    nbMinBetweenMoveBuffer = DEFAULT_TIME_BUFFER_MOVE; // If there is no number of minute between each move from buffer memory, default (60) is applied
                }
                addBufferDir(nbMinBetweenMoveBuffer, path, tempPath, ID);
            } else {
                CameraList.push_back(make_shared<Camera>(path, nbdays, ID, name, log, password, url)); // Create a list of camera
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
}

/* Start to record each cameras
 Check if they are still running, reboot them if not */
void Manager::startRecords() {
    deamonize();
    if (nbSecBetweenRecords == -1) {
        nbSecBetweenRecords = DEFAULT_TIME_BETWEEN_RECORDS;
    }
    cout << "before start threads" << endl;
    boost::thread(&Manager::updateTime, this);
    boost::thread(startMoveFromBuffer, nbdays);
    boost::thread(runBufferDir);
    boost::thread(&Manager::startMvmtDetect, this);
    getListenPort();
    cout << "after start threads" << endl;
    for (const auto &camera : CameraList) {
        camera->startThreadRTSPUrl();
        sleep(1);
        if (camera->GetRTSPurl() != "") {
            boost::thread(&Camera::record, camera); // Start the record in a new thread
            sleep(nbSecBetweenRecords); // wait between the start of each record
        }
    }
    boost::thread(preventMutexHoldLocked);
    while (1) { // make sure every camera is still recording
        removeOldCrashedCameras(); // remove the cameras that crashed over 10min ago
        for (const auto &camera : CameraList) {
            if (!IsInRunningList(to_string(camera->GetID()))) { // If the camera isn't running
                if (!didCameraCrash(camera->GetID())) {
                    addCrashedCamera(camera->GetID());
                }
                if (timeSinceCrashCamera(camera->GetID()) > 10 * 60 && camera->GetRTSPurl() != "") { // Don't send 2 mails in less than 10 min
                    sendEmail("The recording of the camera of ID " + to_string(camera->GetID()) + " crashed.\nThe video recorder tried to reboot it");
                } else if (camera->GetRTSPurl() == "") {
                    camera->startThreadRTSPUrl();
                    sleep(5);
                }
                boost::thread(&Camera::record, camera); // Restart the record
                sleep(nbSecBetweenRecords); // wait so there isn't two records starting a the same time
            } else {
                deleteNode(to_string(camera->GetID())); // Remove the camera from the list
            }
        }
        sleep(60); // Run every minute
    }
}

void Manager::updateTime() {
    string fileToRead = "/var/www/html/public/.infos.dat";
    string fileToWrite = "/var/www/html/public/.infosTemp.dat";
    ofstream initFile(fileToRead);
    initFile << currentDate() << endl;
    for (const auto &camera : CameraList) {
        initFile << camera->GetID() << "/" << currentDate() << "/" << "OK" << endl;
    }
    initFile.close();
    while (1) { // Write the current time in the file every 30 seconds
        ofstream tempFile(fileToWrite);
        ifstream inFile(fileToRead);
        string line;
        tempFile << currentDate() << endl;
        getline(inFile, line);
        while (std::getline(inFile, line)) {
            vector<string> lineExploded = explode(line);
            tempFile << lineExploded.at(0) << "/";
            for (const auto &camera : CameraList) {
                if (camera->GetID() == atoi(lineExploded.at(0).c_str())) {
                    if (didCameraCrash(camera->GetID())) {
                        if (lineExploded.at(2) == "Crash") {
                            tempFile << lineExploded.at(1) << "/";
                        } else {
                            tempFile << currentDate() << "/";
                        }
                        tempFile << "Crash" << endl;
                    } else {
                        if (lineExploded.at(2) == "OK") {
                            tempFile << lineExploded.at(1) << "/";
                        } else {
                            tempFile << currentDate() << "/";
                        }
                        tempFile << "OK" << endl;
                    }
                    break;
                }
            }
        }
        inFile.close();
        tempFile.close();
        remove(fileToRead.c_str());
        rename(fileToWrite.c_str(), fileToRead.c_str());
        sleep(27); // Update date saved in file every 30 seconds
    }
}

void Manager::startMvmtDetect() {
    while (1) {
        m_ip.lock(); // Only one detection at the same time
        detectMvmt();
    }
}

void Manager::detectMvmt() {
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    char client_message[2000]; // Char[] that saves the message received
    socket_desc = socket(AF_INET, SOCK_STREAM, 0); // Create socket
    int trueflag = 1; // Has to be a reference in setsockopt
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &trueflag, sizeof (int));
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET; // IPV4
    server.sin_addr.s_addr = INADDR_ANY; //Any incoming IP
    server.sin_port = htons(listenPort); //listening port
    bind(socket_desc, (struct sockaddr *) &server, sizeof (server));
    listen(socket_desc, 300); // start listening
    c = sizeof (struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t*) & c); //accept connection from an incoming client
    while ((read_size = recv(client_sock, client_message, 2000, 0)) > 0) { //Receive a message from client
        string message = string(client_message).substr(0, string(client_message).size() - 1);
        char ipClient[INET_ADDRSTRLEN]; // Set a char[] that will contain the ip adress of the sender in binary
        inet_ntop(AF_INET, &client.sin_addr, ipClient, INET_ADDRSTRLEN); // Convert the adress from binary to string
        shared_ptr<Camera> cam = getCamByIp(ipClient); // Try to find a camera that has that IP
        if (cam != NULL && message == "mvt") { // Make sure the good message comes from a camera
            std::ofstream out;
            string pathToFile = "";
            if (cam->GetNbdays() > 0) { //record in the final directory
                pathToFile = cam->GetDirectory();
            } else { //record in the buffer directory
                pathToFile = getPathForCameraID(cam->GetID());
            }
            time_t t = time(0); // get time now
            struct tm * nowTime = localtime(&t); //get local time
            string month = to_string(nowTime->tm_mon + 1);
            if (strlen(month.c_str()) < 2) {
                month = "0" + month; // if month < 10 we add a 0 in front of the number
            }
            string day = to_string(nowTime->tm_mday);
            if (strlen(day.c_str()) < 2) {
                day = "0" + day; // if day < 10 we add a 0 in front of the number
            }
            string date = pathToFile + to_string(nowTime->tm_year + 1900) + "." + month + "." + day; //get current date
            mkdir(date.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            pathToFile = date + "/C" + to_string(cam->GetID()) + "-" + to_string(nowTime->tm_year + 1900) + month + day + ".mvt";
            out.open(pathToFile.c_str(), std::ios::app);
            const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            const boost::posix_time::time_duration td = now.time_of_day(); // Get the time offset in current day
            const long hours = td.hours(); // Current hour
            const long minutes = td.minutes(); // Current minute
            const long seconds = td.seconds(); // Current seconds
            const long milliseconds = td.total_milliseconds() -((hours * 3600 + minutes * 60 + seconds) * 1000); // Convert number of seconds since midnight to current milliseconds
            string toPrint = "";
            if (hours < 10) { // if hour between 1 and 9, add a 0 in front of it
                toPrint += "0" + to_string(hours);
            } else {
                toPrint += to_string(hours);
            }
            if (minutes < 10) { // if minute between 1 and 9, add a 0 in front of it
                toPrint += "0" + to_string(minutes);
            } else {
                toPrint += to_string(minutes);
            }
            if (seconds < 10) { // if seconds between 1 and 9, add a 0 in front of it
                toPrint += "0" + to_string(seconds);
            } else {
                toPrint += to_string(seconds);
            }
            if (milliseconds < 10) { // if milliseconds between 1 and 9, add two 0 in front of it
                toPrint += "00" + to_string(milliseconds);
            } else if (milliseconds < 100) { // if milliseconds between 10 and 99, add a 0 in front of it
                toPrint += "0" + to_string(milliseconds);
            } else {
                toPrint += to_string(milliseconds);
            }
            out << toPrint << "\r\n"; // Add the date at the end of the file
            out.close(); // close the file
        }
    }
    close(socket_desc);
    m_ip.unlock(); // Allow to start a new bind
}

void Manager::getListenPort() {
    listenPort = -1;
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    ifstream file(directoryOfFiles + "/ConfigFiles/2NWatchDog.ini");
    string line;
    while (getline(file, line) && listenPort == -1) { //iterate through the file while the configuration isn't over
        if (line.find_first_of("=") != string::npos) { // Dealing differently with separation lines
            string parameterName = line.substr(0, line.find_first_of("="));
            if (parameterName == "port_mvt") {
                this->listenPort = atoi(line.substr(line.find_first_of("=") + 1).c_str()); // Sets the port to the parameter in file
            }
        }
    }
}

/* Return the camera from its IP */
shared_ptr<Camera> Manager::getCamByIp(string ip) {
    //cout << "Start get cam by ip : " << currentDate() << endl;
    for (const auto &cam : CameraList) {
        if (cam->GetUrl().find(":") != string::npos) {
            if (cam->GetUrl().substr(0, cam->GetUrl().find_first_of(":")) == ip) { //get IP by cuttin the port and following
                //cout << "End get cam by ip (found) : " << currentDate() << endl;
                return cam; // return the found camera
            }
        }
    }
    return NULL; // Return NULL if no camera has that IP
}

/* Reboot thread that will move files from buffer to definitive directory if crashed */
void Manager::runBufferDir() {
    if (bufferDirList.size() > 0) {
        int nbMin = bufferDirList.at(0).nbMin; // gets the number of minutes between move
        sleep(10);
        while (1) {
            if (secondsSinceDate(runningBufferMove) > nbMin * 3) { // wait for 3 loop before checking
                boost::thread(startMoveFromBuffer, nbdays);
            }
            sleep(nbMin * 3);
        }
    }
}