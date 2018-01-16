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
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
using namespace std;

int main(int argc, char** argv) {
    
    ManagerVideo m = ManagerVideo::getInstance(1);
    ManagerVideo a = ManagerVideo::getInstance(2);
    ManagerVideo b = ManagerVideo::getInstance(3);
    ManagerVideo c = ManagerVideo::getInstance(4);
    
    cout << m.getID() << endl;
    cout << a.getID() << endl;
    cout << b.getID() << endl;
    cout << c.getID() << endl;
    return EXIT_SUCCESS;
}