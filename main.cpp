/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Alexandre
 *
 * Created on 2 janvier 2018, 10:40
 */
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "Camera.h"
#include "Utility.h"
#include "ManagerVideo.h"
#include <fstream>
#include <regex>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write


using namespace std;

int main(int argc, char** argv) {
    vector<Camera*> CameraList;
    ifstream file("/home/Alexandre/Téléchargements/cameras.ini");
    string line;
    int compteur = -1;
    int nbdays = -1;
    string path = "";
    int ID = 0;
    string name = "";
    string log = "";
    string password = "";
    string url = "";
    while (std::getline(file, line)) { // iterate through each line of the configuration file
        if (line.find_first_of("=") != string::npos) { // Nothing to do for separation lines
            compteur++;
            if ((compteur - 3) % 11 == 0) {
                string toCompare = line.substr(line.find_first_of("=") + 1) + "\0";
                if (isOnlyNumeric(toCompare)) {
                    ID = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                } else {
                    throw 20;
                    cerr << "ERROR : The ID field must be digits only" << endl;
                    break;
                }
            } else if ((compteur - 4) % 11 == 0) {
                name = line.substr(line.find_first_of("=") + 1);
            } else if ((compteur - 5) % 11 == 0) {
                log = line.substr(line.find_first_of("=") + 1);
            } else if ((compteur - 6) % 11 == 0) {
                password = line.substr(line.find_first_of("=") + 1);
            } else if ((compteur - 7) % 11 == 0) {
                url = line.substr(line.find_first_of("=") + 1);
            } else if (compteur > 2 && (compteur - 13) % 11 == 0) {
                if (line.substr(line.find_first_of("=") + 1).compare("1") == 1) {
                    if (ID != 0 && name != "" && log != "" && password != "" && url != "") {
                        CameraList.push_back(new Camera(path, nbdays, ID, name, log, password, url));
                        ID = 0;
                        name = "";
                        log = "";
                        password = "";
                        url = "";
                    } else {
                        cerr << "ERROR : The configuration for a camera is incomplete" << endl;
                        break;
                    }
                }
            } else if (compteur == 1) {
                path = line.substr(line.find_first_of("=") + 1);
            } else if (compteur == 2) {
                string toCompare = line.substr(line.find_first_of("=") + 1);
                cout << "String to compare : " << toCompare << endl;
                cout << "Result : " << boolalpha << isOnlyNumeric(toCompare) << endl;
                if (isOnlyNumeric(toCompare)) {
                    nbdays = atoi(line.substr(line.find_first_of("=") + 1).c_str());
                } else {
                    cerr << "ERROR : Number of days to stock is not an integer" << endl;
                    return EXIT_FAILURE;
                }
            }
        }
    }
    cout << "Number of valid cameras : " << CameraList.size() << endl;
    return EXIT_SUCCESS;
}