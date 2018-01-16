/* 
 * File:   Camera.cpp
 * Author: Alexandre
 * 
 * Created on 3 janvier 2018, 10:21
 */

#include <cstdio>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include "Camera.h"
#include "Utility.h"
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write

using namespace std;
using namespace cv;

Camera::Camera(string str) {
    deamonize();
    this->ID = 2;
    this->directory = str;
    createDirectoryVideos(this->directory);
    while (1) {
        VideoCapture inputVideo(this->url);
        if (!inputVideo.isOpened()) {
            break;
        }

    }
}

Camera::~Camera() {}