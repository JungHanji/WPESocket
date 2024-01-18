#pragma once
#include "Socket.hpp"

struct cliData{
    sockaddr_in clientInfo;
    int clientInfo_size;
    SOCKET cliSock;
    thread *selfThread;
    bool enabled = true;
};

class Server{
    Socket tsock;
    thread *acceptThread;

    public:
    vector<cliData> clients;
    int clisNum = 0;

    cliData &getClient(int inx = 0){
        return clients[inx];
    }

    Socket &getSocket(){
        return tsock;
    }

    vector<cliData> *getClients(){
        return &clients;
    }

    void newClientsThread(function<void(Server *serv, int inx, vector<cliData> *clis)> responce_func, int id){
        clients[id].selfThread = new thread(responce_func, this, id, getClients());
        clients[id].selfThread->detach();
    }

    void listenClients(){
        int erStat = listen(tsock.getSocket(), SOMAXCONN);

        if ( erStat != 0 ) {
            closesocket(tsock.getSocket());
            WSACleanup();
            throw runtime_error("Can't start to listen to. Error # " + to_string(WSAGetLastError()));
        } else { cout << "Listening..." << endl; }
    }

    void acceptConnection(){
        cliData tmp;
        ZeroMemory(&tmp.clientInfo, sizeof(tmp.clientInfo));
        tmp.clientInfo_size = sizeof(tmp.clientInfo);
        tmp.cliSock = accept(tsock.getSocket(), (sockaddr*)&tmp.clientInfo, &tmp.clientInfo_size);

        if (tmp.cliSock == INVALID_SOCKET) {
                closesocket(tsock.getSocket());
                closesocket(tmp.cliSock);
                WSACleanup();
                throw runtime_error("Client detected, but can't connect to a client. Error # " + to_string(WSAGetLastError()));
        } else cout << "Connection to a client established successfully" << endl;
        clients.push_back(tmp);
        clisNum+=1;
    }

    void startDetachedAcceptions(function<void(Server *serv)> acceptingFunc){
        acceptThread = new thread(acceptingFunc, this);
        acceptThread->detach();
    }

    void initAllClis(function<void(Server*, int, vector<cliData>*)> callback_function){
        for(int i = 0; i < clisNum; i++){
            clients[clisNum].selfThread = new thread(callback_function, this, clisNum, &clients);
            clients[clisNum].selfThread->detach();
        }
    }

    int updateCli(SOCKET &cliSock, function<string(string&, Server*)> callback_function){
        string recevied = tsock.trecv(cliSock);
        return tsock.tsend(cliSock, callback_function(recevied, this));
    }

    void updateCli(int cliInx, function<string(string&, Server*)> callback_function){
        if(cliInx < clients.size()){
            string recevied = tsock.trecv(clients[cliInx].cliSock);
            if(tsock.tsend(clients[cliInx].cliSock, callback_function(recevied, this))!=0){
                clients.erase(clients.begin()+cliInx);
            }
        }
    }

    void updateAllClis(function<string(string&, Server*, int, vector<cliData>&)> callback_function){
        int inx = 0, delinx = 0;
        int *toDelete = new int[clients.size()];
        
        for(cliData &cli:clients){
            string recevied = tsock.trecv(cli.cliSock);
            if(tsock.tsend(cli.cliSock, callback_function(recevied, this, inx, clients))!=0){
                toDelete[delinx] = inx;
                delinx++;
            }
            inx++;
        }
        
        for(int i = 0; i < delinx; i++){
            cout<<"deleting cli #"<<toDelete[i]<<endl;
            if(clients[toDelete[i]].selfThread->joinable()) clients[toDelete[i]].selfThread->detach();
            delete clients[toDelete[i]].selfThread;
            
            clients.erase(clients.begin()+toDelete[i]);
        }

        delete []toDelete;
    }

    void clearDisabledClients(){
        int n = 0;
        int *toDelete = new int[clisNum];
        for(int i = 0; i < clisNum; i++){
            if(!getClient(i).enabled){
                toDelete[n] = i;
                n++;
            }
        }
        clisNum -= n;

        for(int i = 0; i < n; i++){
            if(clients[toDelete[i]].selfThread->joinable()) clients[toDelete[i]].selfThread->detach();
            delete clients[toDelete[i]].selfThread;
            
            clients.erase(clients.begin()+toDelete[i]);
        }

        delete []toDelete;
    }

    void quit(){
        tsock.quitConn();
    }

    Server(string str_ip, int int_port): tsock(str_ip, int_port){
        int erStat;

        erStat = bind(tsock.getSocket(), (sockaddr*)tsock.getServInfo(), sizeof(*tsock.getServInfo()));
        if ( erStat != 0 ) {
            closesocket(tsock.getSocket());
            WSACleanup();
            throw runtime_error("Error Socket binding to server info. Error # " + to_string(WSAGetLastError()));
        } else cout << "Binding socket to Server info is OK" << endl;
    }

    Server(){}
    ~Server(){
        for(cliData &cli:clients){
            delete cli.selfThread;
        }
    }
};