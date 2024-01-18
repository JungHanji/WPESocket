#pragma once
#include "Socket.hpp"
#include "pinternal.hpp"

namespace pnet{
    class PClient{
        Socket tsock;

        string ip_serv;
        int    port_serv;

        bool debug;
        string lastReceived;
        string lastSended;

        

        public:
        
        bool enabled = true;
        int tick = 0;

        function<string(PClient&)> sendFunction;
        function<void(PClient&, string)> receiveFunction;

        string getServerIP(){return ip_serv;}
        int    getServerPort(){return port_serv;}

        void connectToServer(){
            int erStat = connect(tsock.getSocket(), (sockaddr*)tsock.getServInfo(), sizeof(*tsock.getServInfo()));
	
            if (erStat != 0) {
                closesocket(tsock.getSocket());
                WSACleanup();
                throw runtime_error("Connection to Server is FAILED. Error # " + to_string(WSAGetLastError()));
            } else if(debug) cout << "Connection established SUCCESSFULLY. Ready to send a message to Server\n";
        }

        void sendToServer(string message){tsock.tsend(tsock.getSocket(), message);}
        string receiveFromServer(){return tsock.trecv(tsock.getSocket());}
        string sendNRecive(string message){
            tsock.tsend(tsock.getSocket(), message);
            return tsock.trecv(tsock.getSocket());
        }

        void disconnect(){
            enabled = false;
            tsock.quitConn();
        }

        void update(string ping_cmd = "-ping"){
            sendToServer(sendFunction(*this));
            string trcv = receiveFromServer();
            if(trcv != ping_cmd)
                if(enabled) receiveFunction(*this, trcv);
            else
                string tmp = receiveFromServer();
            tick++;
        }

        PClient(string ip_serv, int port_serv, bool debug = false): ip_serv(ip_serv), port_serv(port_serv), debug(debug), tsock(ip_serv, port_serv){;}
        PClient(){}
        ~PClient(){
            if(enabled) tsock.quitConn();
            Socket::clean();
        }
    };
};