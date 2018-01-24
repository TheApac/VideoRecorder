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
#include "ManagerVideo.h"
#include "Manager.h"


using namespace std;

int main(int argc, char** argv) {
    //"/home/Alexandre/Téléchargements/cameras.ini"
    Manager* manager = new Manager("/home/Alexandre/Téléchargements/cameras.ini");
    if (manager != nullptr) {
        printf("Manager created");
    }
    manager->~Manager();
}