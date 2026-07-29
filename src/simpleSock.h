#pragma once
#include "simpleError.h"
#include <string>
#include <cstring>
#include <iostream>
#include <stdint.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <Winsock2.h>
    #include <Ws2tcpip.h>
    typedef int socklen_t;
#else 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int SOCKET;
    const int SOCKET_ERROR = -1;
    const int INVALID_SOCKET = ~0;
#endif

#define LOG(msgType, msg) \
    std::cout << msgType << "(nsock::" << __func__ << "): " << msg << '\n';

namespace ssock {

enum class ProtocolType : uint8_t {TCP, UDP};
enum class ShutdownType : uint8_t {WRITE, READ, BOTH};

class Socket;
class Address;

class Socket {
private:
    bool         m_isBlocking;
    ProtocolType m_protocol;
    SOCKET       m_sock;

public:
    Socket() = delete;
    Socket(ProtocolType);
    ~Socket();

    int Bind(Address);
    int Listen(int);
    int Accept(Socket&);
    int Accept(Socket&, Address&);
    int Connect(Address);

    int GetSockAddress(Address&);
    int GetPeerAddress(Address&);

    bool IsBlocking();
    bool IsConnected();
    int  SwitchBlockingState();
    ProtocolType GetProtocolType();

    int Write(const char*);
    int Write(const char*, Address&);
    int Read(char*, int);
    int Read(char*, int, Address&);

    int Shutdown(ShutdownType);
    int Close();
};

class Address {
private:
    sockaddr_in m_addr;
    socklen_t   m_addrlen;
        
public:
    Address();
    Address(std::string, uint16_t);
    ~Address();

    int SetAddress(std::string);
    std::string GetAddress();
    void SetPort(uint16_t);
    uint16_t GetPort();

    friend class Socket;
};

// General functions
int WinStartup() {
    int res = 0;
    #ifdef _WIN32
        WSADATA wsaData;
        res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //#else 
        //LOG("WARN", "No action under UNIX");
    #endif
    return res;
}
int WinCleanup() {
    int res = 0;
    #ifdef _WIN32
        res = WSACleanup();
    //#else 
        //LOG("WARN", "No action under UNIX");
    #endif
    return res;
}

// Socket functions
Socket::Socket(ProtocolType protocol) {
    m_isBlocking = true;
    m_protocol = protocol;
    switch(protocol) {
        case ProtocolType::TCP:
            m_sock = socket(AF_INET, SOCK_STREAM, 0);
        break;

        case ProtocolType::UDP:
            m_sock = socket(AF_INET, SOCK_DGRAM, 0);
        break;
    }
}
Socket::~Socket() {
    Shutdown(ShutdownType::BOTH);
    //if (state == SOCKET_ERROR) LOG("ERROR", strerror(errno));
    Close();
    //if (state == SOCKET_ERROR) LOG("ERROR", strerror(errno));
}

int Socket::Bind(Address address) {return bind(m_sock, (sockaddr*) &address.m_addr, address.m_addrlen);}
int Socket::Listen(int backlog) {return listen(m_sock, backlog);}
int Socket::Accept(Socket &peerSock) {
    int res = SOCKET_ERROR;
    if (peerSock.GetProtocolType() != m_protocol) return res;
    peerSock.m_sock = accept(m_sock, nullptr, nullptr);
    if (peerSock.m_sock != INVALID_SOCKET) res = 0;
    return res;
}
int Socket::Accept(Socket &peerSock, Address &peerAddress) {
    int res = SOCKET_ERROR;
    if (peerSock.GetProtocolType() != m_protocol) return res;
    peerSock.m_sock = accept(m_sock, 
                             (sockaddr*) &peerAddress.m_addr,
                             &peerAddress.m_addrlen);
    if (peerSock.m_sock != INVALID_SOCKET) res = 0;
    return res;
}
int Socket::Connect(Address peerAddress) {return connect(m_sock, (sockaddr*) &peerAddress.m_addr, peerAddress.m_addrlen);}

int Socket::GetSockAddress(Address& sockAddress) {return getsockname(m_sock, (sockaddr*) &sockAddress.m_addr, &sockAddress.m_addrlen);}
int Socket::GetPeerAddress(Address& peerAddress) {return getpeername(m_sock, (sockaddr*) &peerAddress.m_addr, &peerAddress.m_addrlen);}
bool Socket::IsBlocking() {return m_isBlocking;}
bool Socket::IsConnected() {
    int errCode;
    socklen_t errCodeSize = sizeof(errCode);
    getsockopt(m_sock, SOL_SOCKET, SO_ERROR, (char*) &errCode, &errCodeSize);
    return (errCode == 0);
}
int Socket::SwitchBlockingState() {
    int res;
    #ifdef _WIN32
        unsigned long mode = (m_isBlocking) ? 1 : 0;
        res = ioctlsocket(m_sock, FIONBIO, &mode);
    #else 
        int flags = fcntl(m_sock, F_GETFL, 0);
        flags = (m_isBlocking) ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        res = fcntl(m_sock, F_SETFL, flags);
    #endif
    if (res != SOCKET_ERROR) m_isBlocking = !m_isBlocking;
    return res;
}
ProtocolType Socket::GetProtocolType() {return m_protocol;}

int Socket::Write(const char *buf) {
    int res = send(m_sock, buf, strlen(buf), 0);
    return res;
}
int Socket::Write(const char *buf, Address &remoteAddr) {
    int res;
    if (m_protocol != ProtocolType::UDP) {
        LOG("ERROR", "Can't send packets to specific remote address in TCP");
        res = SOCKET_ERROR;
    } else {
        res = sendto(m_sock, buf, strlen(buf), 0, (sockaddr*) &remoteAddr.m_addr, remoteAddr.m_addrlen);
    }
    return res;
}
int Socket::Read(char *buf, int bufSize) {
    int res = recv(m_sock, buf, bufSize, 0);
    return res;
}
int Socket::Read(char *buf, int bufSize, Address &remoteAddr) {
    int res;
    if (m_protocol != ProtocolType::UDP) {
        LOG("ERROR", "Can't receive packets from specific remote address in TCP");
        res = SOCKET_ERROR;
    } else {
        res = recvfrom(m_sock, buf, bufSize, 0, (sockaddr*) &remoteAddr.m_addr, &remoteAddr.m_addrlen);
    }
    return res;
}

int Socket::Shutdown(ShutdownType shutdownType) {
    int res;
    #ifdef _WIN32
        switch(shutdownType) {
            case ShutdownType::READ:
                res = shutdown(m_sock, SD_RECEIVE);
            break;
            case ShutdownType::WRITE:
                res = shutdown(m_sock, SD_SEND);
            break;
            case ShutdownType::BOTH:
                res = shutdown(m_sock, SD_BOTH);
            break;
        }
    #else 
        switch(shutdownType) {
            case ShutdownType::READ:
                res = shutdown(m_sock, SHUT_RD);
            break;
            case ShutdownType::WRITE:
                res = shutdown(m_sock, SHUT_WR);
            break;
            case ShutdownType::BOTH:
                res = shutdown(m_sock, SHUT_RDWR);
            break;
        }
    #endif
    return res;
}
int Socket::Close() {
    int res;
    #ifdef _WIN32 
        res = closesocket(m_sock);
    #else 
        res = close(m_sock);
    #endif
    return res;
}

// Address functions
Address::Address() {
    m_addr.sin_family = AF_INET;
    memset(m_addr.sin_zero, 0, sizeof(m_addr.sin_zero));
    SetAddress("0.0.0.0");
    SetPort(0);
    m_addrlen = sizeof(m_addr);
}
Address::Address(std::string addr, uint16_t port) {
    m_addr.sin_family = AF_INET;
    memset(m_addr.sin_zero, 0, sizeof(m_addr.sin_zero));
    SetAddress(addr);
    SetPort(port);
    m_addrlen = sizeof(m_addr);
}
Address::~Address() {}
int Address::SetAddress(std::string addr) {return inet_pton(AF_INET, addr.c_str(), &m_addr.sin_addr);}
std::string Address::GetAddress() {
    char addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &m_addr.sin_addr, addr, INET_ADDRSTRLEN);
    return std::string(addr);
}
void Address::SetPort(uint16_t port) {m_addr.sin_port = htons(port);}
uint16_t Address::GetPort() {return ntohs(m_addr.sin_port);}
};
