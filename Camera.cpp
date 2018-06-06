/*
 * File:   Camera.cpp
 * Author: Alexandre
 *
 * Created on 3 janvier 2018, 10:21
 */

#include "Camera.h"
#include <string>
#include "Utility.h"
extern "C" {
#include <sodium.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <boost/thread.hpp>
#include "wsdd.nsmap"
#include "plugin/wsseapi.h"
#include "include/soapDeviceBindingProxy.h"
#include "include/soapMediaBindingProxy.h"
#include "include/soapRemoteDiscoveryBindingProxy.h"
#include "CustomException.h"

using namespace std;
using namespace boost;

Camera::Camera(string& path, int& nbdays, int& ID, string& name, string& log, string& password, string& url) {
    if (SecondsToRecord == -1) {
        SecondsToRecord = DEFAULT_TIME_RECORDS; // 900 (15 * 60) : Default time 15 minutes
    }
    if (path.at(path.length() - 1) == '/') { // add a "/" at the end of the path if there is none
        this->directory = path;
    } else {
        this->directory = path + "/";
    }
    this->nbdays = nbdays; // The camera has to delete the old files
    this->ID = ID;
    this->name = name;
    this->log = log;
    this->password = string(getDecodedPassword(password).c_str());
    this->url = url;
    //cout << password << endl;
    //cout << password.compare("2NTech-Lyon") << endl;
}

Camera::Camera(string& tempPath, int& ID, string& name, string& log, string& password, string& url) {
    if (SecondsToRecord == -1) {
        SecondsToRecord = DEFAULT_TIME_RECORDS; // 900 (15 * 60) : Default time 15 minutes
    }
    if (tempPath.at(tempPath.length() - 1) == '/') { // add a "/" at the end of the path if there is none
        this->directory = tempPath;
    } else {
        this->directory = tempPath + "/";
    }
    this->nbdays = -1; // The camera doesn't have to delete the files, as the process that moves them will
    this->ID = ID;
    this->name = name;
    this->log = log;
    this->password = string(getDecodedPassword(password).c_str());
    this->url = url;
}

void Camera::record() {
    this->RTSPurl = "";
    try {
        thread(getFullRTSPUrl, this);
        sleep(5);
        if (this->RTSPurl == "") {
            throw InvalidOnvifConf(this->name + " (" + to_string(this->ID) + ")" + " log : " + this->log + ", pass : " + this->password + ".");
        }
        if (nbdays != -1) {
            removeOldFile(nbdays, this->directory); // Remove the old files if it has to
            if (remainingFreeSpace(this->directory) < AVERAGE_FILE_SIZE * SecondsToRecord / 60) { //make sure the camera has enough space to record
                sendEmail("Not enough space left to record in : " + this->directory);
                int nbdaysTemp = nbdays - 1;
                while (remainingFreeSpace(this->directory) < AVERAGE_FILE_SIZE * SecondsToRecord / 60) { //remove files until it has enough space
                    removeOldFile(nbdaysTemp, this->directory);
                    nbdaysTemp = nbdaysTemp - 1;
                }
            }
        }
        AVFormatContext* context = avformat_alloc_context();
        AVPacket packet; // packet sent by the camera
        av_init_packet(&packet);
        av_register_all(); // Initialize libavformat and register all the muxers
        avcodec_register_all(); // Register all the codecs, parsers and bitstream filters
        avformat_network_init();
        bool error = false; // Don't start recording if an error was encountered
        int averr = avformat_open_input(&context, this->RTSPurl.c_str(), NULL, NULL);
        if (averr != 0) { // open rtsp worked
            if (timeSinceCrashCamera(this->ID) > 10 * 60) { // Don't send 2 mails in less than 10 min
                sendEmail("Couldn't connect to the camera " + this->name + " (ID : " + to_string(this->ID) + ") of url " + this->url); // mail if camera is unreachable
                addCrashedCamera(this->ID);
            }
            error = true;
        } else {
            addLog(getAvError(averr));
        }
        if (!error) {
            averr = avformat_find_stream_info(context, NULL);
            if (averr < 0) { // retrieve informations of the stream
                if (timeSinceCrashCamera(this->ID) > 10 * 60) { // Don't send 2 mails in less than 10 min
                    sendEmail("Couldn't retrieve informations for the camera " + this->name + " (ID : " + to_string(this->ID) + ") of url " + this->url);
                    addCrashedCamera(this->ID);
                }
                error = true;
            }
        }
        if (!error) {
            int video_stream_index; // keep the index of the video stream
            for (int i = 0; i < context->nb_streams; i++) { //search video stream
                if (context->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                    video_stream_index = i;
                }
            }
            AVFormatContext* oc;
            averr = avformat_alloc_output_context2(&oc, NULL, NULL, getFileName().c_str()); // try to create a context based on stream info and file name
            if (!oc) { // If no context was found
                avformat_alloc_output_context2(&oc, NULL, "mp4", getFileName().c_str()); // try to create a context based on stream info and file name on mp4 format
            }
            if (!oc) { // No format could be found
                error = true;
                addLog(getAvError(averr));
            }
            string destinationDirectory = createDirectoryVideos(this->directory); // Create the directory where the camera will record if it doesn't exist
            string fullName = destinationDirectory + getFileName(); // Store the absolute path to the file
            if (!error) {
                AVOutputFormat* fmt;
                fmt = oc->oformat;
                if (!(fmt->flags & AVFMT_NOFILE)) {
                    averr = avio_open(&oc->pb, fullName.c_str(), AVIO_FLAG_WRITE); //open output file
                    if (averr < 0) { // File couldn't be created
                        createDirectoryVideos(this->directory); // Try to recreate the directories
                        averr = avio_open(&oc->pb, fullName.c_str(), AVIO_FLAG_WRITE); //open output file
                        if (averr < 0) {
                            error = true;
                            addLog(getAvError(averr));
                        }
                    }
                }
                if (!error) {
                    AVStream* stream = NULL;
                    stream = avformat_new_stream(oc, context->streams[video_stream_index]->codec->codec); // save which stream to mux
                    avcodec_copy_context(stream->codec, context->streams[video_stream_index]->codec); // set infos to the camera's ones
                    stream->sample_aspect_ratio = context->streams[video_stream_index]->codec->sample_aspect_ratio; //set the file dimensions ratio
                    averr = avformat_write_header(oc, NULL); // write the header in the out file
                    if (averr < 0) { // Header couldn't be written
                        error = true;
                        addLog(getAvError(averr));
                    }
                }
                if (!error) {
                    bool recordNext = false; // start only one other recording
                    time_t t = time(0);
                    long int secondsToStop = time(&t) + SecondsToRecord; // save when to stop recording this video
                    int lostframe = 0; // Saves the number of frames lost
                    while (time(&t) < secondsToStop) { // Loop while the file is not at its max time and frames are available
                        if (!IsInRunningList(to_string(this->ID))) { // If the camera isn't in the list of running cameras
                            thread(addRunningCamera, to_string(this->ID)); // Add itself to it
                        }
                        if (av_read_frame(context, &packet) < 0) { // Read each frame and store it into packet
                            sleep(1);
                            lostframe++;
                            if (lostframe > 20 && timeSinceCrashCamera(this->ID) > 10 * 60) { // Stop if more than 60 images are lost && don't send 2 mails in less than 10 min
                                sendEmail("The camera " + this->name + " (ID : " + to_string(this->ID) + ") of url " + this->url + " stopped sending informations");
                                addCrashedCamera(this->ID);
                                break;
                            }
                        } else { // Packet has been read
                            lostframe = 0; // Reinit number of lost packet
                            if (packet.stream_index == video_stream_index) { //check if the packet is a video
                                av_write_frame(oc, &packet); // write the frame in the out file
                            }
                            if (!recordNext && time(&t) + TIME_BEFORE_NEW_RECORD == secondsToStop) { // If no other record has been started and it's time to start one
                                thread(&Camera::record, this); // Start a new record for the same camera in a new thread
                                recordNext = true; // Store the fact that an other record has been started
                            }
                            av_free_packet(&packet); // clear the packet
                        }
                    }
                    averr = av_write_trailer(oc); // write the trailer in the out file
                    if (averr != 0) {
                        addLog(getAvError(averr));
                    }
                    // ---------- CLEANUP ---------- //
                    averr = avio_closep(&oc->pb);
                    if (averr != 0) {
                        addLog(getAvError(averr));
                    }
                    avformat_free_context(oc);
                    av_free_packet(&packet);
                    avformat_close_input(&context); // Clost the RTSP connexion
                    // ----------------------------- //
                }
            } else {
                sendEmail("Couldn't start writing the file for the camera " + this->name + " (ID : " + to_string(this->ID) + ") of url " + this->url);
                addCrashedCamera(this->ID);
            }
        }
    } catch (InvalidOnvifConf e) {

        sendEmail(e.what());
    }
}

Camera::~Camera() {
    // //cout << "Destroy camera " << to_string(this->ID) << " : " << currentDate() << endl;
}

string Camera::GetDirectory() const {

    return this->directory;
}

int Camera::GetID() const {

    return this->ID;
}

int Camera::GetNbdays() const {

    return this->nbdays;
}

string Camera::GetPassword() const {

    return this->password;
}

string Camera::GetUrl() const {

    return this->url;
}

string Camera::GetRTSPurl() const {

    return this->RTSPurl;
}

string Camera::getFileName() {
    // //cout << "start getFileName (" << to_string(this->ID) << ") : " << currentDate() << endl;
    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time(); // Get the time offset in current day
    const boost::posix_time::time_duration td = now.time_of_day();
    const long hours = td.hours(); // Current hour
    const long minutes = td.minutes(); // Current minute
    const long seconds = td.seconds(); // Current seconds
    const long milliseconds = td.total_milliseconds() -((hours * 3600 + minutes * 60 + seconds) * 1000); // Convert number of seconds since midnight to current milliseconds
    tm date = to_tm(now); // Struct for the date
    string FileName = "C" + to_string(this->ID) + "-" + to_string(date.tm_year + 1900);
    if (date.tm_mon + 1 < 10) { // if month between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(date.tm_mon + 1);
    } else {
        FileName += to_string(date.tm_mon + 1);
    }
    if (date.tm_mday < 10) { // if day between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(date.tm_mday);
    } else {
        FileName += to_string(date.tm_mday);
    }
    FileName += "-"; // separate Date and Hour in the file name
    if (hours < 10) { // if hour between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(hours);
    } else {
        FileName += to_string(hours);
    }
    if (minutes < 10) { // if minute between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(minutes);
    } else {
        FileName += to_string(minutes);
    }
    if (seconds < 10) { // if seconds between 1 and 9, add a 0 in front of it
        FileName += "0" + to_string(seconds);
    } else {
        FileName += to_string(seconds);
    }
    if (milliseconds < 10) { // if milliseconds between 1 and 9, add two 0 in front of it
        FileName += "00" + to_string(milliseconds);
    } else if (milliseconds < 100) { // if milliseconds between 10 and 99, add a 0 in front of it
        FileName += "0" + to_string(milliseconds);
    } else {

        FileName += to_string(milliseconds);
    }
    // //cout << "end getFileName (" << to_string(this->ID) << ") : " << currentDate() << endl;
    return FileName + ".mp4"; //add the extension
}

volatile int Camera::SecondsToRecord = -1;

bool Camera::setSecondsToRecord(int sec) {
    if (sec < 0) {
        sendEmail("The number of seconds must be positive integer. Number given : " + to_string(sec));
    }
    if (SecondsToRecord == -1) {
        SecondsToRecord = sec;
        return true;
    } else { // If the number of seconds to record had already been changed, an error will be thrown

        return false;
    }
}

void Camera::reinitTimeRecord() {

    SecondsToRecord = -1;
}

volatile int Camera::GetSecondsToRecord() {

    return SecondsToRecord;
}

bool Camera::getFullRTSPUrl(Camera* cam) {
    //cout << "start getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
    // Proxy declarations
    DeviceBindingProxy proxyDevice;
    MediaBindingProxy proxyMedia;
    string hostname = "http://" + cam->url + "/onvif/device_service";
    proxyDevice.soap_endpoint = hostname.c_str();
    struct soap *soap = soap_new();
    //cout << cam->password.c_str();
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyDevice.soap, NULL, cam->log.c_str(), cam->password.c_str())) {
        //cout << "end 1 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
        //cout << cam->password.c_str() << endl;
        return false;
    }
    if (SOAP_OK != soap_wsse_add_Timestamp(proxyDevice.soap, "Time", 10)) {
        //cout << "end 2 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
        //cout << cam->password.c_str() << endl;
        return false;
    }
    soap_destroy(soap);
    soap_end(soap);
    // Get Device capabilities
    _tds__GetCapabilities *tds__GetCapabilities = soap_new__tds__GetCapabilities(soap, -1);
    tds__GetCapabilities->Category.push_back(tt__CapabilityCategory__All);
    _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse = soap_new__tds__GetCapabilitiesResponse(soap, -1);
    if (SOAP_OK == proxyDevice.GetCapabilities(tds__GetCapabilities, tds__GetCapabilitiesResponse) && tds__GetCapabilitiesResponse->Capabilities->Media != NULL) {
        proxyMedia.soap_endpoint = tds__GetCapabilitiesResponse->Capabilities->Media->XAddr.c_str();
    }
    // For MediaBindingProxy
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, cam->log.c_str(), cam->password.c_str())) {
        //cout << "end 3 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
        //cout << cam->password.c_str() << endl;
        return false;
    }
    if (SOAP_OK != soap_wsse_add_Timestamp(proxyMedia.soap, "Time", 10)) {
        //cout << "end 4 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
        //cout << cam->password.c_str() << endl;
        return false;
    }
    // Get Device Profiles
    _trt__GetProfiles *trt__GetProfiles = soap_new__trt__GetProfiles(soap, -1);
    _trt__GetProfilesResponse *trt__GetProfilesResponse = soap_new__trt__GetProfilesResponse(soap, -1);
    if (SOAP_OK == proxyMedia.GetProfiles(trt__GetProfiles, trt__GetProfilesResponse)) {
        _trt__GetStreamUri *trt__GetStreamUri = soap_new__trt__GetStreamUri(soap, -1);
        trt__GetStreamUri->StreamSetup = soap_new_tt__StreamSetup(soap, -1);
        trt__GetStreamUri->StreamSetup->Stream = tt__StreamType__RTP_Unicast;
        trt__GetStreamUri->StreamSetup->Transport = soap_new_tt__Transport(soap, -1);
        trt__GetStreamUri->StreamSetup->Transport->Protocol = tt__TransportProtocol__RTSP;
        _trt__GetStreamUriResponse *trt__GetStreamUriResponse = soap_new__trt__GetStreamUriResponse(soap, -1);
        for (int i = 0; i < trt__GetProfilesResponse->Profiles.size(); i++) { // Loop for every profile
            trt__GetStreamUri->ProfileToken = trt__GetProfilesResponse->Profiles[i]->token;
            if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyMedia.soap, NULL, cam->log.c_str(), cam->password.c_str())) {
                //cout << "end 5 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
                //cout << cam->password.c_str() << endl;
                return false;
            }
            if (SOAP_OK == proxyMedia.GetStreamUri(trt__GetStreamUri, trt__GetStreamUriResponse)) { // Get Snapshot URI for profile
                if (trt__GetStreamUriResponse->MediaUri->Uri.find("h264") != string::npos) {
                    cam->RTSPurl = trt__GetStreamUriResponse->MediaUri->Uri;
                } else if (i == 1 && cam->RTSPurl == "") {
                    cam->RTSPurl = trt__GetStreamUriResponse->MediaUri->Uri;
                }
            }
        }
        if (cam->RTSPurl.size() > 0) {
            cam->RTSPurl = cam->RTSPurl.substr(0, 7) + cam->log + ":" + cam->password + "@" + cam->RTSPurl.substr(7);
        }
    }
    soap_destroy(soap);
    soap_end(soap);
    //cout << "end 6 getFullRTSPUrl (" << to_string(cam->ID) << ") : " << currentDate() << endl;
    //cout << cam->RTSPurl << endl;
    //cout << cam->password.c_str() << endl;
    return true;
}