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
#include <vector>
#include <map>

#define DEFAULT_TIME_BETWEEN_RECORDS 30 // seconds to wait for each camera after the previous one : 30
#define DEFAULT_TIME_RECORDS 900 // seconds to record by default : 900 (15min)
#define DEFAULT_DAYS_TO_KEEP 30 // numbers of day before a file must be deleted : 30
#define DEFAULT_TIME_BUFFER_MOVE 60 // number of minutes before the process move the recorded files : 60
#define AVERAGE_FILE_SIZE 10 // average size of a minute of video record in Mo : 10
#define TIME_BEFORE_NEW_RECORD 6 // number of seconds left in a record before a new one is started : 6

using namespace std;

struct upload_status {
    int lines_read;
};

struct node_t {
    string value;
    struct node_t* next;
};

struct bufferDir {
    int nbMin;
    string defDir;
    map<int, string> listBuffer;
};

static string runningBufferMove = "2000:01:01:00:00:00"; // Make sure thread to move files is still running
static vector<bufferDir> bufferDirList; // Keep info for each final directories
void startMoveFromBuffer(int nbdays);
void MoveForEachDir(string defDir, int nbdays);
void addBufferDir(int nbmin, string defDir, string tempDir, int ID);
static vector<string> CrashedCameraList; // Vector that store which camera crashed when
static struct node_t* RunningCameraList; // keep the cameras running
bool IsInRunningList(string ID);
void deleteNode(string valueToDelete);
void addRunningCamera(string ID);
bool isOnlyNumeric(string & str); // Return true if the string represent a positive integer, else return false
int sendEmail(string messageContent); // send an email containing the parameter
static string defineDate(); // Retuen the date on the format of the email parameter
void deamonize(); // Self explaining
string createDirectoryVideos(string rootDirectory); // Create a subdirectory (and the directory if not existent) where records will be stored
static int timeSinceDate(string dateToCompare); // return the number of days since a date
static void removeContentOfDirectory(string path, bool exact); // remove every files in a directory (recursive)
int removeOldFile(int nbDays, string path); // remove the files that are older than the maximum time to keep
int configureSMTP(); // configure the SMTP for curl to send mails
bool fileExists(const string & name); // check if the file at the path "name" exists
string currentDate(); // return the current date at the format "YYYY:MM:DD:HH::mm::ss"
int secondsSinceDate(string dateToCompare); /* Calculate the number of seconds ellapsed since a date formated as "YYYY:MM:DD:HH:MM:SS" */
bool setLocation(string location); /* Saves the geo location of the manager for the emails */
bool isRunningManager(); /* Check if there is not already a manager running */
bool didCameraCrash(int ID); // Check if the camera of ID given in parameter crashed less than 10minutes ago
void removeOldCrashedCameras(); // Remove from the crashed camera list, the ones that crashed more than 10min ago
int timeSinceCrashCamera(int IDCam); // Return the number of seconds since the camera of ID IDCam crashed
void addCrashedCamera(int ID); // Add a camera to the list, with the time it crashed
void moveFromBufferMemory(string &defDir, string tempDir, int IDCam); // Move every file saved by camera IDCam from tempDir to defDir
int secondsSinceRecord(string fileName); // Return the number of seconds since the file was recorded
int getSizeListBuffDir(); // Return the size of the list that keeps the bufferDir structs
string getPathForCameraID(int ID); // Return the path where the record of camera of ID given as parameter is saved
void addLog(string log); // Add a line in the log file
string getAvError(int& errorCode); // Return a string with explanation of AVERROR from its code
long int remainingFreeSpace(string path); // Return the number of free Mo at the path given in parameter

#endif /* UTILITY_H */