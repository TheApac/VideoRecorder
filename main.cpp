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
#include <signal.h>

using namespace std;

int main() {
    signal(SIGCHLD, SIG_IGN);
    string toRemove = "/var/www/html/public/.infos.dat";
    remove(toRemove.c_str());
    int error = configureSMTP();
    if (error == EXIT_FAILURE) {
        cerr << "Config for SMTP is erroneous" << endl;
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == 0) {
        auto manager = make_shared<Manager>();
        if (manager) {
            pid_t pid = fork();
            if (pid == 0) {
                auto watchdog = make_shared<Watchdog>();
            } else {
                manager->startRecords();
            }
        }
    }
    return 0;
}
