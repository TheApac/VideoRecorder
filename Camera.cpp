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

Camera::Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url) {
    //deamonize();
    this->directory = path;
    this->nbdays = nbdays;
    this->ID = ID;
    this->name = name;
    this->log = log;
    this->password = password;
    this->url = url;

    //createDirectoryVideos(this->directory);
    //while (1) {
    ////VideoCapture inputVideo("rtsp://" + this->url);
    ////if (!inputVideo.isOpened()) {
    ////     printf("Couldn't connect to camera\n");
    //break;
    //// }
    ////const string NAME = "testRecordN01.avi";
    //// int ex = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
    ////     Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), // Acquire input size
    ////            (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    ////    VideoWriter outputVideo; // Open the output
    //outputVideo.open(NAME, CV_FOURCC('M','P','4','2'), inputVideo.get(CV_CAP_PROP_FPS), size, true);
    //        if (!outputVideo.isOpened()) {
    //            cout << "Could not open the output video for write: " << this->url << endl;
    //            break;
    //        }
    ////        int fourcc = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));

    ////    char FOURCC_STR[] = {
    ////    (char)(fourcc & 0XFF)
    ////    , (char)((fourcc & 0XFF00) >> 8)
    ////    , (char)((fourcc & 0XFF0000) >> 16)
    ////    , (char)((fourcc & 0XFF000000) >> 24)
    ////    , 0
    ////};
    ////std::cout << "FOURCC is '" << FOURCC_STR << "'\n";
    //        Mat src;
    //        time_t t = time(0);
    //        long int secondsToStop = time(&t) + 30;
    //        while (time(&t) < secondsToStop) { //Show the image captured in the window and repeat
    //            //inputVideo >> src; // read
    //            //outputVideo.write(src);
    //        }
    //        outputVideo.release();
    //        inputVideo.release();
    //}
}

Camera::~Camera() {
}

string Camera::GetDirectory() const{
    return this->directory;
}
int Camera::GetID() const{
    return this->ID;
}
string Camera::GetLog() const{
    return this->log;
}
string Camera::GetName() const{
    return this->name;
}
int Camera::GetNbdays() const{
    return this->nbdays;
}
string Camera::GetPassword() const{
    return this->password;
}
string Camera::GetUrl() const{
    return this->url;
}