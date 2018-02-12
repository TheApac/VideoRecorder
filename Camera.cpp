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
#include <boost/date_time/posix_time/posix_time.hpp>

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
}

void Camera::record() {
    createDirectoryVideos(this->directory);
    // REMOVE OLD FILES
    while (1) {
        VideoCapture inputVideo("rtsp://" + this->log + ":" + this->password + "@" + this->url); //open the stream with the identification
        if (!inputVideo.isOpened()) {
            printf("Couldn't connect to camera\n");
            break;
        }
        Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT)); // Acquire input size
        VideoWriter outputVideo; // Open the output
        if (!outputVideo.isOpened()) {
            cerr << "Could not open the output video to write: " << this->url << endl;
            break;
        }
        /* To keep during testing */
        int fourcc = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
        char FOURCC_STR[] = {
            (char) (fourcc & 0XFF)
            , (char) ((fourcc & 0XFF00) >> 8)
            , (char) ((fourcc & 0XFF0000) >> 16)
            , (char) ((fourcc & 0XFF000000) >> 24)
            , 0
        };
        // cout << FOURCC_STR[0]<< FOURCC_STR[1]<< FOURCC_STR[2]<< FOURCC_STR[3] << endl;
        /* ---------------------- */
        outputVideo.open(this->getFileName(), inputVideo.get(CV_CAP_PROP_FOURCC), inputVideo.get(CV_CAP_PROP_FPS), size, true); //create an output file
        Mat src; // Image type
        time_t t = time(0);
        long int secondsToStop = time(&t) + 60 * 15;
        while (time(&t) < secondsToStop) { // Loop while the file is not at its max time
            inputVideo >> src; // read an image
            outputVideo.write(src); // write it in the output file
        }
        outputVideo.release(); // close the output writer
        inputVideo.release(); // close the video reader
    }
}

Camera::~Camera() {
}

string Camera::GetDirectory() const {
    return this->directory;
}

int Camera::GetID() const {
    return this->ID;
}

string Camera::GetLog() const {
    return this->log;
}

string Camera::GetName() const {
    return this->name;
}

int Camera::GetNbdays() const {
    return this->nbdays;
}

string Camera::GetPassword() const {
    return this->password;
}

string Camera::GetUrl() const {
    return this->url;
}

string Camera::getFileName() {
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    // Get the time offset in current day
    const boost::posix_time::time_duration td = now.time_of_day();
    const long hours = td.hours(); // Current hour
    const long minutes = td.minutes(); // Current minute
    const long seconds = td.seconds(); // Current seconds
    const long milliseconds = td.total_milliseconds() -((hours * 3600 + minutes * 60 + seconds) * 1000); // Convert number of seconds since midnight to current milliseconds
    tm date = to_tm(now); // Struct for the date
    string FileName = "C" + to_string(this->ID) + "-" + to_string(date.tm_year + 1900);
    if (date.tm_mon + 1 < 10) { // if month between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(date.tm_mon + 1);
    } else {
        FileName += to_string(date.tm_mon + 1);
    }
    if (date.tm_mday < 10) { // if day between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(date.tm_mday);
    } else {
        FileName += to_string(date.tm_mday);
    }
    FileName += "-"; // separate Date and Hour in the file name
    if (hours < 10) { // if hour between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(hours);
    } else {
        FileName += to_string(hours);
    }
    if (minutes < 10) { // if minute between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(minutes);
    } else {
        FileName += to_string(minutes);
    }
    if (seconds < 10) { // if seconds between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(seconds);
    } else {
        FileName += to_string(seconds);
    }
    if (milliseconds < 10) { // if milliseconds between 1 and 9, add two 0 in front of it
        FileName += "00" + to_string(milliseconds);
    } else if (milliseconds < 100) { // if milliseconds between 10 and 99, add a 0 in front of it
        FileName += "0" + to_string(milliseconds);
    } else {
        FileName += to_string(milliseconds);
    }
    return FileName + ".mp4"; //add the extension
}