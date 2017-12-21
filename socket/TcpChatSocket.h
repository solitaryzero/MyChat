#ifndef TCPCHATSOCKET_H
#define TCPCHATSOCKET_H

class TcpChatSocket{
private:
    int portNo;
    int maxConn;

public:
    int initSocket();
    void sendMsg();
    void receiveMsg();
};

#endif