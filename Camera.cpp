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
    createDirectoryVideos("/home/Alexandre/testDirectory");
    while (1) {
        VideoCapture inputVideo("rtsp://" + this->log + ":" + this->password + "@" + this->url);
        if (!inputVideo.isOpened()) {
            printf("Couldn't connect to camera\n");
            break;
        }
        const string NAME = this->getFileName();
        Size size = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH), // Acquire input size
                (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
        VideoWriter outputVideo; // Open the output
        if (!outputVideo.isOpened()) {
            cout << "Could not open the output video to write: " << this->url << endl;
            break;
        }
        int fourcc = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
        char FOURCC_STR[] = {
            (char) (fourcc & 0XFF)
            , (char) ((fourcc & 0XFF00) >> 8)
            , (char) ((fourcc & 0XFF0000) >> 16)
            , (char) ((fourcc & 0XFF000000) >> 24)
            , 0
        };
        outputVideo.open(NAME, inputVideo.get(CV_CAP_PROP_FOURCC), inputVideo.get(CV_CAP_PROP_FPS), size, true);
        Mat src;
        time_t t = time(0);
        long int secondsToStop = time(&t) + 30;
        while (time(&t) < secondsToStop) { //Show the image captured in the window and repeat
            inputVideo >> src; // read
            outputVideo.write(src);
        }
        outputVideo.release();
        inputVideo.release();
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
    const long hours = td.hours();
    const long minutes = td.minutes();
    const long seconds = td.seconds();
    const long milliseconds = td.total_milliseconds() -((hours * 3600 + minutes * 60 + seconds) * 1000);
    tm t = to_tm(now);
    string test = "C" + to_string(this->ID) + "-" + to_string(t.tm_year + 1900);
    if (t.tm_mon + 1 < 10) {
        test += "0" + to_string(t.tm_mon + 1);
    } else {
        test += to_string(t.tm_mon + 1);
    }
    if (t.tm_mday < 10) {
        test += "0" + to_string(t.tm_mday);
    } else {
        test += to_string(t.tm_mday);
    }
    test += "-";
    if (hours < 10) {
        test += "0" + to_string(hours);
    } else {
        test += to_string(hours);
    }
    if (minutes < 10) {
        test += "0" + to_string(minutes);
    } else {
        test += to_string(minutes);
    }
    if (seconds < 10) {
        test += "0" + to_string(seconds);
    } else {
        test += to_string(seconds);
    }
    if (milliseconds < 10) {
        test += "00" + to_string(milliseconds);
    } else if (milliseconds < 100) {
        test += "0" + to_string(milliseconds);
    } else {
        test += to_string(milliseconds);
    }
    return test + ".mp4";
}