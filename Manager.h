/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Manager.h
 * Author: Alexandre
 *
 * Created on 24 janvier 2018, 14:27
 */

#ifndef MANAGER_H
#define MANAGER_H

#include <string>
#include <vector>
#include "Camera.h"

using namespace std;

class Manager {
public:
    Manager();
    void startRecords();
    virtual ~Manager();
private:
    void CameraOver(int &enregistrable);
    vector<Camera*> CameraList; /* Save each camera in a vector to run their record function after they all have been initialized */
    // Keep the value of the camera fields while creating them
    int nbdays;
    string path;
    string name;
    string log;
    string password;
    string url;
    int nbSecBetweenRecords = -1;
    int ID;
    void updateTime();
};

#endif /* MANAGER_H */