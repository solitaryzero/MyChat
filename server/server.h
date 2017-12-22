#ifndef SERVER_H
#define SERVER_H

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

class Server{
public:
    int startServer();
};

#endif