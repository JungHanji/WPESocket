#pragma once
#include <re/init.hpp>

#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
const int BUFFER_SIZE = 4096;

class Socket{

    SOCKET tsocket;
    string str_ip;
    int int_port;

    sockaddr_in servInfo;
    in_addr ip_to_num;

    bool debug = false;

    public:

    int          getPort    (){return int_port;}
    string       getBindedIp(){return str_ip;}
    SOCKET      &getSocket  (){return tsocket;}
    sockaddr_in *getServInfo(){return &servInfo;}
    in_addr     *getIpToNum (){return &ip_to_num;}

    Socket(string str_ip, int int_port, bool debug = false): str_ip(str_ip), int_port(int_port), debug(debug){
        WSADATA wsData;
		
        int erStat = WSAStartup(MAKEWORD(2,2), &wsData);
            
        if ( erStat != 0 ) {
            throw runtime_error("Error WinSock version initializaion #" + to_string(WSAGetLastError()));
        } else if(debug) cout << "WinSock initialization is OK" << endl;

        tsocket = socket(AF_INET, SOCK_STREAM, 0);

        if (tsocket == INVALID_SOCKET) {
            closesocket(tsocket);
            WSACleanup();
            throw runtime_error("Error initialization socket # " + to_string(WSAGetLastError()));
        } else if(debug) cout << "Server socket initialization is OK" << endl;

        
        erStat = inet_pton(AF_INET, str_ip.c_str(), &ip_to_num);
        if (erStat <= 0) {
            throw runtime_error("Error in IP translation to special numeric format");
        }

        ZeroMemory(&servInfo, sizeof(servInfo));	
                        
        servInfo.sin_family = AF_INET;
        servInfo.sin_addr = ip_to_num;	
        servInfo.sin_port = htons(int_port);
    }
    
    string trecv(SOCKET &cli_sock, int buffer_size = BUFFER_SIZE){
        char *buffer = new char[buffer_size];
        int bytes_recived = recv(cli_sock, buffer, buffer_size, 0);
        if(bytes_recived > 0){
            return string(buffer, 0, bytes_recived);
        }
        return {};
    }

    int tsend(SOCKET &to_sock, string data, int buffer_size = BUFFER_SIZE){
        int sendResult = send(to_sock, 
                              clamp(data, buffer_size).c_str(), 
                              clamp(data, buffer_size).size() + 1, 
                              0
                        );
        if(sendResult == -1){
            // cout<<"WARNING: Cannot send package to client: the client probably disconnected\n";
            return 1;
        }
        return 0;
    }

    static int clean(){
        return WSACleanup();
    }

    int quitConn(){
        int status = shutdown(tsocket, SD_BOTH);
        if (status == 0) { status = closesocket(tsocket); }
        return status;
    }

    Socket(){}
};