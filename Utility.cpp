/* License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "Utility.h"
#include <string>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include "CustomException.h"
#include <curl/curl.h>
#include <iostream>
#include <pwd.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

boost::mutex v_mutex;
boost::mutex mutex_crashed;

using namespace std;
using namespace boost;

string mailContent = "";
string loginSMTP = "";
string passwordSMTP = "";
string urlSMTP = "";
string SiteLocation = "";

class Camera {
public:
    static volatile int GetSecondsToRecord();
};

/* Verify if every char of a string is a number */
bool isOnlyNumeric(string &str) {
    for (int positionChar = 0; positionChar < str.size() - 1; ++positionChar) { // iterate through the string
        if (str.at(positionChar) < '0' || str.at(positionChar) > '9') { // true if anything but a number is seen
            return false; // return false if anything but a number is seen
        }
    }
    return true; // return true if end of function is reached
}

/* Return the date line to include in the mail */
static string defineDate() {
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
    if (now->tm_hour >= 10) { // add a 0 in front of the hour if current time between 0 and 9
        date += to_string(now->tm_hour);
    } else {
        date += "0" + to_string(now->tm_hour);
    }
    date += ":"; //separate hour and minute
    if (now->tm_min >= 10) { // add a 0 in front of the minute if current time between 0 and 9
        date += to_string(now->tm_min);
    } else {
        date += "0" + to_string(now->tm_min);
    }
    date += ":"; //separate minute and second
    if (now->tm_sec >= 10) { // add a 0 in front of the second if current time between 0 and 9
        date += to_string(now->tm_sec);
    } else {
        date += "0" + to_string(now->tm_sec);
    }
    return "Date :" + date + "\r\n"; // format the date for an email
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
        mailContent.c_str(),
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

int sendEmail(string messageContent) {
    addLog(messageContent);
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx;
    upload_ctx.lines_read = 0;
    curl = curl_easy_init();
    if (curl) {
        mailContent = messageContent + "\nTime of error : " + defineDate().substr(6);
        if (SiteLocation != "") {
            // If location is defined, add it as detail in the email
            mailContent += "\nConcerned site : " + SiteLocation;
        } else {
            // Else add the hostname of the server
            char hostname[128] = "";
            gethostname(hostname, sizeof (hostname));
            mailContent += "\nHostname of server : " + string(hostname);
        }
        /* This is the configuration of the mailserver */
        curl_easy_setopt(curl, CURLOPT_USERNAME, loginSMTP.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, passwordSMTP.c_str()); // TODO encryption
        curl_easy_setopt(curl, CURLOPT_URL, urlSMTP.c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_ADDR);
        /* Add recipient, in this particular case they correspond to the
         * To: addresse in the header */
        recipients = curl_slist_append(recipients, TO_ADDR);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        /* Callback function to specify the payload (the headers and body of the message).*/
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

string createDirectoryVideos(string rootDirectory) {
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
    string date = rootDirectory + std::to_string(now->tm_year + 1900) + "." + month + "." + day; //get current date
    string hour = date + "/H" + to_string(now->tm_hour) + "/"; //get current hour
    while (stat(date.c_str(), &info) != 0) {
        mkdir(date.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory YYYY.MM.DD
        sleep(1);
    }
    mkdir(hour.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create directory "H"+HH in previously created directory
    return hour;
}

/* Calculate the number of days ellapsed since a date formated as "YYYY:MM:DD" */
static int timeSinceDate(string dateToCompare) {
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

/* Calculate the number of seconds ellapsed since a date formated as "YYYY:MM:DD:HH:MM:SS" */
int secondsSinceDate(string dateToCompare) {
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime); // create a time structure
    timeinfo->tm_year = atoi(dateToCompare.substr(0, 4).c_str()) - 1900;
    timeinfo->tm_mon = atoi(dateToCompare.substr(5, 7).c_str()) - 1;
    timeinfo->tm_mday = atoi(dateToCompare.substr(8, 10).c_str());
    timeinfo->tm_hour = atoi(dateToCompare.substr(11, 13).c_str());
    timeinfo->tm_min = atoi(dateToCompare.substr(14, 16).c_str());
    timeinfo->tm_sec = atoi(dateToCompare.substr(17).c_str());
    time_t x = mktime(timeinfo); // create a time_t struct from the timeinfo
    time_t raw_time = time(NULL); // create a time_t struct with current time
    time_t y = mktime(localtime(&raw_time));
    double difference = difftime(y, x); // calculate the number of ms between two dates, convert it in days
    return difference;
}

/* Calculate the number of seconds ellapsed since a file was recorded */
int secondsSinceRecord(string fileName) {
    fileName = fileName.substr(fileName.find_first_of("-") + 1);
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime); // create a time structure
    timeinfo->tm_year = atoi(fileName.substr(0, 4).c_str()) - 1900;
    timeinfo->tm_mon = atoi(fileName.substr(4, 2).c_str()) - 1;
    timeinfo->tm_mday = atoi(fileName.substr(6, 2).c_str());
    timeinfo->tm_hour = atoi(fileName.substr(9, 2).c_str());
    timeinfo->tm_min = atoi(fileName.substr(11, 2).c_str());
    timeinfo->tm_sec = atoi(fileName.substr(13, 2).c_str());
    time_t x = mktime(timeinfo); // create a time_t struct from the timeinfo
    time_t raw_time = time(NULL); // create a time_t struct with current time
    time_t y = mktime(localtime(&raw_time));
    double difference = difftime(y, x); // calculate the number of ms between two dates, convert it in days
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
        perror("Could not open the directory"); // Set the error message in case of bug
    }
}

/* remove every file under path, older than nbDays */
int removeOldFile(int nbDays, string path) {
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
    } else { // Set the error message in case of bug
        perror("Could not open the directory");
        return EXIT_FAILURE;
    }
}

int configureSMTP() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    ifstream file(directoryOfFiles + "/ConfigFiles/2NWatchDog.ini");
    if (!file.is_open()) {
        cerr << directoryOfFiles << "/ConfigFiles/2NWatchDog.ini was not found" << endl;
    }
    string line;
    while (getline(file, line) && (urlSMTP == "" || loginSMTP == "" || passwordSMTP == "")) { // iterate through the file while the configuration isn't over
        if (line.find_first_of("=") != string::npos) { // Dealing differently with separation lines
            string parameterName = line.substr(0, line.find_first_of("="));
            if (parameterName == "serveur_smtp") {
                urlSMTP = "smtp://" + string(line.substr(line.find_first_of("=") + 1));
                urlSMTP = urlSMTP.substr(0, urlSMTP.size() - 1);
            } else if (parameterName == "login_smtp") {
                loginSMTP = line.substr(line.find_first_of("=") + 1);
                loginSMTP = loginSMTP.substr(0, loginSMTP.size() - 1);
            } else if (parameterName == "mdp_smtp") {
                passwordSMTP = line.substr(line.find_first_of("=") + 1);
                passwordSMTP = passwordSMTP.substr(0, passwordSMTP.size() - 1);
            }
        }
    }
    bool error = false;
    if (urlSMTP == "") {
        cerr << "No SMTP url specified" << endl;
        error = true;
    }
    if (loginSMTP == "") {
        cerr << "No SMTP login specified" << endl;
        error = true;
    }
    if (passwordSMTP == "") {
        cerr << "No SMTP password specified" << endl;
        error = true;
    }
    if (error)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

bool fileExists(const string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

/* Return the current date in a
 * YYYY:MM:DD:HH::mm::ss format*/
string currentDate() {
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t); //get local time
    string date = to_string(now->tm_year + 1900) + ":";
    if (now->tm_mon + 1 < 10) {
        date += "0";
    }
    date += to_string(now->tm_mon + 1) + ":";
    if (now->tm_mday < 10) {
        date += "0";
    }
    date += to_string(now->tm_mday) + ":";
    if (now->tm_hour < 10) {
        date += "0";
    }
    date += to_string(now->tm_hour) + ":";
    if (now->tm_min < 10) {
        date += "0";
    }
    date += to_string(now->tm_min) + ":";
    if (now->tm_sec < 10) {
        date += "0";
    }
    date += to_string(now->tm_sec);
    return date;
}

/* Saves the geo location of the manager for the emails */
bool setLocation(string location) {
    if (SiteLocation == "" || location == "") {
        SiteLocation = location;
        return true;
    }
    return false;
}

void addRunningCamera(string ID) {
    v_mutex.lock();
    struct node_t* newNode = new node_t;
    newNode->value = ID;
    newNode->next = RunningCameraList;
    RunningCameraList = newNode;
    v_mutex.unlock();
}

bool IsInRunningList(string ID) {
    v_mutex.lock();
    struct node_t *nodeSearch = nullptr;
    nodeSearch = RunningCameraList; // save the head of the list
    if (nodeSearch == NULL) {
        v_mutex.unlock();
        return false;
    }
    while (nodeSearch->value != ID && nodeSearch->next != NULL) { // While the node hasn't been found
        nodeSearch = nodeSearch->next;
    }
    bool returnValue = false;
    if (nodeSearch->value == ID) {
        returnValue = true;
    }
    v_mutex.unlock();
    return returnValue;
}

void deleteNode(string valueToDelete) {
    v_mutex.lock();
    struct node_t* nodeSearch = nullptr;
    struct node_t* previous = nullptr;
    nodeSearch = RunningCameraList;
    if (nodeSearch != nullptr) {
        if (nodeSearch->value == valueToDelete) {
            RunningCameraList = nodeSearch->next;
            nodeSearch = NULL;
        }
        while (nodeSearch != nullptr && nodeSearch->value != valueToDelete) {
            previous = nodeSearch;
            nodeSearch = nodeSearch->next;
        }
        if (nodeSearch != NULL) {
            previous->next = nodeSearch->next;
        }
    }
    v_mutex.unlock();
}

/* Check if there is not already a manager running */
bool isRunningManager() {
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    if (fileExists(directoryOfFiles + "/.RunningVideoRecorder")) {
        return true;
    }
    return false;
}

void removeOldCrashedCameras() {
    mutex_crashed.lock();
    vector<string> CrashedCameraListCopy = CrashedCameraList;
    int index = 0; //keep the position of the camera
    for (string cameraInfo : CrashedCameraListCopy) {
        if (secondsSinceDate(cameraInfo.substr(cameraInfo.find_first_of('-') + 1)) > 10 * 60) { // Erease if the camera crashed more than 10 minutes ago
            CrashedCameraListCopy.erase(CrashedCameraListCopy.begin() + index);
        } else { //Stay at the index if a delete was made
            ++index;
        }
    }
    CrashedCameraList = CrashedCameraListCopy;
    mutex_crashed.unlock();
}

bool didCameraCrash(int ID) {
    mutex_crashed.lock();
    int index = 0;
    while (index != CrashedCameraList.size()) {
        if (CrashedCameraList[index].substr(0, CrashedCameraList[index].find_first_of('-')) == to_string(ID)) {
            mutex_crashed.unlock();
            return true; // If the camera is found, exit the function
        }
        ++index;
    }
    mutex_crashed.unlock();
    return false;
}

int timeSinceCrashCamera(int IDCam) {
    if (!didCameraCrash(IDCam)) { // If it never crashed, return a large number
        addCrashedCamera(IDCam);
        return 9999999;
    }
    mutex_crashed.lock();
    int index = 0;
    while (index != CrashedCameraList.size()) { // iterate through the list
        if (CrashedCameraList.at(index).substr(0, CrashedCameraList.at(index).find_first_of('-')) == to_string(IDCam)) { // if good camera
            mutex_crashed.unlock();
            return secondsSinceDate(CrashedCameraList.at(index).substr(CrashedCameraList.at(index).find_first_of('-') + 1)); // return the number of second since crash
        }
        ++index;
    }
}

void addCrashedCamera(int ID) {
    if (!didCameraCrash(ID)) {
        mutex_crashed.lock();
        CrashedCameraList.push_back(to_string(ID) + "-" + currentDate());
        mutex_crashed.unlock();
    }
}

void startMoveFromBuffer(int nbdays) {
    if (bufferDirList.size() > 0) {
        int nbMin = bufferDirList.at(0).nbMin;
        for (bufferDir buff : bufferDirList) {
            thread(MoveForEachDir, buff.defDir, nbdays); // Start a thread for each final directory
        }
        sleep(nbMin);
    }
}

void MoveForEachDir(string defDir, int nbdays) {
    map<int, string> bufferMap;
    int nbMin;
    for (bufferDir buf : bufferDirList) { // Iterate to find the good bufferDir
        if (buf.defDir == defDir) { // Saves the infos
            bufferMap = buf.listBuffer;
            nbMin = buf.nbMin;
        }
    }
    while (1) { // every nbMin, move the files recorded from the right cam from each tempDir in the bufferMap to defDir
        runningBufferMove = currentDate();
        removeOldFile(nbdays, defDir);
        int nbdaysTemp = nbdays - 1;
        while (remainingFreeSpace(defDir) < AVERAGE_FILE_SIZE * Camera::GetSecondsToRecord() / 60) {
            sendEmail("Not enough space left to record in : " + defDir);
            removeOldFile(nbdaysTemp, defDir);
            nbdaysTemp = nbdaysTemp - 1;
        }
        for (auto const &buff : bufferMap) {
            moveFromBufferMemory(defDir, buff.second, buff.first); // buff.first : IDCam, buff.second : tempDir
        }
        sleep(nbMin);
    }
}

void moveFromBufferMemory(string& defDir, string tempDir, int IDCam) {
    DIR * dir;
    DIR * datedir;
    DIR * hourDir;
    string dateDirName;
    string hourDirName;
    struct dirent *entR;
    struct dirent *entD;
    struct dirent *entH;
    string oldName;
    string newName;
    dir = opendir(tempDir.c_str());
    while (entR = readdir(dir)) { //read "root" directory
        if (strcmp(entR->d_name, ".") == 0 || strcmp(entR->d_name, "..") == 0 || string(entR->d_name).substr(0, 2) != "20") {
            continue; //Do not iterate on current and parent directories
        }
        if (fileExists(tempDir + entR->d_name)) { // Prevent caching problems
            if (entR->d_type == DT_DIR) { // Iterate in subdirectories only
                dateDirName = tempDir + entR->d_name; // root/YYYY.MM.DD
                datedir = opendir(dateDirName.c_str());
                while (entD = readdir(datedir)) { // read "date" directory
                    if (strcmp(entD->d_name, ".") == 0 || strcmp(entD->d_name, "..") == 0 || string(entD->d_name).substr(0, 1) != "H") {
                        continue; //Do not iterate on current and parent directories
                    }
                    if (fileExists(tempDir + entR->d_name + "/" + entD->d_name)) {
                        if (entD->d_type == DT_DIR) { // Iterate in subdirectories only
                            hourDirName = tempDir + entR->d_name + "/" + entD->d_name; // root/YYYY.MM.DD/HX
                            hourDir = opendir(hourDirName.c_str());
                            while (entH = readdir(hourDir)) { // read "hour" directory
                                if (strcmp(entH->d_name, ".") == 0 || strcmp(entH->d_name, "..") == 0 || string(entH->d_name).substr(0, 1) != "C") {
                                    continue; //Do not iterate on current and parent directories
                                }
                                if (string(entH->d_name).substr(1, (string(entH->d_name).find_first_of("-") - 1)) == to_string(IDCam)) {
                                    if (secondsSinceRecord(entH->d_name) > Camera::GetSecondsToRecord() + 30) {
                                        oldName = tempDir + entR->d_name + "/" + entD->d_name + "/" + entH->d_name; // tempDir/YYYY.MM.DD/HX/C(IDCam)-YYYYMMDDD-HHmmssmmm.mp4
                                        newName = defDir + entR->d_name + "/" + entD->d_name + "/" + entH->d_name; // defDir/YYYY.MM.DD/HX/C(IDCam)-YYYYMMDDD-HHmmssmmm.mp4
                                        mkdir(defDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO); // create the dir if it doesn't exist
                                        string defdirR = defDir + entR->d_name + "/";
                                        mkdir(defdirR.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
                                        string defdirD = defdirR + entD->d_name + "/";
                                        mkdir(defdirD.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
                                        rename(oldName.c_str(), newName.c_str()); // move the file
                                    }
                                }
                            }
                            closedir(hourDir); // prevent memory leak
                            rmdir(hourDirName.c_str()); //remove directory if empty
                        }
                    }

                }
                closedir(datedir); // prevent memory leak
                rmdir(dateDirName.c_str()); //remove directory if empty
            }
        }
    }
    closedir(dir); // prevent memory leak
}

void addBufferDir(int nbmin, string defDir, string tempDir, int IDCam) {
    if (defDir.at(defDir.length() - 1) != '/') { // add a "/" at the end of the path if there is none
        defDir = defDir + "/";
    }
    if (tempDir.at(tempDir.length() - 1) != '/') { // add a "/" at the end of the path if there is none
        tempDir = tempDir + "/";
    }
    bool found = false;
    for (bufferDir bufferdirs : bufferDirList) { // try to find if an item exist for defDir
        if (bufferdirs.defDir == defDir) {
            found = true;
            bufferdirs.listBuffer[IDCam] = tempDir; // update its map
            break;
        }
    }
    if (!found) { // don't add it if exist
        bufferDir temp; // = new bufferDir;
        temp.defDir = defDir;
        temp.nbMin = nbmin;
        temp.listBuffer[IDCam] = tempDir;
        bufferDirList.push_back(temp); // Add a new item for defDir
    }
}

int getSizeListBuffDir() {
    return bufferDirList.size();
}

string getPathForCameraID(int ID) {
    int indexVec = 0;
    while (indexVec < getSizeListBuffDir()) {
        try {
            if (bufferDirList.at(indexVec).listBuffer[ID] != "") {
                return bufferDirList.at(indexVec).defDir;
            }
            indexVec++;
        } catch (std::exception const& e) {
            indexVec++;
        }
    }
    return "";
}

void addLog(string log) {
    std::ofstream out;
    struct passwd *pw = getpwuid(getuid());
    string directoryOfFiles = string(pw->pw_dir) + "/.VideoRecorderFiles";
    string logFileName = directoryOfFiles + "/.logs"; // Path to log file
    out.open(logFileName.c_str(), std::ios::app);
    if (log != "Success") { // Don't print code Success in log
        out << currentDate() << " : " << log << "\r\n";
    }
    out.close();
}

string getAvError(int& errorCode) {
    if (errorCode < 0) {
        errorCode = -errorCode;
    }
    char errorMessage[512];
    int found = av_strerror(errorCode, errorMessage, 512);
    if (found == 0) {
        return (string(errorMessage));
    } else {
        return "Unknown error";
    }
}

long int remainingFreeSpace(string path) {
    boost::filesystem::space_info si = boost::filesystem::space(path);
    return si.available / 1024 / 1024; // Bytes -> Kilobytes -> Megabytes
}