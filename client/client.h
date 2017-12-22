#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string>
#include <vector>
#include "../socket/TcpChatSocket.h"

using namespace std;

class Client{
public:
    int startClient();
};

#endif