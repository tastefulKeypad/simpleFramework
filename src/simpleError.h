/*
 * !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!
 * !!! THIS HEADER IS NOT MEANT TO BE STANDALONE !!!
 * !!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!
 *
 * SocketError is an enum class based on _WIN32 error codes
 * Under UNIX a dictionary is used to convert from native 
 *   error codes to SocketError and vice versa
*/

#pragma once
#include <cstring>
#include <string>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    //#include <Winsock2.h>
    int WSAGetLastError();
    #include <Winerror.h>
#else 
    #include <errno.h>
    #include <unordered_map>
#endif

typedef int errcode_t;

namespace ssock {
    // NOTE: 
    //   '--- same as X' means that under UNIX there is no 
    //   matching unique error code 
    // That means under UNIX we will never encounter those
    //   errors under normal circumstances, therefore they 
    //   are mapped to closest matching error code
    enum class SocketError : errcode_t {
        Error                = -1,     
        Success                     = 0,      
        InvalidHandle               = 6,      // --- same as EBADF
        NotEnoughMemory             = 8,      // ENOMEM
        InvalidParameter            = 87,     // EINVAL
        OperationAborted            = 995,    // --- same as ECANCELED
        IOPending                   = 997,    // --- same as EINPROGRESS
        InterruptSignal             = 10004,  // EINTR
        BadFileDescriptor           = 10009,  // EBADF
        AccessDenied                = 10013,  // EACCES
        InvalidAddress              = 10014,  // EFAULT
        InvalidValue                = 10022,  // EINVAL
        TooManySockets              = 10024,  // EMFILE
        WouldBlock                  = 10035,  // EWOULDBLOCK
        InProgress                  = 10036,  // EINPROGRESS
        AlreadyInProgress           = 10037,  // EALREADY
        NotSocket                   = 10038,  // ENOTSOCK
        DestinationAddressRequired  = 10039,  // EDESTADDRREQ
        MessageSizeTooLong          = 10040,  // EMSGSIZE
        WrongProtocol               = 10041,  // EPROTOTYPE
        InvalidProtocol             = 10042,  // ENOPROTOOPT
        ProtocolNotSupported        = 10043,  // EPROTONOSUPPORT
        SocketTypeNotSupported      = 10044,  // ESOCKTNOSUPPORT
        OperationNotSupported       = 10045,  // EOPNOTSUPP
        ProtocolFamilyNotSupported  = 10046,  // EPFNOSUPPORT
        AddressFamilyNotSupported   = 10047,  // EAFNOSUPPORT
        AddressAlreadyInUse         = 10048,  // EADDRINUSE
        AddressNotAvailable         = 10049,  // EADDRNOTAVAIL
        NetworkIsDown               = 10050,  // ENETDOWN
        NetworkIsUnreachable        = 10051,  // ENETUNREACH
        ConnectionAbortedByNetwork  = 10052,  // ENETRESET
        ConnectionAborted           = 10053,  // ECONNABORTED
        ConnectionReset             = 10054,  // ECONNRESET
        NoBufferSpace               = 10055,  // ENOBUFS
        AlreadyConnected            = 10056,  // EISCONN
        NotConnected                = 10057,  // ENOTCONN
        UsageAfterShutdown          = 10058,  // ESHUTDOWN
        TooManyReferences           = 10059,  // ETOOMANYREFS
        ConnectionTimeout           = 10060,  // ETIMEDOUT
        ConnectionRefused           = 10061,  // ECONNREFUSED
        TooManySymbolicLinks        = 10062,  // ELOOP
        NameTooLong                 = 10063,  // ENAMETOOLONG
        HostIsDown                  = 10064,  // EHOSTDOWN
        HostIsUnreachable           = 10065,  // EHOSTUNREACH
        DirectoryNotEmpty           = 10066,  // ENOTEMPTY
        TooManyProcesses            = 10067,  // --- same as EMFILE
        TooManyUsers                = 10068,  // EUSERS
        DiskQuotaExceeded           = 10069,  // EDQUOT
        StaleFileHandle             = 10070,  // ESTALE
        ObjectIsRemote              = 10071,  // EREMOTE
        WinNetworkSystemNotReady    = 10091,  // --- WIN SPECIFIC
        WinVersionNotSupported      = 10092,  // --- WIN SPECIFIC
        WinNotInitialized           = 10093,  // --- WIN SPECIFIC
        DisconnectIsInProgress      = 10101,  // --- same as EINPROGRESS
        Cancelled                   = 10103   // ECANCELED
    };

    #ifdef _WIN32
    #else
        const std::unordered_map<errcode_t, SocketError> unixErrorToSocketError = {
            {-1,              SocketError::Error},                       
            {0,               SocketError::Success},                     
            {EBADF,           SocketError::InvalidHandle},               
            {ENOMEM,          SocketError::NotEnoughMemory},             
            {EINVAL,          SocketError::InvalidParameter},            
            {ECANCELED,       SocketError::OperationAborted},            
            {EINPROGRESS,     SocketError::IOPending},                   
            {EINTR,           SocketError::InterruptSignal},             
            {EBADF,           SocketError::BadFileDescriptor},           
            {EACCES,         SocketError::AccessDenied},                
            {EFAULT,          SocketError::InvalidAddress},              
            {EINVAL,          SocketError::InvalidValue},                
            {EMFILE,          SocketError::TooManySockets},              
            {EWOULDBLOCK,     SocketError::WouldBlock},                  
            {EINPROGRESS,     SocketError::InProgress},                  
            {EALREADY,        SocketError::AlreadyInProgress},           
            {ENOTSOCK,        SocketError::NotSocket},                   
            {EDESTADDRREQ,    SocketError::DestinationAddressRequired},  
            {EMSGSIZE,        SocketError::MessageSizeTooLong},          
            {EPROTOTYPE,      SocketError::WrongProtocol},               
            {ENOPROTOOPT,     SocketError::InvalidProtocol},             
            {EPROTONOSUPPORT, SocketError::ProtocolNotSupported},        
            {ESOCKTNOSUPPORT, SocketError::SocketTypeNotSupported},      
            {EOPNOTSUPP,      SocketError::OperationNotSupported},       
            {EPFNOSUPPORT,    SocketError::ProtocolFamilyNotSupported},  
            {EAFNOSUPPORT,    SocketError::AddressFamilyNotSupported},   
            {EADDRINUSE,      SocketError::AddressAlreadyInUse},         
            {EADDRNOTAVAIL,   SocketError::AddressNotAvailable},         
            {ENETDOWN,        SocketError::NetworkIsDown},               
            {ENETUNREACH,     SocketError::NetworkIsUnreachable},        
            {ENETRESET,       SocketError::ConnectionAbortedByNetwork},  
            {ECONNABORTED,    SocketError::ConnectionAborted},           
            {ECONNRESET,      SocketError::ConnectionReset},             
            {ENOBUFS,         SocketError::NoBufferSpace},               
            {EISCONN,         SocketError::AlreadyConnected},            
            {ENOTCONN,        SocketError::NotConnected},                
            {ESHUTDOWN,       SocketError::UsageAfterShutdown},          
            {ETOOMANYREFS,    SocketError::TooManyReferences},           
            {ETIMEDOUT,       SocketError::ConnectionTimeout},           
            {ECONNREFUSED,    SocketError::ConnectionRefused},           
            {ELOOP,           SocketError::TooManySymbolicLinks},        
            {ENAMETOOLONG,    SocketError::NameTooLong},                 
            {EHOSTDOWN,       SocketError::HostIsDown},                  
            {EHOSTUNREACH,    SocketError::HostIsUnreachable},           
            {ENOTEMPTY,       SocketError::DirectoryNotEmpty},           
            {EMFILE,          SocketError::TooManyProcesses},            
            {EUSERS,          SocketError::TooManyUsers},                
            {EDQUOT,          SocketError::DiskQuotaExceeded},           
            {ESTALE,          SocketError::StaleFileHandle},             
            {EREMOTE,         SocketError::ObjectIsRemote},              
            {EINPROGRESS,     SocketError::DisconnectIsInProgress},      
            {ECANCELED,       SocketError::Cancelled}
        };

        const std::unordered_map<SocketError, errcode_t> socketErrorToUnixError = {
            {SocketError::Error,                -1},
            {SocketError::Success,                     0},                    
            {SocketError::InvalidHandle,               EBADF},
            {SocketError::NotEnoughMemory,             ENOMEM},
            {SocketError::InvalidParameter,            EINVAL},
            {SocketError::OperationAborted,            ECANCELED},
            {SocketError::IOPending,                   EINPROGRESS},
            {SocketError::InterruptSignal,             EINTR},
            {SocketError::BadFileDescriptor,           EBADF},
            {SocketError::AccessDenied,                EACCES},
            {SocketError::InvalidAddress,              EFAULT},
            {SocketError::InvalidValue,                EINVAL},
            {SocketError::TooManySockets,              EMFILE},
            {SocketError::WouldBlock,                  EWOULDBLOCK},
            {SocketError::InProgress,                  EINPROGRESS},
            {SocketError::AlreadyInProgress,           EALREADY},
            {SocketError::NotSocket,                   ENOTSOCK},
            {SocketError::DestinationAddressRequired,  EDESTADDRREQ},
            {SocketError::MessageSizeTooLong,          EMSGSIZE},
            {SocketError::WrongProtocol,               EPROTOTYPE},
            {SocketError::InvalidProtocol,             ENOPROTOOPT},
            {SocketError::ProtocolNotSupported,        EPROTONOSUPPORT},
            {SocketError::SocketTypeNotSupported,      ESOCKTNOSUPPORT},
            {SocketError::OperationNotSupported,       EOPNOTSUPP},
            {SocketError::ProtocolFamilyNotSupported,  EPFNOSUPPORT},
            {SocketError::AddressFamilyNotSupported,   EAFNOSUPPORT},
            {SocketError::AddressAlreadyInUse,         EADDRINUSE},
            {SocketError::AddressNotAvailable,         EADDRNOTAVAIL},
            {SocketError::NetworkIsDown,               ENETDOWN},
            {SocketError::NetworkIsUnreachable,        ENETUNREACH},
            {SocketError::ConnectionAbortedByNetwork,  ENETRESET},
            {SocketError::ConnectionAborted,           ECONNABORTED},
            {SocketError::ConnectionReset,             ECONNRESET},
            {SocketError::NoBufferSpace,               ENOBUFS},
            {SocketError::AlreadyConnected,            EISCONN},
            {SocketError::NotConnected,                ENOTCONN},
            {SocketError::UsageAfterShutdown,          ESHUTDOWN},
            {SocketError::TooManyReferences,           ETOOMANYREFS},
            {SocketError::ConnectionTimeout,           ETIMEDOUT},
            {SocketError::ConnectionRefused,           ECONNREFUSED},
            {SocketError::TooManySymbolicLinks,        ELOOP},
            {SocketError::NameTooLong,                 ENAMETOOLONG},
            {SocketError::HostIsDown,                  EHOSTDOWN},
            {SocketError::HostIsUnreachable,           EHOSTUNREACH},
            {SocketError::DirectoryNotEmpty,           ENOTEMPTY},
            {SocketError::TooManyProcesses,            EMFILE},
            {SocketError::TooManyUsers,                EUSERS},
            {SocketError::DiskQuotaExceeded,           EDQUOT},
            {SocketError::StaleFileHandle,             ESTALE},
            {SocketError::ObjectIsRemote,              EREMOTE},
            {SocketError::WinNetworkSystemNotReady,    -1},
            {SocketError::WinVersionNotSupported,      -1},
            {SocketError::WinNotInitialized,           -1},
            {SocketError::DisconnectIsInProgress,      EINPROGRESS},
            {SocketError::Cancelled,                   ECANCELED}
        };
    #endif


    errcode_t GetLastNativeError() {
        errcode_t errCode;
        #ifdef _WIN32 
            errCode = WSAGetLastError();
        #else 
            errCode = errno;
        #endif
        return errCode;
    }

    SocketError GetLastError() {
        SocketError errCode;
        #ifdef _WIN32 
            errCode = static_cast<SocketError>(GetLastNativeError());
        #else 
            auto it = unixErrorToSocketError.find(GetLastNativeError());
            if (it == unixErrorToSocketError.end()) 
                 errCode = SocketError::Error;
            else errCode = it->second;
        #endif
        return errCode;
    }

    std::string GetNativeErrorMsg(errcode_t errCode) {
        #ifdef _WIN32 
            static char errMsg[256] = {0};
            FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                0, errCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                errMsg, 256, 0);
            return std::string(errMsg);
        #else 
            return std::string(strerror(errCode));
        #endif
    }
    std::string GetErrorMsg(SocketError errCode) {
        #ifdef _WIN32 
            return GetNativeErrorMsg(static_cast<errcode_t>(errCode));
        #else 
            auto it = socketErrorToUnixError.find(errCode);
            if (it == socketErrorToUnixError.end()) 
                return std::string("ERROR(ssock::GetErrorMsg): Failed to convert SocketError to native error!");
            return GetNativeErrorMsg(it->second);
        #endif
    }
};

