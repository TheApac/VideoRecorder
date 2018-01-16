/* License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <time.h>
#include <string>
#include "Utility.h"

using namespace std;

void deamonize() {
    pid_t pid;
    /* Fork off the parent process */
    pid = fork();
    /* An error occurred */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    /* On success: The child process becomes session leader */
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    chdir("/");
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }
}

void createDirectoryVideos(string rootDirectory) {
    struct stat info;
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    string month = to_string(now->tm_mon + 1);
    if (strlen(month.c_str()) < 2) {
        month = "0" + month;
    }
    string day = to_string(now->tm_mday);
    if (strlen(day.c_str()) < 2) {
        day = "0" + day;
    }
    string date = rootDirectory + "/" + std::to_string(now->tm_year + 1900) + "." + month + "." + day; //get current date
    string hour = date + "/H" + to_string(now->tm_hour); //get current hour
    if (stat(date.c_str(), &info) != 0) {
        mkdir(date.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory YYYY.MM.DD
    }
    mkdir(hour.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory "H"+HH in previously created directory
}

int timeSinceDate(string dateToCompare) {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    timeinfo->tm_year = atoi(dateToCompare.substr(0, 4).c_str()) - 1900;
    timeinfo->tm_mon = atoi(dateToCompare.substr(5, 7).c_str()) - 1;
    timeinfo->tm_mday = atoi(dateToCompare.substr(8).c_str());
    time_t x = mktime(timeinfo);
    time_t raw_time = time(NULL);
    time_t y = mktime(localtime(&raw_time));
    double difference = difftime(y, x) / (60 * 60 * 24);
    return difference;
}

void removeContentOfDirectory(string path) {
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // check if the hour is passed
            if (now->tm_hour > atoi(path.substr(path.size() - 2).c_str())) { //get only the 2 digits for the hour
                DIR *subdir;
                subdir = opendir(ent->d_name);
                struct dirent *subent;
                while ((subent = readdir(subdir)) != NULL) {
                    remove(subent->d_name);
                }
                closedir(subdir);
                remove(ent->d_name);
            }
        }
        closedir(dir);
    } else {
        // could not open directory
        perror("");
    }
}

int removeOldFile(int nbDays, string path) {
    //remove every file under path, older than nbDays
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        // check all the files and directories within directory
        while ((ent = readdir(dir)) != NULL) {
            // check if the directory is older than nbDays
            if (strlen(ent->d_name) > 3 && timeSinceDate(ent->d_name) > nbDays) {
                string folderToRemove = path + "/" + ent->d_name; //absolute path to directory
                removeContentOfDirectory(folderToRemove);
                remove(folderToRemove.c_str()); // remove the directory
            }
        }
        closedir(dir);
        return EXIT_SUCCESS;
    } else {
        // could not open directory
        perror("");
        return EXIT_FAILURE;
    }
}