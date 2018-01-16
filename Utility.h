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

void deamonize();
void createDirectoryVideos(string rootDirectory);
int timeSinceDate(string dateToCompare);
void removeContentOfDirectory(string path, bool exact);
int removeOldFile(int nbDays, string path);

#endif /* UTILITY_H */

