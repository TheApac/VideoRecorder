/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Watchdog.cpp
 * Author: Alexandre
 *
 * Created on 12 février 2018, 13:50
 */

#include "Watchdog.h"
#include "Utility.h"
#include "Manager.h"
#include "CustomException.h"
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <fstream>

Watchdog::Watchdog() {
    deamonize();
    sleep(30);
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string fileName = directoryOfFiles + "/.RunningVideoRecorder";
    string line = "";
    char hostname[128] = "";
    remove(fileName.c_str());
    while (1) {
        ifstream file(fileName);
        if (file.is_open()) {
            getline(file, line);
            if (secondsSinceDate(line) > 60) {
                gethostname(hostname, sizeof (hostname));
                sendEmail("Le manager du serveur " + string(hostname) + " ne répondait plus");
                Camera::reinitTimeRecord();
                pid_t pid = fork();
                if (pid == 0) {
                    remove(fileName.c_str());
                    Manager *manager = new Manager();
                    manager->startRecords();
                }
            }
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                Manager *manager = new Manager();
                manager->startRecords();
            }
        }
        line = "";
        file.close();
        sleep(60);
    }
}

Watchdog::Watchdog(const Watchdog & orig) {
}

Watchdog::~Watchdog() {
}