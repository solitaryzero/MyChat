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
#include <mutex>

#include "../socket/TcpChatSocket.h"
#include <json11.hpp>
#include "../common.h"
#include "serverDatabase.h"

using namespace std;
using namespace json11;

class Server{
private:
    TcpChatSocket* serverSock;
    queue<function<void()>> tasks;
    map<int,thread> threadMap;
    map<string,TcpChatSocket*> clientSocketMap;
    map<string,vector<string>> msgBuffer;
    ServerDatabase db; 
    mutex taskLock;
    int nextSocketid;

    int sendMessageTo(string name, string content);
    TcpChatSocket* genServerSocket();
    TcpChatSocket* waitForSocket();
    void catchClientSocket(TcpChatSocket* clientSock);

public:
    int startServer();
};

#endif