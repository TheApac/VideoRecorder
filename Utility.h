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
#define FROM_MAIL "2N Technologies" FROM_ADDR
#define TO_MAIL   "Notified person " TO_ADDR

#include <string>

using namespace std;

struct upload_status {
    int lines_read;
};

bool isOnlyNumeric(string &str); // Return true if the string represent a positive integer, else return false
int sendEmail(string messageContent);   // send an email containing the parameter
static string defineDate(); // Retuen the date on the format of the email parameter
void deamonize(); // Self explaining
void createDirectoryVideos(string rootDirectory); // Create a subdirectory (and the directory if not existent) where records will be stored
static int timeSinceDate(string dateToCompare); // return the number of days since a date
static void removeContentOfDirectory(string path, bool exact); // remove every files in a directory (recursive)
int removeOldFile(int nbDays, string path); // remove the files that are older than the maximum time to keep

#endif /* UTILITY_H */

