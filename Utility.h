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

#include <string>
using namespace std;

class Utility {
public:
    static void deamonize();
    static void createDirectoryVideos(string rootDirectory);
    static int timeSinceDate(string dateToCompare);
    static void removeContentOfDirectory(string path);
    static int removeOldFile(int nbDays, string path);
private:
};

#endif /* UTILITY_H */

