#include "pclient.hpp"

int main(){
    pnet::PClient client("127.0.0.1", 9000);
    client.connectToServer();

    // vector<string> messages{"Hello server", "Some data", "Goodbye server"};

    client.sendFunction = [&](pnet::PClient &cli) -> string{
        // if(cli.tick == 3) {
        //     cli.disconnect(); 
        //     return {};
        // } 

        // return messages[cli.tick];

        string message;
        cout<<"> ";
        getline(cin, message);
        if(message.empty()){
            cli.disconnect();
            return {};
        }

        return {message};
    };

    client.receiveFunction = [&](pnet::PClient &cli, string serverResponce){
        cout << "Received from server: " << serverResponce << endl;
    };

    while(client.enabled) client.update();
}