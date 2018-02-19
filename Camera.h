/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Camera.h
 * Author: Alexandre
 *
 * Created on 16 janvier 2018, 09:22
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <string>

class Manager;

using namespace std;

class Camera {
public:
    Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url, Manager* manager);
    virtual ~Camera();
    int GetID() const;

    string GetDirectory() const;

    string GetLog() const;

    string GetName() const;

    int GetNbdays() const;

    string GetPassword() const;

    string GetUrl() const;

    void record();

    string getFileName();
private:
    string directory; /* Directory where records will be saved */
    int nbdays; /* Positive integer \n\n Number of days to keep the records */
    int ID; /* Positive integer \n\n ID of the camera */
    string name; /* Name of the camera */
    string url; /* URL of the rtsp */
    string log; /* Log for the rtsp */
    string password; /* Password for the rtsp */
    Manager* manager; /* Application manager */
    string timeOfLastCrash; /* Keep the date of the last email sent */
};

#endif /* CAMERA_H */
