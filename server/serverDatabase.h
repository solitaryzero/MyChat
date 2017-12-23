#ifndef SERVERDATABASE_H
#define SERVERDATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "../common.h"

using namespace std;

class ServerDatabase{
public:
    ServerDatabase();
    ~ServerDatabase();

    map<string, string> userData;

    void init();
    void save();
    int createUser(string name, string password);
};

#endif