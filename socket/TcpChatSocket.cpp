#include "TcpChatSocket.h"

#define BUFSIZE 65535

using namespace std;

TcpChatSocket::TcpChatSocket(int sfd){
    socketfd = sfd;
}

int TcpChatSocket::initSocket(){
    bool flag = true;
    setsockopt(socketfd,SOL_SOCKET ,SO_REUSEADDR,(const char*)&flag,sizeof(bool)); 
    return 0;
}

int TcpChatSocket::sendMsg(string s){
    BinData dataOut;
    dataOut.resize(s.size()+1);
    char* pDst = &dataOut[0];
    memcpy(pDst,s.data(),s.size());
    dataOut.at(s.size()) = '\0';
    if (write(socketfd,dataOut.data(),dataOut.size()) < 0){
        perror("send error");
        return 1;
    }
    return 0;
}

int TcpChatSocket::sendMsg(void* p, int len){
    BinData dataOut;
    dataOut.resize(len);
    char* pDst = &dataOut[0];
    memcpy(pDst,p,len);
    if (write(socketfd,dataOut.data(),dataOut.size()) < 0){
        perror("send error");
        return 1;
    }
    return 0;
}

int TcpChatSocket::sendMsg(BinData dataOut){
    if (write(socketfd,dataOut.data(),dataOut.size()) < 0){
        perror("send error");
        return 1;
    }
    return 0;
}

BinData TcpChatSocket::recvMsg(){
    int byteLength;
    char buf[BUFSIZE];
    byteLength = read(socketfd,buf,BUFSIZE);
    BinData res;
    if (byteLength < 0){
        perror("receive error");
        res.resize(0);
        return res;
    }

    res.resize(byteLength);
    memcpy(res.data(),buf,byteLength);
    return res;
}

int TcpChatSocket::shutDownSocket(){
    shutdown(socketfd,2);
    return 0;
}