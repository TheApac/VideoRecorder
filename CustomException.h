/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   CustomException.h
 * Author: Alexandre
 *
 * Created on 24 janvier 2018, 11:56
 */

#ifndef CUSTOMEXCEPTION_H
#define CUSTOMEXCEPTION_H

#include <string>
#include <exception>

using namespace std;

class CustomException : public exception {
protected:
    string _msg;
public:
    CustomException(std::string const& msg);
    ~CustomException() throw ();
    const char *what() const throw ();
};

class UndefinedField : public CustomException {
public:
    UndefinedField(std::string const& msg); /* Exception to throw when a field is not declared on a camera in the configuration file */
};

class InvalidID : public CustomException {
public:
    InvalidID(std::string const& msg); /* Exception to throw when the parameter for the camera ID is not a positive integer */
};

class InvalidNbDays : public CustomException {
public:
    InvalidNbDays(std::string const& msg); /* Exception to throw when the parameter for the number of days is not a positive integer */
};

class DuplicateField : public CustomException {
public:
    DuplicateField(std::string const& msg); /* Exception to throw when a field is declared twice for the same camera in the configuration file */
};

class InvalidLine : public CustomException {
public:
    InvalidLine(std::string const& msg); /* Exception to throw when an invalid line is encountered in the configuration file */
};

class EmptyLine : public CustomException {
public:
    EmptyLine(std::string const& msg); /* Exception to throw when an empty line is encountered in the configuration file */
};

class FileNotFound : public CustomException {
public:
    FileNotFound(std::string const& msg); /* Exception to throw when one of the configuration files is not found */
};

class DuplicateID : public CustomException {
public:
    DuplicateID(std::string const& msg); /* Exception to throw when one of the configuration files is not found */
};

class InvalidNbMin : public CustomException {
public:
    InvalidNbMin(std::string const& msg); /* Exception to throw when one of the configuration parameter for minutes is not a positive integer */
};

class InvalidOnvifConf : public CustomException {
public:
    InvalidOnvifConf(std::string const& msg); /* Exception to throw when one the onvif configuration is erroneous */
};

class EmptyPassword : public CustomException {
public:
    EmptyPassword(std::string const& msg); /* Exception to throw when the encrypted password was erroneous */
};

class DirectoryNotExist : public CustomException {
public:
    DirectoryNotExist(std::string const& msg); /* Exception to throw when the directory in which the camera has to record doesn't exist */
};

#endif /* CUSTOMEXCEPTION_H */

