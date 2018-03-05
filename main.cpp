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
    //    int error = configureSMTP();
    //    if (error == EXIT_FAILURE) {
    //        cerr << "Config for SMTP is erroneous" << endl;
    //        exit(EXIT_FAILURE);
    //    }
    //
    //    string name = "test", log = "admin", password = "2NTech-Lyon", url = "192.168.0.212:4503/Stream1", path = "/home/Alexandre/testDirectory";
    //    int ID = 1, nbdays = 1;
    //
    //    Camera* cameraTest = new Camera(path, ID, nbdays, name, log, password, url);
    //    cameraTest->record();


    AVFormatContext* context = avformat_alloc_context();
    int video_stream_index;

    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    //open rtsp
    if (avformat_open_input(&context, "rtsp://admin:2NTech-Lyon@192.168.0.212:4503/Stream1", NULL, NULL) != 0) {
        return EXIT_FAILURE;
    }

    if (avformat_find_stream_info(context, NULL) < 0) {
        return EXIT_FAILURE;
    }

    //search video stream
    for (int i = 0; i < context->nb_streams; i++) {
        if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            video_stream_index = i;
    }

    AVPacket packet;
    av_init_packet(&packet);

    //open output file
    AVOutputFormat* fmt = av_guess_format(NULL, "test.mp4", NULL);
    AVFormatContext* oc = avformat_alloc_context();
    oc->oformat = fmt;
    avio_open2(&oc->pb, "test.mp4", AVIO_FLAG_WRITE, NULL, NULL);

    AVStream* stream = NULL;

    av_read_play(context); //play RTSP
    time_t t = time(0);
    long int secondsToStop = time(&t) + 60 + 5;
    long int secondsToWait = time(&t) + 5;
    av_read_frame(context, &packet);
    if (stream == NULL) {//create stream in file
        stream = avformat_new_stream(oc, context->streams[video_stream_index]->codec->codec);
        avcodec_copy_context(stream->codec, context->streams[video_stream_index]->codec);
        stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio;
        int error = avformat_write_header(oc, NULL);
    }
    while (av_read_frame(context, &packet) >= 0 && time(&t) < secondsToWait) {
    }

    while (av_read_frame(context, &packet) >= 0 && time(&t) < secondsToStop) { // Loop while the file is not at its max time
        if (packet.stream_index == video_stream_index) {//packet is video
            av_write_frame(oc, &packet);
        }
        av_free_packet(&packet);
        av_init_packet(&packet);
    }

    av_write_trailer(oc);
    avio_close(oc->pb);
    avformat_free_context(oc);

    return (EXIT_SUCCESS);




    //pid_t pid = fork();
    //if (pid == 0) {
    //  Manager *manager = new Manager();
    // manager->run();
    //} else {
    //Watchdog *watchdog = new Watchdog();
    //}
}