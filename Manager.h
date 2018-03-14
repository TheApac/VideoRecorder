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
    void run();
    void startRecords();
    static bool isRunningManager();
    virtual ~Manager();
    //static struct node_t* RunningCameraList; // Linked list in wich every camera will self declare in to show its still running
private:
    void CameraOver(int &enregistrable);
    vector<Camera*> CameraList; /* Save each camera in a vector to run their record function after they all have been initialized */
    vector<string> CrashedCameraList; // Vector that store which camera crashed when
    // Keep the value of the camera fields while creating them
    int nbdays;
    string path;
    string name;
    string log;
    string password;
    string url;
    int ID;
    bool didCameraCrash(int ID);
    void removeOldCrashedCameras();
};

#endif /* MANAGER_H */