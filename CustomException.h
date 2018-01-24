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
    string msg;
public:
    DefaultException(std::string const& msg);
    ~DefaultException() throw ();
    const char *what() const throw ();
};



#endif /* CUSTOMEXCEPTION_H */

