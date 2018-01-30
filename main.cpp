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
#include <iostream>
extern "C" {
#include <sodium.h>
}
using namespace std;

int main(int argc, char** argv) {
    Manager *manager = new Manager(string("/home/Alexandre/Téléchargements/cameras.ini"));
    if (manager != nullptr) {
        printf("Manager created\n");
    }
    if (sodium_init() == -1) {
        cout << "Erreur libsodium" << endl;
    } else {
        cout << "Libsodium ok" << endl;
    }
}