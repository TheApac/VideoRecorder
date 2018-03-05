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
#include <signal.h>
#include <pwd.h>

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
    av_register_all();
    if (avformat_open_input(&context, "rtsp://admin:2NTech-Lyon@192.168.0.212:4503/Stream1", NULL, NULL) != 0) {
        cout << "erreur open" << endl;
        return -1; // Couldn't open file
    }
    if (avformat_find_stream_info(context, NULL) > 0) {
        cout << "erreur find info" << endl;
        return -1;
    }
    AVStream* stream = NULL;
    av_read_play(context);

    //pid_t pid = fork();
    //if (pid == 0) {
    //  Manager *manager = new Manager();
    // manager->run();
    //} else {
    //Watchdog *watchdog = new Watchdog();
    //}
}