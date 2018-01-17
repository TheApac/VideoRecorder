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
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "Camera.h"
#include "Utility.h"
#include "ManagerVideo.h"
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write


using namespace std;

int main(int argc, char** argv) {
    time_t t = time(0);
    long int secondsToStop = time(&t) + 30;
    while (time(&t) < secondsToStop) {
        printf("Current sec : %ld\n", time(&t));
        printf("Sec to stop : %ld\n", secondsToStop);
        sleep(5);
    }

    return EXIT_SUCCESS;
}