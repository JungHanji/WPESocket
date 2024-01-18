#pragma once
#include "Socket.hpp"

namespace pnet{
    namespace internal{

        class Client{
            sockaddr_in info;
            SOCKET isocket;
            int infoSize;
            unique_ptr<thread> m_thread;
            public:

            string ip;
            int    port;
            bool   enabled = true;
            bool   nclient = false;

            sockaddr_in &getInfo(){return info; }
            SOCKET &getISocket(){return isocket;}
            int &getInfoSize(){return infoSize; }
            unique_ptr<thread> &getThread(){return m_thread;}

            Client(sockaddr_in info, SOCKET isocket, int infoSize): info(info), isocket(isocket), infoSize(infoSize){
                char ip_addr[512];
                inet_ntop(AF_INET, &(info.sin_addr), ip_addr, INET_ADDRSTRLEN);
                ip   = string(ip_addr, 0, 512);
                port = ntohs(info.sin_port);
            }
            
            Client(const Client& other): 
                info(other.info), isocket(other.isocket), infoSize(other.infoSize), 
                ip(other.ip), port(other.port), enabled(other.enabled), nclient(other.nclient) 
            {
                if (other.m_thread) {
                    m_thread.reset(other.m_thread ? new thread(std::move(*other.m_thread)) : nullptr);
                }
            }
            Client& operator=(const Client& other){
                this->info = other.info;
                this->isocket = other.isocket;
                this->infoSize = other.infoSize; 
                this->ip = other.ip;
                this->port = other.port; 
                this->enabled = other.enabled;
                this->nclient = other.nclient;
                
                if (other.m_thread) {
                    m_thread.reset(other.m_thread ? new thread(std::move(*other.m_thread)) : nullptr);
                }

                return *this;
            }

            Client(bool nclient):nclient(nclient){}
            Client(){}
            ~Client(){}
        };

        // Only server
        class SAccept{
            Socket serverSocket; 
            public:

            pnet::internal::Client accpetCon(){
                sockaddr_in clientInfo;
                int clientInfo_size;
                SOCKET cliSock;

                ZeroMemory(&clientInfo, sizeof(clientInfo));
                clientInfo_size = sizeof(clientInfo);
                cliSock = accept(serverSocket.getSocket(), (sockaddr*)&clientInfo, &clientInfo_size);

                if (cliSock == INVALID_SOCKET) {
                        closesocket(serverSocket.getSocket());
                        closesocket(cliSock);
                        WSACleanup();
                        throw runtime_error("Client detected, but can't connect to a client. Error # " + to_string(WSAGetLastError()));
                } else cout << "Connection to a client established successfully" << endl;

                return {
                    clientInfo,
                    cliSock,
                    clientInfo_size
                };
            }

            void accpetCon(Client &cli){
                sockaddr_in clientInfo;
                int clientInfo_size;
                SOCKET cliSock;

                ZeroMemory(&clientInfo, sizeof(clientInfo));
                clientInfo_size = sizeof(clientInfo);
                cliSock = accept(serverSocket.getSocket(), (sockaddr*)&clientInfo, &clientInfo_size);

                if (cliSock == INVALID_SOCKET) {
                        closesocket(serverSocket.getSocket());
                        closesocket(cliSock);
                        WSACleanup();
                        throw runtime_error("Client detected, but can't connect to a client. Error # " + to_string(WSAGetLastError()));
                } else cout << "Connection to a client established successfully" << endl;

                cli = {
                    clientInfo,
                    cliSock,
                    clientInfo_size
                };
            }
            
            SAccept(Socket serverSocket): serverSocket(serverSocket){}
            SAccept(){}
            ~SAccept(){}
        };
    }
};