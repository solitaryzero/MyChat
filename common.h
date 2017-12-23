#ifndef COMMON_H
#define COMMON_H

#define SERVER_PORT 8001

#define DB_FILENAME "db.txt"

#define MSG_TYPE_LOGIN_USERNAME 1
#define MSG_TYPE_LOGIN_PASSWORD 2
#define MSG_TYPE_REGISTER_USERNAME 3
#define MSG_TYPE_REGISTER_PASSWORD 4
#define MSG_TYPE_CHAT_WITH_SOMEBODY 5
#define MSG_TYPE_STRINGMSG 6

struct MsgHeader{
    int msgType;
    int length;
};

#endif