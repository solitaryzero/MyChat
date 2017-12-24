#include "client.h"

void Client::tryRegister(){
    string name,password;
    printf("Trying to register...\n");

    printf("Enter your name: ");
    cin.getline(buf,BUFSIZE);
    buf[strlen(buf)] = '\0';
    name.assign(buf);

    printf("Enter your password: ");
    cin.getline(buf,BUFSIZE); 
    buf[strlen(buf)] = '\0';
    password.assign(buf);

    Json res = Json::object{
        {"Type",MSG_TYPE_REGISTER},
        {"Name",name},
        {"Password",password}
    };

    serverSock->sendMsg(res.dump());
}

void Client::tryLogin(){
    string name,password;
    printf("Trying to login...\n");

    printf("Enter your name: ");
    cin.getline(buf,BUFSIZE); 
    buf[strlen(buf)] = '\0';
    name.assign(buf);

    printf("Enter your password: ");
    cin.getline(buf,BUFSIZE); 
    buf[strlen(buf)] = '\0';
    password.assign(buf);

    Json res = Json::object{
        {"Type",MSG_TYPE_LOGIN},
        {"Name",name},
        {"Password",password}
    };

    serverSock->sendMsg(res.dump());
}

void Client::tryChat(){
    string name;

    cout << "You want to chat with: ";
    cin.getline(buf,BUFSIZE);
    buf[strlen(buf)] = '\0';
    name.assign(buf);

    chatPartner = name;
    cout << "You are now chatting with: " << name << endl;

    auto iter = msgBuffer.find(name);
    if (iter == msgBuffer.end()){
        return;
    }

    if (msgBuffer[name].size() == 0) return;

    cout << OFFLINE_MSG_HINT << name << ":" << endl;
    for (int i=0;i<msgBuffer[name].size();i++){
        cout << name << ": " << msgBuffer[name][i] << endl;
    }
    msgBuffer[name].clear();
}

void Client::tryExitChat(){
    if (chatPartner == NO_NAME){
        cout << "You are not chatting with others." << endl;
        return;
    }
    cout << "exited chat with " << chatPartner << "." << endl;
    chatPartner = NO_NAME;
}

void Client::tryListUsers(){
    Json res = Json::object{
        {"Type",MSG_TYPE_LISTUSERS},
    };

    serverSock->sendMsg(res.dump());
}

void Client::tryListFriends(){
    Json res = Json::object{
        {"Type",MSG_TYPE_LISTFRIENDS},
    };

    serverSock->sendMsg(res.dump());
}

void Client::sendMsg(){
    buf[strlen(buf)] = '\0';
    string msg(buf);

    if (chatPartner == NO_NAME){
        cout << "unknown command" << endl;
        return;
    }

    Json res = Json::object{
        {"Type",MSG_TYPE_STRINGMSG},
        {"Name",chatPartner},
        {"Content",msg}
    };

    serverSock->sendMsg(res.dump());
}

int Client::startClient(){
    int socketfd;
    int byteLength;
    struct sockaddr_in serverSockAddr;
    memset(&serverSockAddr,0,sizeof(serverSockAddr));
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverSockAddr.sin_port = htons(SERVER_PORT);

    chatPartner = NO_NAME;

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

    thread recvThread = thread([=](){
        while(true){
            inData = serverSock->recvMsg();//接收服务器端信息  
            if (inData.size() == 0) break;

            string err;
            Json msg = Json::parse(inData.data(),err);
            int msgType = msg["Type"].int_value();

            switch(msgType){
                case MSG_TYPE_ERRORMSG:{
                    cout << msg["Content"].string_value() << endl;
                    break;
                } 

                case MSG_TYPE_INFOMSG:{
                    cout << msg["Content"].string_value() << endl;
                    break;
                } 

                case MSG_TYPE_LOGIN_SUCCESS_MSG:{
                    cout << msg["Content"].string_value() << endl;
                    Json res = Json::object{
                        {"Type",MSG_TYPE_GET_BUFFERED_STRINGMSG}
                    };
                    serverSock->sendMsg(res.dump());    //获取离线信息
                    break;
                }

                case MSG_TYPE_STRINGMSG:{
                    string author = msg["Author"].string_value();
                    string content = msg["Content"].string_value();

                    if (author == chatPartner){
                        cout << author << ": " << content << endl;
                    } else {
                        auto iter = msgBuffer.find(author);
                        if (iter == msgBuffer.end()){
                            vector<string> newVec;
                            msgBuffer[author] = newVec;
                        }
                        msgBuffer[author].push_back(content);
                    }
                    break;
                }

                case MSG_TYPE_LISTUSERS:{
                    cout << "All users: " << endl;

                    for (int i=0;i<msg["Size"].int_value();i++){
                        cout << msg["Content"][i]["Name"].string_value() << " ";
                        if (msg["Content"][i]["isOnline"].bool_value()){
                            cout << "Online" << endl;
                        } else {
                            cout << "Offline" << endl;
                        }
                    }
                    break;
                }

                case MSG_TYPE_LISTFRIENDS:{
                    cout << "All friends: " << endl;

                    for (int i=0;i<msg["Size"].int_value();i++){
                        cout << msg["Content"][i]["Name"].string_value() << " ";
                        if (msg["Content"][i]["isOnline"].bool_value()){
                            cout << "Online" << endl;
                        } else {
                            cout << "Offline" << endl;
                        }
                    }
                    break;
                }

                case MSG_TYPE_GET_BUFFERED_STRINGMSG:{
                    Json::array data = msg["Content"].array_items();
                    for (int i=0;i<data.size();i++){
                        string err;
                        string s = data[i].string_value();
                        Json tmp = Json::parse(s,err);

                        string author = tmp["Author"].string_value();
                        string content = tmp["Content"].string_value();

                        if (author == chatPartner){
                            cout << author << ": " << content << endl;
                        } else {
                            auto iter = msgBuffer.find(author);
                            if (iter == msgBuffer.end()){
                                vector<string> newVec;
                                msgBuffer[author] = newVec;
                            }
                            msgBuffer[author].push_back(content);
                        }
                    }
                }

                default:
                {
                    break;
                }
            }

            fflush(stdout);
        }
    });
      
    /*循环的发送接收信息并打印接收信息--recv返回接收到的字节数，send返回发送的字节数*/  
    thread sendThread = thread([=](){
        while(true)  
        {  
            cout << ">> ";
            cin.getline(buf,BUFSIZE);
            if (strcmp(buf,"quit") == 0){
                break;  
            } else if (strcmp(buf,"register") == 0){
                tryRegister();
            } else if (strcmp(buf,"login") == 0){
                tryLogin();
            } else if (strcmp(buf,"chat") == 0){
                tryChat();
            } else if (strcmp(buf,"search") == 0){
                tryListUsers();
            } else if (strcmp(buf,"exit") == 0){
                tryExitChat();
            } else {
                sendMsg();
            }
        }  
    });

    recvThread.join();
    sendThread.join();

    serverSock->shutDownSocket();
    return 0;
}