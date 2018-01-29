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

Manager::Manager(string ConfigFile) {
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
                if (line.substr(0, line.find_first_of("=")) == "ID") {
                    string toCompare = line.substr(line.find_first_of("=") + 1);
                    if (isOnlyNumeric(toCompare)) { // check if the ID is a positive integer
                        if (ID == -1) { // if no ID is set for the current camera
                            ID = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                        } else { // two IDs are defined for the same camera
                            throw DuplicateField("ID (" + to_string(ID) + ")");
                        }
                    } else { // The id is not a positive integer
                        throw InvalidID(toCompare.substr(0, toCompare.size() - 1));
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Nom") {
                    if (name == "") { // if no name is set for the current camera
                        name = line.substr(line.find_first_of("=") + 1);
                    } else { // two names are defined for the same camera
                        throw DuplicateField("Name (" + name.substr(0, name.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Login") {
                    if (log == "") { // if no log is set for the current camera
                        log = line.substr(line.find_first_of("=") + 1);
                    } else { // two logs are defined for the same camera
                        throw DuplicateField("Log (" + log.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Mdp") {
                    if (password == "") { // if no password is set for the current camera
                        password = line.substr(line.find_first_of("=") + 1);
                    } else { // two passwords are defined for the same camera
                        throw DuplicateField("Password (" + password.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "URLCamera") {
                    if (url == "") { // if no url is set for the current camera
                        url = line.substr(line.find_first_of("=") + 1);
                    } else { // two URLs are defined for the same camera
                        throw DuplicateField("URL (" + url.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Enregistrable") {
                    if (line.substr(line.find_first_of("=") + 1).compare("1") == 1) { // check if the camera is recordable
                        enregistrable = 1;
                    } else if (line.substr(line.find_first_of("=") + 1).compare("0") == 1) { // if not, update the param
                        enregistrable = 0;
                    } else { // if field not equal to 0 or 1, throw error
                        throw InvalidLine(to_string(nbLinesRead) + " - enregistrable must be 0 or 1");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "DossierEnreg") {
                    if (path == "") {
                        path = line.substr(line.find_first_of("=") + 1);
                    } else { //only one saving directory can be specified
                        throw DuplicateField("Path");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "NbJourStock") {
                    string toCompare = line.substr(line.find_first_of("=") + 1);
                    if (isOnlyNumeric(toCompare)) { //check if the number of days to keep is a positive integer
                        if (nbdays == -1) {
                            nbdays = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                        } else { //throw error if two number of days are specified
                            throw DuplicateField("Number of days to stock");
                        }
                    } else {
                        throw InvalidNbDays(toCompare.substr(0, toCompare.size() - 1));
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Site") {
                    if (location == "") {
                        location = line.substr(line.find_first_of("=") + 1);
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
        // check for the last camera of the file
        CameraOver(enregistrable);
        cout << "Number of valid cameras : " << CameraList.size() << endl;
    } catch (CustomException &e) {
        // If any error is encountered, display the error and the line
        cerr << e.what() << " on line : " << nbLinesRead << endl;
        string error = "Error in config file given as parameter for the Manager\n"
                "The file : " + ConfigFile + " returned an " + string(e.what()) + " on line " + to_string(nbLinesRead) + "\n\n";
        if (location != "") {
            error += "Concerned site : " + location + "\n";
        }
        sendEmail(error);
        // Quit the constructor
        exit(EXIT_FAILURE);
    }
    for(Camera *camera : CameraList) {
        cout << camera->GetName() <<endl; //TODO supprimer cette ligne
        sleep(5);
        //camera->Record
    }
}

void Manager::CameraOver(int &enregistrable) {
    if (ID != -1 && name != "" && log != "" && password != "" && url != "" && (enregistrable == 0 || enregistrable == 1)) {
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