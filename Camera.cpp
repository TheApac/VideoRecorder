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
extern "C" {
#include <sodium.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <boost/thread.hpp>
using namespace std;
using namespace boost;

Camera::Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url) {
    if (SecondsToRecord == -1) {
        SecondsToRecord = DEFAULT_TIME_RECORDS; // 900 (15 * 60) : Default time 15 minutes
    }
    if (path.at(path.length() - 1) == '/') { // add a "/" at the end of the path if there is none
        this->directory = path;
    } else {
        this->directory = path + "/";
    }
    this->nbdays = nbdays;
    this->ID = ID;
    this->name = name;
    this->log = log;
    this->password = password;
    this->url = url;
    this->timeOfLastCrash = "1900:01:01:00:00:00";
}

void Camera::record() {
    string destinationDirectory = createDirectoryVideos(this->directory);
    removeOldFile(1, this->directory);
    string link = "rtsp://" + this->log + ":" + this->password + "@" + this->url;
    int video_stream_index; // keep the index of the video stream
    AVFormatContext* context = avformat_alloc_context();
    AVPacket packet; // packet sent by the camera

    av_init_packet(&packet);
    av_register_all(); // Initialize libavformat and register all the muxers
    avcodec_register_all(); // Register all the codecs, parsers and bitstream filters
    bool error = false;
    if (avformat_open_input(&context, link.c_str(), NULL, NULL) != 0) { //open rtsp
        sendEmail("Couldn't connect to the camera " + to_string(this->ID) + " of url " + this->url); // mail if camera is unreachable
        error = true;
    }
    if (!error) {
        if (avformat_find_stream_info(context, NULL) < 0) { // retrieve informations of the stream
            sendEmail("Couldn't retrieve informations for the camera " + to_string(this->ID) + " of url " + this->url);
            error = true;
        }
    }
    if (!error) {
        for (int i = 0; i < context->nb_streams; i++) { //search video stream
            if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                video_stream_index = i;
        }
        AVOutputFormat* fmt = av_guess_format(NULL, "sample.mp4", NULL); //create format for given file
        AVFormatContext* oc = avformat_alloc_context();
        oc->oformat = fmt;
        string fullName = destinationDirectory + getFileName();
        avio_open2(&oc->pb, fullName.c_str(), AVIO_FLAG_WRITE, NULL, NULL); //open output file

        AVStream* stream = NULL;
        stream = avformat_new_stream(oc, context->streams[video_stream_index]->codec->codec); // save which stream to mux
        avcodec_copy_context(stream->codec, context->streams[video_stream_index]->codec); // set infos to the camera's ones
        stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio; //set the file dimensions ratio
        avformat_write_header(oc, NULL); // write the header in the out file
        bool recordNext = false; // start only one other recording
        time_t t = time(0);
        long int secondsToStop = time(&t) + SecondsToRecord; // save when to stop recording this video

        while (time(&t) < secondsToStop) { // Loop while the file is not at its max time and frames are available
            if (av_read_frame(context, &packet) < 0) {
                if (secondsSinceDate(this->timeOfLastCrash) < 10 * 60) { // Don't send 2 mails in less than 10 min
                    sendEmail("The camera " + to_string(this->ID) + " of url " + this->url + " stopped sending informations");
                    this->timeOfLastCrash = currentDate(); // set the time of last mail to now
                    break;
                }
            }
            if (packet.stream_index == video_stream_index) { //check if the packet is a video
                av_write_frame(oc, &packet); // write the frame in the out file
            }
            if (!recordNext && time(&t) + 5 == secondsToStop) {
                thread(&Camera::record, this);
                recordNext = true;
            }
            if (!IsInRunningList(to_string(this->ID))) {
                addRunningCamera(to_string(this->ID));
            }
            av_free_packet(&packet); // clear the packet
            av_init_packet(&packet); // init the packet
        }
        av_write_trailer(oc); // write the trailer in the out file
        // ---------- CLEANUP ---------- //
        avio_closep(&oc->pb);
        avformat_free_context(oc);
        av_free_packet(&packet);
        avformat_close_input(&context);
        // ----------------------------- //
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

volatile int Camera::SecondsToRecord = -1;

bool Camera::setSecondsToRecord(int sec) {
    if (sec < 0) {
        sendEmail("The number of seconds must be positive integer. Number given : " + to_string(sec));
    }
    if (SecondsToRecord == -1) {
        SecondsToRecord = sec;
        return true;
    } else {
        return false;
    }
}

void Camera::reinitTimeRecord() {
    SecondsToRecord = -1;
}