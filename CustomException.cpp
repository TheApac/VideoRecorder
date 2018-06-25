/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   CustomException.cpp
 * Author: Alexandre
 *
 * Created on 24 janvier 2018, 11:56
 */

#include "CustomException.h"

CustomException::CustomException(std::string const& msg) : _msg(msg) {
    _msg = msg;
}

CustomException::~CustomException() throw () {
}

const char *CustomException::what() const throw () {
    return _msg.data();
}

UndefinedField::UndefinedField(std::string const &msg) : CustomException("Undefined field : " + msg) {
}

InvalidID::InvalidID(std::string const &msg) : CustomException("Invalid id : " + msg + " should be a positive integer") {
}

InvalidNbDays::InvalidNbDays(std::string const &msg) : CustomException("Invalid number of days : " + msg + " should be a positive integer") {
}

DuplicateField::DuplicateField(std::string const &msg) : CustomException("Field duplicated : " + msg) {
}

InvalidLine::InvalidLine(std::string const &msg) : CustomException("Invalid line : " + msg) {
}

EmptyLine::EmptyLine(std::string const &msg) : CustomException("Empty line in config file : " + msg) {
}

FileNotFound::FileNotFound(std::string const &msg) : CustomException("File not found : " + msg) {
}

DuplicateID::DuplicateID(std::string const &msg) : CustomException("The ID : " + msg + " is used twice") {
}

InvalidNbMin::InvalidNbMin(std::string const &msg) : CustomException("Invalid number of minutes : " + msg + " should be a positive integer") {
}

InvalidOnvifConf::InvalidOnvifConf(std::string const &msg) : CustomException("Invalid Onvif configuration for camera : " + msg) {
}

EmptyPassword::EmptyPassword(std::string const &msg) : CustomException("Empty password for camera : " + msg + ", the encrypted password given couldn't be decrypted") {
}