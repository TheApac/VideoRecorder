/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Watchdog.h
 * Author: Alexandre
 *
 * Created on 12 février 2018, 13:50
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "Manager.h"

class Watchdog {
public:
    Watchdog();
    Watchdog(const Watchdog& orig);
    virtual ~Watchdog();
};

#endif /* WATCHDOG_H */

