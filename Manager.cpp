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

Manager::Manager(string ConfigFile) {
    name = "", log = "", password = "", url = "", path = "";
    ID = -1;
    nbdays = -1;
    int nbLinesRead = 0;
    ifstream file(ConfigFile);
    string line;
    regex ChangeCam("^\\[CAMERA");
    try {
        while (std::getline(file, line)) { // iterate through each line of the configuration file
            ++nbLinesRead;
            if (line.find_first_of("=") != string::npos) { // Dealing differently with separation lines
                if (line.substr(0, line.find_first_of("=")) == "ID") {
                    string toCompare = line.substr(line.find_first_of("=") + 1);
                    if (isOnlyNumeric(toCompare)) {
                        if (ID == -1) {
                            ID = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                        } else {
                            throw DuplicateField("ID (" + to_string(ID) + ")");
                        }
                    } else {
                        throw InvalidID(toCompare.substr(0, toCompare.size() - 1));
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Nom") {
                    if (name == "") {
                        name = line.substr(line.find_first_of("=") + 1);
                    } else {
                        throw DuplicateField("Name (" + name.substr(0, name.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Login") {
                    if (log == "") {
                        log = line.substr(line.find_first_of("=") + 1);
                    } else {
                        throw DuplicateField("Log (" + log.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Mdp") {
                    if (password == "") {
                        password = line.substr(line.find_first_of("=") + 1);
                    } else {
                        throw DuplicateField("Password (" + password.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "URLCamera") {
                    if (url == "") {
                        url = line.substr(line.find_first_of("=") + 1);
                    } else {
                        throw DuplicateField("URL (" + url.substr(0, log.size() - 1) + ")");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "Enregistrable") {
                    if (line.substr(line.find_first_of("=") + 1).compare("1") == 1) {
                        if (ID == 0) throw UndefinedField("ID");
                        if (ID != -1 && name != "" && log != "" && password != "" && url != "") {
                            CameraList.push_back(new Camera(path, nbdays, ID, name, log, password, url));
                            ID = -1;
                            name = "";
                            log = "";
                            password = "";
                            url = "";
                        } else {
                            cerr << "ERROR : The configuration for a camera is incomplete" << endl;
                            break;
                        }
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "DossierEnreg") {
                    if (path == "") {
                        path = line.substr(line.find_first_of("=") + 1);
                    } else {
                        throw DuplicateField("Path");
                    }
                } else if (line.substr(0, line.find_first_of("=")) == "NbJourStock") {
                    string toCompare = line.substr(line.find_first_of("=") + 1);
                    if (isOnlyNumeric(toCompare)) {
                        if (nbdays == -1) {
                            nbdays = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                        } else {
                            throw DuplicateField("Number of days to stock");
                        }
                    } else {
                        throw InvalidNbDays(toCompare.substr(0, toCompare.size() - 1));
                    }
                }
                //VERIFICATION CAMERA FINIE
            } else if (regex_search(line, ChangeCam)) {
                ID = -1;
                name = "";
                log = "";
                password = "";
                url = "";
            } else if (!regex_search(line, regex("^\\[PARAM]"))) {
                if (line.substr(0, line.size() - 1).empty()) {
                    throw EmptyLine(to_string(nbLinesRead));
                } else {
                    throw InvalidLine(line.substr(0, line.size() - 1));
                }
            }
        }
        cout << "Number of valid cameras : " << CameraList.size() << endl;
    } catch (CustomException &e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

bool Manager::isCameraOver() {
    if (ID != -1) {
    }
}

Manager::~Manager() {
}

