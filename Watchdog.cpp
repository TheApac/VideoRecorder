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
#include <iostream>
#include <signal.h>

Watchdog::Watchdog() {
    //deamonize();
    sleep(60);
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string fileName = directoryOfFiles + "/.RunningVideoRecorder";
    string line = "";
    string lastMail = "2000:01:01:00:00:00";
    while (1) {
        if (fileExists(fileName)) {
            ifstream file(fileName);
            if (file.is_open()) {
                getline(file, line);
                if (secondsSinceDate(line) > 180) {
                    if (secondsSinceDate(lastMail) > 10 * 60) {
                        sendEmail("Le manager du serveur ne répondait plus");
                        lastMail = currentDate();
                    }
                    Camera::reinitTimeRecord();
                    signal(SIGCHLD, SIG_IGN);
                    pid_t pid = fork();
                    if (pid == 0) {
                        file.close();
                        remove(fileName.c_str());
                        setLocation("");
                        std::ofstream outFile;
                        outFile.open("/home/Alexandre/Documents/crashLog.txt", std::ios::app);
                        outFile << "MANAGER CRASHED" << currentDate() << endl;
                        outFile.close();
                        auto manager = make_shared<Manager>();
                        manager->startRecords();
                        exit(EXIT_SUCCESS);
                    } else {
                        sleep(120);
                    }
                }
                file.close();
            }
        } else {
            sleep(5);
            if (!fileExists(fileName)) {
                signal(SIGCHLD, SIG_IGN);
                Camera::reinitTimeRecord();
                pid_t pid = fork();
                if (pid == 0) {
                    if (secondsSinceDate(lastMail) > 10 * 60) {
                        sendEmail("Le manager du serveur ne répondait plus");
                        lastMail = currentDate();
                    }
                    lastMail = currentDate();
                    remove(fileName.c_str());
                    setLocation("");
                    std::ofstream outFile;
                    outFile.open("/home/Alexandre/Documents/crashLog.txt", std::ios::app);
                    outFile << "MANAGER CRASHED" << currentDate() << endl;
                    outFile.close();
                    auto manager = make_shared<Manager>();
                    manager->startRecords();
                    exit(EXIT_SUCCESS);
                }
            }
        }
        line = "";
        sleep(60);
    }
}

Watchdog::~Watchdog() {
}