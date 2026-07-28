#include "simpleSock.h"

const int BUFFER_SIZE = 512;

class Server {
private:
    int errCode;
    ssock::Socket listenSock, clientSock;
    char buffer[BUFFER_SIZE];

public:
    Server() = delete;
    Server(std::string addr, uint16_t port) 
        : listenSock(ssock::ProtocolType::TCP), 
          clientSock(ssock::ProtocolType::TCP) 
    {
        errCode = listenSock.Bind(ssock::Address(addr, port));
        if (errCode != SOCKET_ERROR) errCode = listenSock.Listen(64);
    }
    ~Server() {}

    int GetErrorCode() {return errCode;}

    int GetServerAddress(ssock::Address& outAddr) {return listenSock.GetSockAddress(outAddr);}

    int Accept() {
        ssock::Address addr;
        errCode = listenSock.Accept(clientSock);
        if (errCode != SOCKET_ERROR) {
            std::cout << "Accepted client!\n";
            clientSock.GetSockAddress(addr);
            std::cout << "Local client sock address  = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
            clientSock.GetPeerAddress(addr);
            std::cout << "Remote client sock address = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
        }
        return errCode;
    }

    void Receive() {
        int readBytes = clientSock.Read(buffer, BUFFER_SIZE);
        std::cout << "Received " << readBytes << " bytes: " << buffer << '\n';
    }

    void Reply() {
        #ifdef _WIN32
            std::strcat(buffer, " --- WINDOWS echo server");
        #else 
            std::strcat(buffer, " --- UNIX echo server");
        #endif
        int sentBytes = clientSock.Write(buffer);
        std::cout << "Sent " << sentBytes << " bytes: " << buffer << '\n';
        memset(buffer, 0, BUFFER_SIZE);
    }

    void DisconnectClient() {
        clientSock.Shutdown(ssock::ShutdownType::BOTH);
        clientSock.Close();
    }
};

int main(int argc, char* argv[]) {
    uint16_t port = 8080;
    if (argc > 1) port = std::atoi(argv[1]);
    ssock::WinStartup();
    {
        std::cout << "Will try to start a server at " << port << " port\n";
        Server server("0.0.0.0", 8080);
        if (server.GetErrorCode() == SOCKET_ERROR) {
            std::cout << "Failed to start server!\n";
            return 1;
        }
        ssock::Address serverAddress; 
        server.GetServerAddress(serverAddress);
        std::cout << "Started server at address: " << serverAddress.GetAddress() << ':' << serverAddress.GetPort() << '\n';
        while (true) {
            if (server.Accept() != SOCKET_ERROR) {
                server.Receive();
                server.Reply();
                server.DisconnectClient();
            }
        }
    }
    ssock::WinStartup();
}
