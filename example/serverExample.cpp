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

    errcode_t BindAndListen(std::string addrIn, uint16_t port) {
        std::cout << "Will try to bind a server at " << port << " port\n";
        if (listenSock.Bind(ssock::Address(addrIn, port)) == SOCKET_ERROR) 
            return SOCKET_ERROR;
        if (listenSock.Listen(64) == SOCKET_ERROR) 
            return SOCKET_ERROR;

        ssock::Address addr; 
        listenSock.GetSockAddress(addr);
        std::cout << "Server started listening at address: " << addr.GetFullAddress() << '\n';
        return SUCCESS;
    }

    errcode_t Accept() {
        ssock::Address addr;
        if (listenSock.Accept(clientSock) == SOCKET_ERROR)
            return SOCKET_ERROR;

        std::cout << "\nAccepted client!\n";
        clientSock.GetSockAddress(addr);
        std::cout << "Local client sock address  = " << addr.GetFullAddress() << '\n';
        clientSock.GetPeerAddress(addr);
        std::cout << "Remote client sock address = " << addr.GetFullAddress() << '\n';
        return SUCCESS;
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
    if (argc > 1) port = std::atoi(argv[1]);
    ssock::WinStartup();
    {
        Server server;
        if (server.BindAndListen("0.0.0.0", port) == SOCKET_ERROR) {
            LogError("Failed to start a server");
            return static_cast<errcode_t>(ssock::GetLastError());
        }
        while (true) {
            if (server.Accept() != SOCKET_ERROR) {
                server.Receive();
                server.Reply();
                server.DisconnectClient();
            } else LogError("Failed to accept client connection");
        }
    }
    ssock::WinCleanup();
}
