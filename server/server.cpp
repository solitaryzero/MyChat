#include "server.h"

#define BUFSIZE 100
#define MAX_USERS 10

void Server::catchClientSocket(TcpChatSocket* clientSock){
    string s = "hello world!";
    clientSock->sendMsg(s);//发送欢迎信息

    thread sockLoopThread = thread([=](){
        //接收客户端的数据并将其发送给客户端
        BinData inData;
        while (1){  
            inData = clientSock->recvMsg();
            if (inData.size() == 0) break;
            printf("==============\n");
            printf("from socket %d:\n", clientSock->socketfd);
            printf("%s\n",inData.data());
            printf("%d\n",inData.size());
            fflush(stdout);  
            if(clientSock->sendMsg(inData.data(),inData.size()) > 0)  
            {  
                perror("write error");  
                return 1;  
            }  
        } 
    });
    sockLoopThread.detach();
}

//启动服务器
int Server::startServer(){
    while (!tasks.empty()){     //清空事件队列
        tasks.pop();
    }

    this->serverSock = genServerSocket();   //生成服务器的socket

    /*
    thread taskThread = thread([=](){       //事件处理队列线程
        while(true){
            if (!tasks.empty()){
                function<void()> func = tasks.front();
                tasks.pop();
                func();
            }
        }
    });
    */

    thread waitForSocketThread = thread([=](){
        while(true){
            TcpChatSocket* clientSock;
            clientSock = waitForSocket();
            if (clientSock != nullptr) {
                catchClientSocket(clientSock);
            }
        }
    });
    
    waitForSocketThread.join();

    serverSock->shutDownSocket();
    delete(serverSock);

    return 0;
}

//生成服务器对应的socket
TcpChatSocket* Server::genServerSocket(){
    int serverSocketfd;
    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr,0,sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = htons(SERVER_PORT);

    if ((serverSocketfd = socket(PF_INET,SOCK_STREAM,0)) < 0){
        perror("socket create error");
        return nullptr;
    }

    TcpChatSocket* serverSock = new TcpChatSocket(serverSocketfd);
    serverSock->initSocket();

    if (bind(serverSocketfd,(struct sockaddr*)&serverSockAddr,sizeof(serverSockAddr)) < 0){
        perror("socket bind error");
        return nullptr;
    }

    if (listen(serverSocketfd,MAX_USERS) < 0){
        perror("listen error");
        return nullptr;
    }

    return serverSock;
}

//等待下一个用户socket
TcpChatSocket* Server::waitForSocket(){
    int clientSocketfd;
    struct sockaddr_in clientSockAddr;
    socklen_t sinSize = sizeof(struct sockaddr_in);
    if ((clientSocketfd = accept(serverSock->socketfd, (struct sockaddr*)&clientSockAddr, &sinSize)) < 0){
        perror("accept error");
        return nullptr;
    }

    TcpChatSocket* clientSock = new TcpChatSocket(clientSocketfd);
    clientSock->initSocket();
    printf("accept client %s\n",inet_ntoa(clientSockAddr.sin_addr));  

    return clientSock;
}