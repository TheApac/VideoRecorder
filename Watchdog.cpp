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
    string line = "";
    while (1) {
        if (!fileExists(string(pw->pw_dir) + "/.RunningVideoRecorder")) {
            pid_t pid = fork();
            if (pid <= 0) {
                Manager *manager = new Manager();
                manager->run();
            }
        } else {
            ifstream file(string(pw->pw_dir) + "/.RunningVideoRecorder");
            getline(file, line);
            if (secondsSinceDate(line) > 60 * 5) {
                //ERREUR
                pid_t pid = fork();
                if (pid <= 0) {
                    Manager *manager = new Manager();
                    manager->run();
                }
            }
            line = "";
            file.close();
        }
        sleep(60);
    }
}

Watchdog::Watchdog(const Watchdog & orig) {
}

Watchdog::~Watchdog() {
}

void Watchdog::watch() {

}