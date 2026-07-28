#include "simpleSock.h"

const int BUFFER_SIZE = 512;

class Client {
private:
    int errCode;
    ssock::Socket sock;
    char buffer[BUFFER_SIZE];


public:
    Client() : sock(ssock::ProtocolType::TCP) {memset(buffer, 0, sizeof(buffer));}
    ~Client() {}

    int GetErrorCode() {return errCode;}

    int Connect(ssock::Address serverAddr) {
        errCode = sock.Connect(serverAddr);
        if (errCode != SOCKET_ERROR) {
            ssock::Address addr;
            std::cout << "Connected to server!\n";
            sock.GetSockAddress(addr);
            std::cout << "Local client sock address  = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
            sock.GetPeerAddress(addr);
            std::cout << "Remote server sock address = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
        }
        return errCode;
    }

    void Receive() {
        int readBytes = sock.Read(buffer, BUFFER_SIZE);
        std::cout << "Received " << readBytes << " bytes: " << buffer << '\n';
    }

    void Send(std::string word) {
        int sentBytes = sock.Write(word.c_str());
        std::cout << "Sent " << sentBytes << " bytes: " << word.substr(0, sentBytes) << '\n';
    }
};

int main(int argc, char* argv[]) {
    std::string addr = "127.0.0.1";
    uint16_t    port = 8080;
    if (argc > 1) {
        addr = argv[1];
        if (argc > 2) port = std::atoi(argv[2]);
    }
    ssock::WinStartup();
    {
        Client client;
        std::cout << "Will try to connect to " << addr << ':' << port << '\n'; 
        if (client.Connect(ssock::Address(addr, port)) == SOCKET_ERROR) {
            std::cout << "Failed to connect to remote server!\n";
            return 1;
        }
        #ifdef _WIN32
            client.Send("Hello from .#-WINDOWS-#. client!");
        #else 
            client.Send("Hello from !__UNIX__! client!");
        #endif
        client.Receive();
    }
    ssock::WinCleanup();
}
