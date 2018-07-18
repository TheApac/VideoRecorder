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
#include <memory>
#include "Camera.h"

using namespace std;

class Manager {
public:
    Manager();
    void startRecords();
    virtual ~Manager();
private:
    void CameraOver(int &enregistrable);
    vector<shared_ptr<Camera>> CameraList; /* Save each camera in a vector to run their record function after they all have been initialized */
    /* Keep the value of the camera fields while creating them */
    int nbdays;
    int ID;
    string path;
    string tempPath;
    string name;
    string log;
    string password;
    string url;
    string repertoireDefaut;
    /* ------------------------------------------------------- */
    int listenPort; // Port where movement notification are sent
    int nbMinBetweenMoveBuffer;
    int nbSecBetweenRecords = -1;
    void updateTime(); // Print the current date and time in a file, to make sure the process is still running
    void startMvmtDetect(); // Start a new thread waiting for a tcp message
    void detectMvmt(); // Wait for a TCP notification and check if the message is good and coming from a camera
    void getListenPort(); // Retrieve the port where notification are sent
    shared_ptr<Camera> getCamByIp(string ip); // Return a camera from an IP, NULL if no camera is found
    void runBufferDir(); // Reboot thread that will move files from buffer to definitive directory if crashed
};

#endif /* MANAGER_H */