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
#include <iostream>
extern "C" {
#include <sodium.h>
}
#include <unistd.h>
#include <signal.h>

using namespace std;

int main() {
    int error = configureSMTP();
    if (error == EXIT_FAILURE) {
        cerr << "Config for SMTP is erroneous" << endl;
        exit(EXIT_FAILURE);
    }
    try {
        Manager *manager = new Manager();
        if (manager != nullptr) {
            printf("Manager created\n");
        }
    } catch (CustomException &e) {
        cerr << e.what() << endl;
        string error = "File ~/.VideoRecorder/cameras.ini was not found on the server : ";
        char hostname[128] = "";
        gethostname(hostname, sizeof (hostname));
        error += string(hostname) + "\n";
        sendEmail(error);
        exit(EXIT_FAILURE);
    }
    if (sodium_init() == -1) {
        cout << "Erreur libsodium" << endl;
    } else {
        cout << "Libsodium ok" << endl;
    }
}