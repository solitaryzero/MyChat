#include "serverDatabase.h"

ServerDatabase::ServerDatabase(){

}

ServerDatabase::~ServerDatabase(){
    
}

void ServerDatabase::init(){
    ifstream in(DB_FILENAME);
    if (!in) return;

    int len;
    string name, password;
    in >> len;
    for (int i=0;i<len;i++){
        in >> name;
        in >> password;
        userData[name] = password;
    }
    in.close();
}

void ServerDatabase::save(){
    ofstream out(DB_FILENAME);
    int len = userData.size();
    printf("%d\n",len);
    out << len << endl;
    for (auto i=userData.begin();i!=userData.end();++i){
        out << i->first << endl;
        out << i->second << endl;
    }
    out.close();
}

int ServerDatabase::createUser(string name, string password){
    auto iter = userData.find(name);
    if (iter != userData.end()){
        return 1;
    }

    userData[name] = password;
    return 0;
}