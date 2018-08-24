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

class Watchdog {
public:
    Watchdog();
    virtual ~Watchdog();
private:
    static void signalHandler(int signum);
    static bool stopping;
};

#endif /* WATCHDOG_H */

