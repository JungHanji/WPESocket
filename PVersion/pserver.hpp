#pragma once
#include "Socket.hpp"
#include "pinternal.hpp"


namespace pnet{

    class PServer{
        std::unique_ptr<std::thread> acceptConnectionThread;
        mutex cliMutex, tsockMutex;
        
        Socket tsock;
        internal::Client nclient = internal::Client(true);
        map<int, internal::Client> clis;

        string ip;
        int port;

        bool debug;
        int nclients = 0;

        public:
        function<void(internal::Client&, int)> clientAccept, clientDisconnect;
        function<string(internal::Client&, int, string)> clientUpdate;

        void internal_clientAccept(PServer *iserv){
            while(true){
                iserv->getSocketMutex().lock();
                    internal::SAccept tmpapt(iserv->getSocket());
                iserv->getSocketMutex().unlock();
                
                internal::Client tmpcli = tmpapt.accpetCon();
                
                iserv->getClientMutex().lock();
                    iserv->clis[iserv->clis.size()] = tmpcli;
                    int size = iserv->clis.size();
                    iserv->clientAccept(tmpcli, size-1);
                    
                    iserv->clis[size-1].getThread() = make_unique<thread>( thread(
                            [&](PServer *serv, int id){
                                serv->getClientMutex().lock();
                                    bool &enabled = serv->getPointerClinet(id)->enabled;
                                serv->getClientMutex().unlock();
                                while(enabled){
                                    serv->internal_clientUpdate(serv, id);
                                }
                            }, this, size-1
                        )
                    );
                iserv->getClientMutex().unlock();
            }
        }

        internal::Client *getPointerClinet(int id){

            if(isKeyInMap(clis, id)){
                return &clis[id];
            } else {
                cout<<"[ERROR] Cant get client pointer: index is not exists: " + to_string(id) + '\n';
                return &nclient;
            }
        }
        
        void internal_clientUpdate(PServer *iserv, int i){
            // iserv->getSocketMutex().lock();
                string recvstr = iserv->getSocket().trecv(iserv->getPointerClinet(i)->getISocket());
                if(!recvstr.empty()){
                    if(
                        iserv->getSocket().tsend(
                            iserv->getPointerClinet(i)->getISocket(), 
                            iserv->clientUpdate(
                                *iserv->getPointerClinet(i), i,
                                recvstr
                            )
                        ) != 0)
                    {
                        
                        closesocket(iserv->getPointerClinet(i)->getISocket());
                        clientDisconnect(*iserv->getPointerClinet(i), i);
                        iserv->getPointerClinet(i)->enabled = false;
                    
                    }
                } else {
                    if(iserv->getSocket().tsend(iserv->getPointerClinet(i)->getISocket(), "-ping")){
                        closesocket(iserv->getPointerClinet(i)->getISocket());
                        clientDisconnect(*iserv->getPointerClinet(i), i);
                        iserv->getPointerClinet(i)->enabled = false;
                    }
                }
            // iserv->getSocketMutex().unlock();
        }

        void listenPkgs(){
            int erStat = listen(tsock.getSocket(), SOMAXCONN);

            if ( erStat == -1 ) {
                closesocket(tsock.getSocket());
                WSACleanup();
                throw runtime_error("Can't start to listen to. Error # " + to_string(errno));
            } else if(debug) {  
                cout << "Listening..." << endl;
            }
        }

        void listenAccepts(){
            acceptConnectionThread = make_unique<thread>(thread([&](PServer *serv){serv->internal_clientAccept(serv);}, this));
            acceptConnectionThread->detach();
        }

        void deleteDisabledClients(){
            // vector<int> ids = adc::getKeys(clis);
            // for(int i = 0; i < ids.size(); i++){
            //     if(!clis[ids[i]].enabled){
            //         clis.erase(ids[i]);
            //     }
            // }
        }

        Socket &getSocket(){return tsock;}
        mutex  &getClientMutex(){return cliMutex;}
        mutex  &getSocketMutex(){return tsockMutex;}

        PServer(string ip, int port, bool debug = false): ip(ip), port(port), debug(debug), tsock(ip, port) {
            int erStat;

            erStat = bind(tsock.getSocket(), (sockaddr*)tsock.getServInfo(), sizeof(*tsock.getServInfo()));
            if ( erStat != 0 ) {
                closesocket(tsock.getSocket());
                WSACleanup();
                throw runtime_error("Error Socket binding to server info. Error # " + to_string(WSAGetLastError()));
            } else if(debug) cout << "Binding socket to Server info is OK" << endl;
        }
        PServer(){}
        ~PServer(){
            tsock.quitConn();
            Socket::clean();
        }
    };

}