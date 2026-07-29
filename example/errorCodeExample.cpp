#include "simpleSock.h"

int main() {
    ssock::WinStartup();
    {
        std::cout << "Native error message (0) = " << ssock::GetNativeErrorMsg(0) << '\n';
        std::cout << "Native error message (-1) = " << ssock::GetNativeErrorMsg(-1) << '\n';
        ssock::Socket sock(ssock::ProtocolType::TCP);
        ssock::Address malformedAddr("999.999.999.999", 22);
        errcode_t res = sock.Bind(malformedAddr);
        
        if (res == SOCKET_ERROR) {
            std::cout << "Failed to bind socket!\n";
            errcode_t ec1 = ssock::GetLastNativeError();
            errcode_t ec2 = ssock::GetLastNativeError();
            std::cout << "Native errcode = " << ec1 << " & " << ec2 << '\n';
            std::cout << "Native error message = " << ssock::GetNativeErrorMsg(ec1) << '\n';
            ssock::SocketError se1 = ssock::GetLastError(),
                               se2 = ssock::GetLastError();

            std::cout << "OS-agnostic errcode = " << static_cast<errcode_t>(se1) << " & " << static_cast<errcode_t>(se2) << '\n';
            std::cout << "OS-agnostic error message = " << ssock::GetNativeErrorMsg(ec1) << '\n';
        }
    }
    ssock::WinCleanup();
}
