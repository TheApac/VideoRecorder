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

using namespace std;

class Camera {
public:
    Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url); // With a definitive directory
    Camera(string& tempPath, int& ID, string& name, string& log, string& password, string& url); // With a buffer directory
    virtual ~Camera();

    int GetID() const;
    string GetDirectory() const;
    int GetNbdays() const;
    string GetPassword() const;
    string GetUrl() const;

    void record();

    static bool setSecondsToRecord(int sec); // Set the time to record for each camera, return false if already set
    static void reinitTimeRecord(); // Set the time to record to -1
    static volatile int GetSecondsToRecord();
    bool getFullRTSPUrl();
    string getFileName(); // Returns the filename, according to the ID, current date and time
private:
    volatile static int SecondsToRecord; /* Positive integer \n\n Number of seconds of each recording */
    string directory; /* Directory where records will be saved */
    string tempDirectory; /* Buffer memory where records will be saved */
    int nbdays; /* Positive integer \n\n Number of days to keep the records */
    int ID; /* Positive integer \n\n ID of the camera */
    string name; /* Name of the camera */
    string url; /* URL of the camera's web interface */
    string RTSPurl; /* URL of the camera's rtsp */
    string log; /* Log for the rtsp */
    string password; /* Password for the rtsp */
    string timeOfLastCrash; /* Keep the date of the last email sent */
};

#endif /* CAMERA_H */
