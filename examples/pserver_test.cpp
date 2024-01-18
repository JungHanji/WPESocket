#include "pserver.hpp"

int main(){
    pnet::PServer server("127.0.0.1", 9000);
    
    server.clientAccept = [&](pnet::internal::Client &cli, int index){
        cout<<"Client #"+to_string(index)+" connected\n";
    };
    
    server.clientDisconnect = [&](pnet::internal::Client &cli, int index){
        cout<<"Client #"+to_string(index)+" disconnected\n";
    };

    server.clientUpdate = [&](pnet::internal::Client &cli, int index, string outputFromCli){
        cout<<"Recevied from client #"+ to_string(index) + ' ' + cli.ip + ':' + to_string(cli.port) + ": " + outputFromCli + '\n';
        return "Ok, server recevied";
    };

    server.listenPkgs();
    server.listenAccepts();
    while (true) ; // make something other
}