class TcpChatSocket{
private:
    int portNo;
    int maxConn;

public:
    void initSocket();
    void sendMsg();
    void receiveMsg();
};