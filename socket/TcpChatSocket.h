#ifndef TCPCHATSOCKET_H
#define TCPCHATSOCKET_H

#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

typedef vector<char> BinData;

class TcpChatSocket{
public:
    TcpChatSocket(int sfd);

    int socketfd;

    int initSocket();
    int sendMsg(string s);
    int sendMsg(void* p, int len);
    int sendMsg(BinData dataOut);
    BinData recvMsg();
    int shutDownSocket();
};

#endif