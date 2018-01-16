/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ManagerVideo.h
 * Author: Alexandre
 *
 * Created on 16 janvier 2018, 09:23
 */

#ifndef MANAGERVIDEO_H
#define MANAGERVIDEO_H

class ManagerVideo {
public:
    static ManagerVideo& getInstance(int ID);
    virtual ~ManagerVideo();
    int getID();

private:
    static ManagerVideo *instance;
    ManagerVideo(int ID);
    int ID;
};

#endif /* MANAGERVIDEO_H */

