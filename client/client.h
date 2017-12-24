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
#include <thread>

#include "../socket/TcpChatSocket.h"
#include <json11.hpp>
#include "../common.h"

#define BUFSIZE 100

using namespace std;
using namespace json11;

class Client{
private:
    TcpChatSocket* serverSock;
    void tryRegister();
    void tryLogin();
    void tryChat();
    void tryExitChat();
    void tryListUsers();
    void tryListFriends();
    void tryAddFriend();
    void tryProfile();
    void sendMsg();
    char buf[BUFSIZE];
    map<string,vector<string>> msgBuffer;
    BinData inData;
    string chatPartner;

public:
    int startClient();
};

#endif