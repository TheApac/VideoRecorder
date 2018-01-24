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
    UndefinedField(std::string const& msg);
};

class InvalidID : public CustomException {
public:
    InvalidID(std::string const& msg);
};

class InvalidPath : public CustomException {
public:
    InvalidPath(std::string const& msg);
};

class InvalidNbDays : public CustomException {
public:
    InvalidNbDays(std::string const& msg);
};

class InvalidURL : public CustomException {
public:
    InvalidURL(std::string const& msg);
};

class DuplicateField : public CustomException {
public:
    DuplicateField(std::string const& msg);
};

class InvalidLine : public CustomException {
public:
    InvalidLine(std::string const& msg);
};

class EmptyLine : public CustomException {
public:
    EmptyLine(std::string const& msg);
};


#endif /* CUSTOMEXCEPTION_H */

