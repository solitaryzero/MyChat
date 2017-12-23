#include "client.h"

void Client::tryRegister(){
    string name,password;
    printf("Trying to register...\n");

    printf("Enter your name: ");
    scanf("%s",buf);  
    buf[strlen(buf)] = '\0';
    name.assign(buf);

    printf("Enter your password: ");
    scanf("%s",buf);  
    buf[strlen(buf)] = '\0';
    password.assign(buf);

    Json res = Json::object{
        {"Type",MSG_TYPE_REGISTER},
        {"Name",name},
        {"Password",password}
    };

    serverSock->sendMsg(res.dump());

    inData = serverSock->recvMsg();
    printf("length:%d\n",inData.size());
    printf("received:%s\n",inData.data());  
    fflush(stdout);
}

void Client::tryLogin(){
    string name,password;
    printf("Trying to login...\n");

    printf("Enter your name: ");
    scanf("%s",buf);  
    buf[strlen(buf)] = '\0';
    name.assign(buf);

    printf("Enter your password: ");
    scanf("%s",buf);  
    buf[strlen(buf)] = '\0';
    password.assign(buf);

    Json res = Json::object{
        {"Type",MSG_TYPE_LOGIN},
        {"Name",name},
        {"Password",password}
    };

    serverSock->sendMsg(res.dump());

    inData = serverSock->recvMsg();
    printf("length:%d\n",inData.size());
    printf("received:%s\n",inData.data());  
    fflush(stdout);
}

void Client::sendMsg(){
    buf[strlen(buf)] = '\0';
    string msg(buf);

    Json res = Json::object{
        {"Type",MSG_TYPE_STRINGMSG},
        {"Message",msg}
    };

    serverSock->sendMsg(res.dump());
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
        } else if (strcmp(buf,"login") == 0){
            tryLogin();
        } else {
            sendMsg();
        }
    }  
    
    serverSock->shutDownSocket();
    return 0;
}