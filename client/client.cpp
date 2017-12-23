#include "client.h"

void Client::tryRegister(){
    char buf[BUFSIZE];
    MsgHeader header;

    printf("Enter your name: \n");
    scanf("%s",buf);  
    header.msgType = MSG_TYPE_REGISTER_USERNAME;
    header.length = strlen(buf)+1;
    buf[strlen(buf)] = '\0';
    serverSock->sendMsg(&header, sizeof(MsgHeader));
    serverSock->sendMsg(buf,strlen(buf)+1);

    printf("Enter your password: \n");
    scanf("%s",buf);  
    header.msgType = MSG_TYPE_REGISTER_PASSWORD;
    header.length = strlen(buf)+1;
    buf[strlen(buf)] = '\0';
    serverSock->sendMsg(&header, sizeof(MsgHeader));
    serverSock->sendMsg(buf,strlen(buf)+1);

    inData = serverSock->recvMsg();
    printf("length:%d\n",inData.size());
    printf("received:%s\n",inData.data());  
    fflush(stdout);
}

void Client::sendMsg(){
    MsgHeader header;

    header.msgType = MSG_TYPE_STRINGMSG;
    header.length = strlen(buf)+1;
    buf[strlen(buf)] = '\0';
    serverSock->sendMsg(buf,strlen(buf)+1);
    inData = serverSock->recvMsg();
    printf("length:%d\n",inData.size());
    printf("received:%s\n",inData.data());  
    fflush(stdout);
}

int Client::startClient(){
    int socketfd;
    int byteLength;
    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr,0,sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverSockAddr.sin_port = htons(SERVER_PORT);

    if ((socketfd = socket(PF_INET,SOCK_STREAM,0)) < 0){
        perror("socket create error");
        return 1;
    }   
      
    /*将套接字绑定到服务器的网络地址上*/  
    if (connect(socketfd, (struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr)) < 0){  
        perror("connect error");  
        return 1;  
    }  
    printf("connected to server\n");  
    serverSock = new TcpChatSocket(socketfd);
    serverSock->initSocket();

    inData = serverSock->recvMsg();//接收服务器端信息  
    printf("%s\n",inData.data()); //打印服务器端信息  
    fflush(stdout);
      
    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/  
    while(1)  
    {  
        printf("Enter string to send:\n");  
        scanf("%s",buf);  
        if (strcmp(buf,"quit") == 0){
            break;  
        } else if (strcmp(buf,"register") == 0){
            tryRegister();
        } else {
            sendMsg();
        }
    }  
    
    serverSock->shutDownSocket();
    return 0;
}