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
    Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url);
    virtual ~Camera();
    int GetID() const;

    string GetDirectory() const;

    string GetLog() const;

    string GetName() const;

    int GetNbdays() const;

    string GetPassword() const;

    string GetUrl() const;
    
    void record();

private:
    string directory;
    int nbdays;
    int ID;
    string name;
    string log; 
    string password;
    string url;
};

#endif /* CAMERA_H */
