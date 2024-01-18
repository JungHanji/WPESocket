#pragma once
#include "Socket.hpp"

class Client{
    Socket tsock;
    public:

    void connectServer(){
        int erStat = connect(tsock.getSocket(), (sockaddr*)tsock.getServInfo(), sizeof(*tsock.getServInfo()));
	
        if (erStat != 0) {
            closesocket(tsock.getSocket());
            WSACleanup();
            throw runtime_error("Connection to Server is FAILED. Error # " + to_string(WSAGetLastError()));
        } else cout << "Connection established SUCCESSFULLY. Ready to send a message to Server\n";
    }

    string getAnsFromServ(string message){
        tsock.tsend(tsock.getSocket(), message);
        return tsock.trecv(tsock.getSocket());
    }

    void quit(){
        tsock.quitConn();
    }

    Client(string str_ip, int int_port): tsock(str_ip, int_port){}
    Client(){}
};