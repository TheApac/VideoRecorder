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
#include <unistd.h>
#include <pwd.h>
#include <fstream>

Watchdog::Watchdog() {
    deamonize();
    sleep(60);
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string fileName = directoryOfFiles + "/.RunningVideoRecorder";
    string line = "";
    while (1) {
        if (fileExists(fileName)) {
            ifstream file(fileName);
            if (file.is_open()) {
                getline(file, line);
                if (secondsSinceDate(line) > 60) {
                    sendEmail("Le manager du serveur ne répondait plus");
                    Camera::reinitTimeRecord();
                    pid_t pid = fork();
                    if (pid == 0) {
                        remove(fileName.c_str());
                        Manager *manager = new Manager();
                        manager->startRecords();
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            file.close();
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                sendEmail("Le manager du serveur n'a pas démarré");
                remove(fileName.c_str());
                Manager *manager = new Manager();
                manager->startRecords();
                exit(EXIT_SUCCESS);
            }
        }
        line = "";
        sleep(60);
    }
}

Watchdog::Watchdog(const Watchdog & orig) {
}

Watchdog::~Watchdog() {
}