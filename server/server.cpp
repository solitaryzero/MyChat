#include "server.h"

#define BUFSIZE 100
#define MAX_USERS 10

int Server::sendMessageTo(string name, string content){
    if (!db.doesUserExist(name)){
        return USER_NOT_FOUND_ERROR;
    }

    auto iter = clientSocketMap.find(name);
    if ((iter == clientSocketMap.end()) || (clientSocketMap[name] == nullptr)){
        auto iter1 = msgBuffer.find(name);
        if (iter1 == msgBuffer.end()){
            vector<string> newVec;
            msgBuffer[name] = newVec;
        }
        msgBuffer[name].push_back(content);
    } else {
        clientSocketMap[name]->sendMsg(content);
    }
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

    TcpChatSocket* clientSock = new TcpChatSocket(clientSocketfd,nextSocketid);
    nextSocketid++;
    clientSock->initSocket();
    printf("accept client %s\n",inet_ntoa(clientSockAddr.sin_addr));  

    return clientSock;
}

void Server::catchClientSocket(TcpChatSocket* clientSock){
    string s = "hello world!";
    clientSock->sendMsg(s);//发送欢迎信息

    threadMap[clientSock->socketid] = thread([=](){
        //接收客户端的数据
        BinData inData;
        while (1){  
            inData = clientSock->recvMsg();
            if (inData.size() == 0) break;
            printf("%s\n",inData.data());

            taskLock.lock();            //加入处理信息事件
            tasks.push([=](){
                string err;
                Json msg = Json::parse(inData.data(),err);
                int msgType = msg["Type"].int_value();

                switch(msgType){
                    case MSG_TYPE_REGISTER:{
                        string name = msg["Name"].string_value();
                        string password = msg["Password"].string_value();
                        int res = db.createUser(name,password);
                        if (res == 1){
                            Json cont = Json::object{
                                {"Type", MSG_TYPE_ERRORMSG},
                                {"Content", USER_ALREADY_EXIST_ERROR_STRING}
                            };
                            clientSock->sendMsg(cont.dump());
                        } else {
                            Json cont = Json::object{
                                {"Type", MSG_TYPE_INFOMSG},
                                {"Content", REGISTER_SUCCESS_MSG}
                            };
                            clientSock->sendMsg(cont.dump());
                            db.save();
                        }
                        break;
                    } 

                    case MSG_TYPE_LOGIN:{
                        string name = msg["Name"].string_value();
                        string password = msg["Password"].string_value();
                        int res = db.checkUser(name,password);
                        if (res == 1){
                            Json cont = Json::object{
                                {"Type", MSG_TYPE_ERRORMSG},
                                {"Content", LOGIN_FAILED_MSG}
                            };
                            clientSock->sendMsg(cont.dump());
                        } else {
                            Json cont = Json::object{
                                {"Type", MSG_TYPE_LOGIN_SUCCESS_MSG},
                                {"Content", LOGIN_SUCCESS_MSG}
                            };
                            clientSock->sendMsg(cont.dump());
                            clientSock->name = name;
                            clientSocketMap[name] = clientSock;
                            db.setOnlineState(name,true);
                        }
                        break;
                    } 

                    case MSG_TYPE_STRINGMSG:{
                        if (clientSock->name == NO_NAME){
                            Json cont = Json::object{
                                {"Type",MSG_TYPE_ERRORMSG},
                                {"Content",NOT_LOGINED_ERROR_STRING}
                            };
                            clientSock->sendMsg(cont.dump());
                            break;
                        }

                        string name = msg["Name"].string_value();
                        string content = msg["Content"].string_value();
                        Json cont = Json::object{
                            {"Type",MSG_TYPE_STRINGMSG},
                            {"Author",clientSock->name},
                            {"Content",content}
                        };
                        int res = sendMessageTo(name,cont.dump());
                        if (res == USER_NOT_FOUND_ERROR){
                            cont = Json::object{
                                {"Type",MSG_TYPE_ERRORMSG},
                                {"Content",USER_NOT_FOUND_ERROR_STRING}
                            };
                            clientSock->sendMsg(cont.dump());
                        }
                        break;
                    }

                    case MSG_TYPE_LISTUSERS:{
                        vector<string> res = db.findAllUsers();
                        int len = res.size();
                        Json::array users;
                        for (int i=0;i<res.size();i++){
                            Json tmp = Json::object{
                                {"Name",res[i]},
                                {"isOnline",db.getOnlineState(res[i])}
                            };
                            users.push_back(tmp);
                        }
                        Json cont = Json::object{
                            {"Type",MSG_TYPE_LISTUSERS},
                            {"Size",len},
                            {"Content",users}
                        };
                        clientSock->sendMsg(cont.dump());
                        break;
                    }

                    case MSG_TYPE_GET_BUFFERED_STRINGMSG:{
                        vector<string> bufferedMsg = msgBuffer[clientSock->name];
                        msgBuffer[clientSock->name].clear();
                        Json cont = Json::object{
                            {"Type",MSG_TYPE_GET_BUFFERED_STRINGMSG},
                            {"Content",bufferedMsg}
                        };
                        clientSock->sendMsg(cont.dump());
                        break;
                    }

                    default:
                    {
                        perror("unknown command");
                        break;
                    }
                }
            });
            taskLock.unlock();

        } 

        taskLock.lock();        //广播下线的用户
        printf("socket No.%d closed\n", clientSock->socketid);
        tasks.push([=](){
            db.setOnlineState(clientSock->name,false);
            threadMap[clientSock->socketid].join();
            if (clientSock->name != NO_NAME){
                clientSocketMap.erase(clientSock->name);
            }
            threadMap.erase(clientSock->socketid);
            delete clientSock;
        });
        taskLock.unlock();
    });
    
}

//启动服务器
int Server::startServer(){
    //清空事件队列
    while (!tasks.empty()){     
        tasks.pop();
    }

    db.init();    //初始化数据库

    nextSocketid = 0;

    this->serverSock = genServerSocket();   //生成服务器的socket

    thread taskThread = thread([=](){       //事件处理队列线程
        while(true){
            if (!tasks.empty()){
                taskLock.lock();
                function<void()> func = tasks.front();
                tasks.pop();
                taskLock.unlock();
                func();
            }
        }
    });

    thread waitForSocketThread = thread([=](){
        while(true){
            TcpChatSocket* clientSock;
            clientSock = waitForSocket();
            if (clientSock != nullptr) {
                tasks.push([=](){
                    catchClientSocket(clientSock);
                });
            }
        }
    });
    
    waitForSocketThread.join();

    serverSock->shutDownSocket();
    delete(serverSock);

    return 0;
}