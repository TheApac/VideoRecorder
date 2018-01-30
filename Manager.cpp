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
#include "CustomException.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include <signal.h>

Manager::Manager(string ConfigFile) {
    //deamonize();
    name = "", log = "", password = "", url = "", path = "";
    ID = -1;
    nbdays = -1;
    bool firstCamera = true;
    string location = "";
    int enregistrable = -1;
    int nbLinesRead = 0;
    ifstream file(ConfigFile);
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
                        name = parameterValue;
                    } else { // two names are defined for the same camera
                        throw DuplicateField("Name (" + name.substr(0, name.size() - 1) + ")");
                    }
                } else if (parameterName == "Login") {
                    if (log == "") { // if no log is set for the current camera
                        log = parameterValue;
                    } else { // two logs are defined for the same camera
                        throw DuplicateField("Log (" + log.substr(0, log.size() - 1) + ")");
                    }
                } else if (parameterName == "Mdp") {
                    if (password == "") { // if no password is set for the current camera
                        password = parameterValue;
                    } else { // two passwords are defined for the same camera
                        throw DuplicateField("Password (" + password.substr(0, log.size() - 1) + ")");
                    }
                } else if (parameterName == "URLCamera") {
                    if (url == "") { // if no url is set for the current camera
                        url = parameterValue;
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
                        path = parameterValue;
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
        cout << "Number of valid cameras : " << CameraList.size() << endl;
    } catch (CustomException &e) {
        cerr << e.what() << " on line : " << nbLinesRead << endl; // If any error is encountered, display the error and the line
        string error = "Error in config file given as parameter for the Manager\n"
                "The file : " + ConfigFile + " returned an " + string(e.what()) + " on line " + to_string(nbLinesRead) + "\n\n";
        if (location != "") {
            error += "Concerned site : " + location + "\n";
        } else {
            char hostname[128] = "";
            gethostname(hostname, sizeof (hostname));
            error += "Hostname of server : " + string(hostname) + "\n";
        }
        sendEmail(error);
        // Quit the constructor
        exit(EXIT_FAILURE);
    }
    for (Camera *camera : CameraList) {
        pid_t pid = fork();
        if (pid != 0) {
            cout << camera->getFileName() << endl; //TODO supprimer cette ligne
            //camera->Record
            exit(0);
        }
        sleep(5);
    }
}

void Manager::CameraOver(int &enregistrable) {
    if (ID != -1 && name != "" && log != "" && password != "" && url != "" && enregistrable != -1) {
        // If all the fields are completed and the camera can be recorded
        if (enregistrable == 1) {
            CameraList.push_back(new Camera(path, nbdays, ID, name, log, password, url));
        }
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
}