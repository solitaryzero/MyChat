#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string>
#include "server.h"

#define BUFSIZE 100

int Server::startServer(){
    int serverSocketfd;
    int clientSocketfd;
    int byteLength;
    char buf[BUFSIZE];
    struct sockaddr_in serverSockAddr;
    struct sockaddr_in clientSockAddr;
    memset(&serverSockAddr,0,sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = INADDR_ANY;
    serverSockAddr.sin_port = htons(8003);

    if ((serverSocketfd = socket(PF_INET,SOCK_STREAM,0)) < 0){
        perror("socket create error");
        return 1;
    }

    if (bind(serverSocketfd,(struct sockaddr*)&serverSockAddr,sizeof(serverSockAddr)) < 0){
        perror("socket bind error");
        return 1;
    }

    if (listen(serverSocketfd,5) < 0){
        perror("listen error");
        return 1;
    }

    socklen_t sinSize = sizeof(struct sockaddr_in);
    if ((clientSocketfd = accept(serverSocketfd, (struct sockaddr*)&clientSockAddr, &sinSize)) < 0){
        perror("accept error");
        return 1;
    }

    printf("accept client %s",inet_ntoa(clientSockAddr.sin_addr));  
    byteLength = write(clientSocketfd,"Welcome to my server",21);//发送欢迎信息  
    printf("here after send\n");
    printf("length: %d\n",byteLength);
      
    /*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/  
    while ((byteLength = read(clientSocketfd, buf, BUFSIZE)) > 0){  
        buf[byteLength]='\0';  
        printf("%s\n",buf);
        fflush(stdout);  
        if(write(clientSocketfd,buf,byteLength)<0)  
        {  
            perror("write error");  
            return 1;  
        }  
    } 
    
    shutdown(serverSocketfd,2);
    shutdown(clientSocketfd,2);

    return 0;
}