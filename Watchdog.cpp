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

bool Watchdog::stopping = false;

Watchdog::Watchdog() {
    cout << "Watchdog start" << endl;
    deamonize();
    sleep(60);
    stopping = false;
    signal(SIGINT, signalHandler);
    string fileName = "/var/www/html/public/.infos.dat";
    string line = "";
    string lastMail = "2018:08:20:00:00:00";
    while (!stopping) {
        if (fileExists(fileName)) {
            ifstream file(fileName);
            if (file.is_open()) {
                getline(file, line);
                if (secondsSinceDate(line) > 180) {
                    if (secondsSinceDate(lastMail) > DEFAULT_TIME_BETWEEN_MAILS) {
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
                        struct passwd *pw = getpwuid(getuid());
                        string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorder";
                        string logFileName = directoryOfFiles + "/logs"; // Path to log file
                        outFile.open(logFileName, std::ios::app);
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
                    if (secondsSinceDate(lastMail) > DEFAULT_TIME_BETWEEN_MAILS) {
                        lastMail = currentDate();
                        sendEmail("Le manager du serveur ne répondait plus");
                    }
                    remove(fileName.c_str());
                    setLocation("");
                    std::ofstream outFile;
                    struct passwd *pw = getpwuid(getuid());
                    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorder";
                    string logFileName = directoryOfFiles + "/logs"; // Path to log file
                    outFile.open(logFileName, std::ios::app);
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

void Watchdog::signalHandler(int signum) {
    stopping = true;
    sleep(3);
    exit(signum);
}