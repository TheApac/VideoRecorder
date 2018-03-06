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
#include "Utility.h"
#include "Manager.h"
#include "CustomException.h"
#include "Watchdog.h"
#include <iostream>
extern "C" {
#include <sodium.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <unistd.h>
#include <fstream>
#include <pwd.h>
#include <string.h>

using namespace std;

int main() {
    //    struct passwd *pw = getpwuid(getuid());
    //    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    //    string toRemove = directoryOfFiles + "/.RunningVideoRecorder";
    //    remove(toRemove.c_str());
    int error = configureSMTP();
    if (error == EXIT_FAILURE) {
        cerr << "Config for SMTP is erroneous" << endl;
        exit(EXIT_FAILURE);
    }
    //
    //    string name = "test", log = "admin", password = "2NTech-Lyon", url = "192.168.0.212:4503/Stream1", path = "/home/Alexandre/testDirectory";
    //    int ID = 1, nbdays = 1;
    //
    //    Camera* cameraTest = new Camera(path, ID, nbdays, name, log, password, url);
    //    cameraTest->record();

    int video_stream_index; // keep the index of the video stream
    AVFormatContext* context = avformat_alloc_context();
    AVPacket packet; // packet sent by the camera

    av_init_packet(&packet);
    av_register_all(); // Initialize libavformat and register all the muxers
    avcodec_register_all(); // Register all the codecs, parsers and bitstream filters

    if (avformat_open_input(&context, "rtsp://admin:2NTech-Lyon@192.168.0.211:4502/Stream1", NULL, NULL) != 0) { //open rtsp
        sendEmail("Couldn't connect to the camera " + string("1") + " of url " + "192.168.0.211:4502/Stream1"); // mail if camera is unreachable
        return EXIT_FAILURE;
    }
    if (avformat_find_stream_info(context, NULL) < 0) { // retrieve informations of the stream
        sendEmail("Couldn't retrieve informations for the camera " + string("1") + " of url " + "192.168.0.211:4502/Stream1");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < context->nb_streams; i++) { //search video stream
        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }

    AVOutputFormat* fmt = av_guess_format(NULL, "test.mp4", NULL); //create format for given file
    AVFormatContext* oc = avformat_alloc_context();
    oc->oformat = fmt;
    avio_open2(&oc->pb, "test.mp4", AVIO_FLAG_WRITE, NULL, NULL); //open output file

    AVStream* stream = NULL;
    stream = avformat_new_stream(oc, context->streams[video_stream_index]->codec->codec); // save which stream to mux
    avcodec_copy_context(stream->codec, context->streams[video_stream_index]->codec); // set infos to the camera's ones
    stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio; //set the file dimensions ratio
    int errorRSTP = avformat_write_header(oc, NULL); // write the header in the out file

    bool recordNext = false; // start only one other recording
    time_t t = time(0);
    long int secondsToStop = time(&t) + 30; // save when to stop recording this video
    while (time(&t) < secondsToStop) { // Loop while the file is not at its max time and frames are available
        if (av_read_frame(context, &packet) < 0) {
            // if (secondsSinceDate(this->timeOfLastCrash) < 10 * 60) { // Don't send 2 mails in less than 10 min
            sendEmail("The camera " + string("1") + " of url " + "192.168.0.211:4502/Stream1" + " stopped sending informations");
            //this->timeOfLastCrash = currentDate(); // set the time of last mail to now
            //}
        }
        if (packet.stream_index == video_stream_index) { //check if the packet is a video
            av_write_frame(oc, &packet); // write the frame in the out file
        }
        //if (!recordNext && time(&t) + 15 == secondsToStop) {
        //            pid_t pid = fork();
        //            if (pid == 0) {
        //                deamonize();
        //                this->record();
        //            } else if (pid != 0) {
        //                recordNext = true;
        //            }
        //        }
        //if (find(this->manager->RunningCameraList.begin(), this->manager->RunningCameraList.end(), to_string(this->ID)) == this->manager->RunningCameraList.end()) {
        //            this->manager->RunningCameraList.push_back(to_string(this->ID));
        //        }
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
    return (EXIT_SUCCESS);

    //pid_t pid = fork();
    //if (pid == 0) {
    //  Manager *manager = new Manager();
    // manager->run();
    //} else {
    //Watchdog *watchdog = new Watchdog();
    //}
}