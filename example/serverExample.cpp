#include "simpleSock.h"

const int BUFFER_SIZE = 512;

void LogError(std::string msg) {
    std::cout << msg << '\n' << "Reason: "
              << ssock::GetErrorMsg(ssock::GetLastError()) << '\n';
}

class Server {
private:
    ssock::Socket listenSock, clientSock;
    char buffer[BUFFER_SIZE];

public:
    Server() 
        : listenSock(ssock::ProtocolType::TCP), 
          clientSock(ssock::ProtocolType::TCP) {}
    ~Server() {}

    errcode_t BindAndListen(std::string addr, uint16_t port) {
        errcode_t ec = listenSock.Bind(ssock::Address(addr, port));
        if (ec != SOCKET_ERROR) ec = listenSock.Listen(64);
        return ec;
    }

    errcode_t GetServerAddress(ssock::Address& outAddr) {return listenSock.GetSockAddress(outAddr);}

    int Accept() {
        errcode_t ec;
        ssock::Address addr;
        ec = listenSock.Accept(clientSock);
        if (ec != SOCKET_ERROR) {
            std::cout << "\nAccepted client!\n";
            clientSock.GetSockAddress(addr);
            std::cout << "Local client sock address  = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
            clientSock.GetPeerAddress(addr);
            std::cout << "Remote client sock address = " << addr.GetAddress() << ':' << addr.GetPort() << '\n';
        }
        return ec;
    }

    void Receive() {
        memset(buffer, 0, BUFFER_SIZE);
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
    }

    void DisconnectClient() {
        clientSock.Shutdown(ssock::ShutdownType::BOTH);
        clientSock.Close();
    }
};

int main(int argc, char* argv[]) {
    uint16_t port = 8080;
    Server server;
    if (argc > 1) port = std::atoi(argv[1]);
    ssock::WinStartup();
    {
        std::cout << "Will try to start a server at " << port << " port\n";
        if (server.BindAndListen("0.0.0.0", port) == SOCKET_ERROR) {
            LogError("Failed to start a server");
            return static_cast<errcode_t>(ssock::GetLastError());
        }
        ssock::Address serverAddress; 
        server.GetServerAddress(serverAddress);
        std::cout << "Started server at address: " << serverAddress.GetAddress() << ':' << serverAddress.GetPort() << '\n';
        while (true) {
            if (server.Accept() != SOCKET_ERROR) {
                server.Receive();
                server.Reply();
                server.DisconnectClient();
            } else LogError("Failed to accept client connection");
        }
    }
    ssock::WinStartup();
}
