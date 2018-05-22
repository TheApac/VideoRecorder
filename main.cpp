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
#include "Watchdog.h"
#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string.h>

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

using namespace std;

int main(int argc, char **argv) {
    string path = "/home/Alexandre/testDirectory";
    int in = 30;
    string log = string(argv[2]);
    string pwd = string(argv[3]);
    string url = string(argv[1]);
    Camera* cam = new Camera(path, in, in, path, log, pwd, url);
    cam->record();
    //    struct passwd *pw = getpwuid(getuid());
    //    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    //    string toRemove = directoryOfFiles + "/.RunningVideoRecorder";
    //    remove(toRemove.c_str());
    //    int error = configureSMTP();
    //    if (error == EXIT_FAILURE) {
    //        cerr << "Config for SMTP is erroneous" << endl;
    //        exit(EXIT_FAILURE);
    //    }
    //    pid_t pid = fork();
    //    if (pid == 0) {
    //        auto manager = make_shared<Manager>();
    //        if (manager) {
    //            pid_t pid = fork();
    //            if (pid == 0) {
    //                auto watchdog = make_shared<Watchdog>();
    //            } else {
    //                manager->startRecords();
    //            }
    //        }
    //    }
}