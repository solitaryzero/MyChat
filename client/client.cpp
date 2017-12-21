#include <iostream>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string>
#include "client.h"

#define BUFSIZE 100

int Client::startClient(){
    int socketfd;
    int byteLength;
    char buf[BUFSIZE];
    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr,0,sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverSockAddr.sin_port = htons(8003);

    if ((socketfd = socket(PF_INET,SOCK_STREAM,0)) < 0){
        perror("socket create error");
        return 1;
    }   
      
    /*将套接字绑定到服务器的网络地址上*/  
    if (connect(socketfd, (struct sockaddr *)&serverSockAddr, sizeof(struct sockaddr)) < 0){  
        perror("connect error");  
        return 1;  
    }  
    printf("connected to server");  
    byteLength = read(socketfd,buf,BUFSIZE);//接收服务器端信息  
    buf[byteLength]='\0';  
    printf("length: %d\n",byteLength);
    printf("%s\n",buf); //打印服务器端信息  
    fflush(stdout);
      
    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/  
    while(1)  
    {  
        printf("Enter string to send:");  
        scanf("%s",buf);  
        if (!strcmp(buf,"quit")){
            break;  
        }
        byteLength = write(socketfd,buf,strlen(buf));  
        byteLength = read(socketfd,buf,BUFSIZE);  
        buf[byteLength] = '\0';  
        printf("received:%s\n",buf);  
    }  
    
    shutdown(socketfd,2);
    return 0;
}