/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utility.h
 * Author: Alexandre
 *
 * Created on 16 janvier 2018, 09:23
 */

#ifndef UTILITY_H
#define UTILITY_H

#define FROM_ADDR    "<no-reply@2n-tech.com>"
#define TO_ADDR      "<a.cuevas@2n-tech.com>"
#define DATE_MAIL ""
#define FROM_MAIL "2N Technologies" FROM_ADDR
#define TO_MAIL   "Notified person " TO_ADDR

#include <string>

using namespace std;

struct upload_status {
    int lines_read;
};

int sendEmail();
static string defineDate();
void deamonize();
void createDirectoryVideos(string rootDirectory);
static int timeSinceDate(string dateToCompare);
static void removeContentOfDirectory(string path, bool exact);
int removeOldFile(int nbDays, string path);

#endif /* UTILITY_H */

