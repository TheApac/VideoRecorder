/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "ManagerVideo.h"
#include <iostream>
using namespace std;

ManagerVideo::ManagerVideo(int ID) {
    this->ID = ID;
    cout << "creation" << endl;
}

ManagerVideo& ManagerVideo::getInstance(int ID) {
    static ManagerVideo instance(ID);
    return instance;
}

int ManagerVideo::getID() {
    return this->ID;
}

ManagerVideo::~ManagerVideo() {
}