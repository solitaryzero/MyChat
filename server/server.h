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
#include <thread>
#include <queue>
#include <map>

#include "../socket/TcpChatSocket.h"
#include "../common.h"
#include "serverDatabase.h"

using namespace std;

class Server{
private:
    TcpChatSocket* serverSock;
    queue<function<void()>> tasks;
    map<string,BinData> msgBuffer;
    ServerDatabase db; 
    void catchClientSocket(TcpChatSocket* clientSock);

public:
    int startServer();
    TcpChatSocket* genServerSocket();
    TcpChatSocket* waitForSocket();
};

#endif