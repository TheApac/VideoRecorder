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
#include "Manager.h"
#include "Camera.h"
#include "Utility.h"
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;

Camera::Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url/*, Manager* manager*/) {
    //deamonize();
    this->directory = path;
    this->nbdays = nbdays;
    this->ID = ID;
    this->name = name;
    this->log = log;
    this->password = password;
    this->url = url;
    this->manager = manager;
    this->timeOfLastCrash = "1900:01:01:00:00:00";
}

void Camera::record() {
    //    string destinationDirectory = createDirectoryVideos(this->directory);
    //    removeOldFile(1, this->directory);
    //    string link = "rtsp://" + this->log + ":" + this->password + "@" + this->url;
    //    VideoCapture inputVideo(link); //open the stream with the identification
    //    if (!inputVideo.isOpened()) {
    //        sendEmail("Couldn't connect to the camera " + to_string(ID) + " of url " + url);
    //        exit(EXIT_FAILURE);
    //    }
    //    inputVideo.set(CV_CAP_PROP_FOURCC, CV_FOURCC('H', '2', '6', '4'));
    //    Size size = Size(1920, 1080); // Acquire input size
    //    VideoWriter outputVideo; // Open the output
    //    int fourcc = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
    //    outputVideo.open(destinationDirectory + "/" + this->getFileName(), CV_FOURCC('H', '2', '6', '4'), int(15), size, true); //create an output file
    //    if (!outputVideo.isOpened()) {
    //        cerr << "Could not open the output video to write: " << this->url << endl;
    //        exit(EXIT_FAILURE);
    //    }
    //    Mat src; // Image type
    //    time_t t = time(0);
    //    long int secondsToStop = time(&t) + 60 * 2;
    //    bool recordNext = false;
    //    while (time(&t) < secondsToStop) { // Loop while the file is not at its max time
    //        //fourcc = static_cast<int> (inputVideo.get(CV_CAP_PROP_FOURCC));
    //        //if (fourcc == 0) {
    //        // if (secondsSinceDate(this->timeOfLastCrash) < 10 * 60) {
    //        // cout << "aftercreate" << endl;
    //        // sendEmail("The camera " + to_string(ID) + " of url " + url + " didn't receive any incoming stream");
    //        // this->timeOfLastCrash = currentDate();
    //        //            }
    //        //}
    //        inputVideo >> src; // read an image
    //        outputVideo.write(src); // write it in the output file
    //        if (!recordNext && time(&t) + 15 == secondsToStop) {
    //            pid_t pid = fork();
    //            if (pid == 0) {
    //                deamonize();
    //                this->record();
    //            } else if (pid != 0) {
    //                recordNext = true;
    //            }
    //        }
    //        //if (find(this->manager->RunningCameraList.begin(), this->manager->RunningCameraList.end(), to_string(this->ID)) != this->manager->RunningCameraList.end()) {
    //        //    this->manager->RunningCameraList.push_back(to_string(this->ID));
    //        //}
    //    }
    //    outputVideo.release(); // close the output writer
    //    inputVideo.release(); // close the video reader
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
    return FileName + ".avi"; //add the extension
}