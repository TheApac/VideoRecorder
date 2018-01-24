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
    Manager(string ConfigFile);
    virtual ~Manager();
private:
    bool isCameraOver();
    vector<Camera*> CameraList;
    int nbdays;
    string path;
    string name;
    string log;
    string password;
    string url;
    int ID;
};

#endif /* MANAGER_H */

