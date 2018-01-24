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
#include <curl/curl.h>

using namespace std;

bool isOnlyNumeric(string &str) {
    for (int positionChar = 0; positionChar < str.size() - 1; ++positionChar) { // iterate through the string
        if (str.at(positionChar) < '0' || str.at(positionChar) > '9') { // if a non-digit char is found
            return false;
        }
    }
    return true; // return true if end of function is reached
}

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    struct upload_status *upload_ctx = (struct upload_status *) userp;
    const char *data;
    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
        return 0;
    }
    const char *payload_text[] = {// table with every element to send
        defineDate().c_str(),
        "To: " TO_MAIL "\r\n", //sender
        "From: " FROM_MAIL "\r\n", //recipient
        "Message-ID: <error-ManagerVideo@no-reply@2n-tech.com>\r\n",
        "Subject: An error occured\r\n", // Object of the email
        "\r\n", /* empty line to divide headers from body*/
        /* start of the email body
         Could add as many line as wanted, even empty ones*/
        "An error as occured.\r\n",
        /* End of email body*/
        NULL //end of the email
    };
    data = payload_text[upload_ctx->lines_read];
    if (data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        upload_ctx->lines_read++;
        return len;
    }
    return 0;
}

static string defineDate() {
    // return the date line to include in the mail
    string date = "";
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    switch (now->tm_wday) { // define the day to write depending of its number in the week
        case 0: date = "Sun"; //week start on sunday
            break;
        case 1: date = "Mon";
            break;
        case 2: date = "Tue";
            break;
        case 3: date = "Wed";
            break;
        case 4: date = "Thu";
            break;
        case 5: date = "Fri";
            break;
        case 6: date = "Sat";
    }
    date += ", " + to_string(now->tm_mday) + " ";
    switch (now->tm_mon) { // define the month to write depending of its number in the year
        case 0: date += "Jan";
            break;
        case 1: date += "Feb";
            break;
        case 2: date += "Mar";
            break;
        case 3: date += "Apr";
            break;
        case 4: date += "May";
            break;
        case 5: date += "Jun";
            break;
        case 6: date += "Jul";
            break;
        case 7: date += "Aug";
            break;
        case 8: date += "Seb";
            break;
        case 9: date += "Oct";
            break;
        case 10: date += "Nov";
            break;
        case 11: date += "Dec";
    }
    date += " " + to_string(now->tm_year + 1900) + " "; // the year is the number of year between 1900 and today
    if (now->tm_hour > 10) { // add a 0 in front of the hour if current time between 0 and 9
        date += to_string(now->tm_hour);
    } else {
        date += "0" + to_string(now->tm_hour);
    }
    date += ":"; //separate hour and minute
    if (now->tm_min > 10) { // add a 0 in front of the minute if current time between 0 and 9
        date += to_string(now->tm_min);
    } else {
        date += "0" + to_string(now->tm_min);
    }
    date += ":"; //separate minute and second
    if (now->tm_sec > 10) { // add a 0 in front of the second if current time between 0 and 9
        date += to_string(now->tm_sec);
    } else {
        date += "0" + to_string(now->tm_sec);
    }
    return "Date :" + date + "\r\n"; // format the date for an email
}

int sendEmail() {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;

    upload_ctx.lines_read = 0;

    curl = curl_easy_init();
    if (curl) {
        /* This is the configuration of the mailserver */
        curl_easy_setopt(curl, CURLOPT_USERNAME, "no-reply@2n-tech.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "hnO4vbpl54pw0PIQ"); // TODO encryption
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://ssl0.ovh.net");

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_ADDR);
        /* Add recipient, in this particular case they correspond to the
         * To: addresse in the header */
        recipients = curl_slist_append(recipients, TO_ADDR);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        /* We're using a callback function to specify the payload (the headers and
         * body of the message).*/
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        /* Send the message */
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }
    return (int) res;
}

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
    umask(0); //remove every right to the process
    chdir("/"); // change directory to root
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x); //close every system file possibly opened by the process
    }
}

void createDirectoryVideos(string rootDirectory) {
    struct stat info;
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    string month = to_string(now->tm_mon + 1);
    if (strlen(month.c_str()) < 2) {
        month = "0" + month; // if month < 10 we add a 0 in front of the number
    }
    string day = to_string(now->tm_mday);
    if (strlen(day.c_str()) < 2) {
        day = "0" + day; // if day < 10 we add a 0 in front of the number
    }
    string date = rootDirectory + "/" + std::to_string(now->tm_year + 1900) + "." + month + "." + day; //get current date
    string hour = date + "/H" + to_string(now->tm_hour); //get current hour
    if (stat(date.c_str(), &info) != 0) {
        mkdir(date.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory YYYY.MM.DD
    }
    mkdir(hour.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory "H"+HH in previously created directory
}

static int timeSinceDate(string dateToCompare) {
    // Calculate the number of days ellapsed since a date formated as "YYYY:MM:DD"
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime); // create a time structure
    timeinfo->tm_year = atoi(dateToCompare.substr(0, 4).c_str()) - 1900;
    timeinfo->tm_mon = atoi(dateToCompare.substr(5, 7).c_str()) - 1;
    timeinfo->tm_mday = atoi(dateToCompare.substr(8).c_str());
    time_t x = mktime(timeinfo); // create a time_t struct from the timeinfo
    time_t raw_time = time(NULL); // create a time_t struct with current time
    time_t y = mktime(localtime(&raw_time));
    double difference = difftime(y, x) / (60 * 60 * 24); // calculate the number of ms between two dates, convert it in days
    return difference;
}

static void removeContentOfDirectory(string path, bool exact) {
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    DIR *dir;
    struct dirent *ent; // will be used to store every entity in the directory
    if ((dir = opendir(path.c_str())) != NULL) { // try to open the directory
        while ((ent = readdir(dir)) != NULL) { // iterate through its content
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) { //Do not iterate on current and parent directories
                continue;
            }
            string entryName = ent->d_name; // we store the entry name in a string
            if (ent->d_type == DT_DIR && !exact) { // if the date is higher than nbDays remove everything in every subdirectories
                removeContentOfDirectory(path + "/" + ent->d_name, true);
                string dirToRemove = path + "/" + ent->d_name;
                remove(dirToRemove.c_str()); // remove the subdirectory
            } else if (ent->d_type == DT_DIR && exact && now->tm_hour > atoi(entryName.substr(entryName.size() - 2).c_str())) { // else only remove when the hour indicated is lower than current hour
                removeContentOfDirectory(path + "/" + ent->d_name, true);
                string dirToRemove = path + "/" + ent->d_name;
                remove(dirToRemove.c_str()); // remove the subdirectory
            } else if (ent->d_type != DT_DIR) {
                string fileToRemove = path + "/" + ent->d_name;
                remove(fileToRemove.c_str()); // if it's not a directory, remove it
            }
        }
        closedir(dir); //close the directory to prevent any memory leak
    } else {
        // Set the error message in case of bug
        perror("Could not open the directory");
    }
}

int removeOldFile(int nbDays, string path) {
    //remove every file under path, older than nbDays
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        // check all the files and directories within directory
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                continue; //Do not iterate on current and parent directories
            }
            // check if the directory is older than nbDays
            if (timeSinceDate(ent->d_name) > nbDays) {
                string folderToRemove = path + "/" + ent->d_name; //absolute path to directory
                removeContentOfDirectory(folderToRemove, false); // empty the directory
                remove(folderToRemove.c_str()); // remove the directory
            }
            if (timeSinceDate(ent->d_name) == nbDays) { // if date exactly equal to nbDays, we don't delete files where hour is higher than current hour
                string folderToRemove = path + "/" + ent->d_name; //absolute path to directory
                removeContentOfDirectory(folderToRemove, true); // remove files that need to be removed
            }
        }
        closedir(dir); //close the directory to prevent any memory leak
        return EXIT_SUCCESS;
    } else {
        // Set the error message in case of bug
        perror("Could not open the directory");
        return EXIT_FAILURE;
    }
}