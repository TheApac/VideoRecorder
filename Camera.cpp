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
    //deamonize();
    this->url = "172.18.2.1:8101";
    this->ID = 2;
    this->directory = str;
    //createDirectoryVideos(this->directory);
    while (1) {
        VideoCapture inputVideo("rtsp://" + this->url);
        if (!inputVideo.isOpened()) {
            printf("Couldn't connect to camera\n");
            break;
        }
        const string NAME = "testRecordN01.avi";
        int ex = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
        Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), // Acquire input size
                (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

        VideoWriter outputVideo; // Open the output
        outputVideo.open(NAME, ex, inputVideo.get(CV_CAP_PROP_FPS), size, true);
        if (!outputVideo.isOpened()) {
            cout << "Could not open the output video for write: " << this->url << endl;
            break;
        }
        Mat src;
        time_t t = time(0);
        long int secondsToStop = time(&t) + 30;
        while (time(&t) < secondsToStop) { //Show the image captured in the window and repeat
            inputVideo >> src; // read
            outputVideo.write(src);
        }
    }
}

Camera::~Camera() {
}