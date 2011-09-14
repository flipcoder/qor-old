#include <iostream>
#include <sstream>
#include "pn.h"

bool pnInit()
{
#ifdef __WIN32__
    WSADATA wsad;
    if(WSAStartup(0x0202, &wsad)!=NO_ERROR)
    {
        pnRegisterError("WinSock Initialization Failed!");
        return false;
    }
#endif
    return true;
}


void pnClose()
{
#ifdef __WIN32__
    WSACleanup();
#endif
}

std::string pnLastError()
{
    std::ostringstream oss;
    oss.str("");
    
    #ifdef __WIN32__
        int err = WSAGetLastError();
        oss << "WinSock Error: " << err;
    #endif
    
    return oss.str();
}

void pnOutput(const std::string str)
{
    std::cout
        <<str
        <<std::endl;
}


void pnRegisterError(const std::string err)
{
    std::string o = "Error: ";
    o += err;
    pnOutput(o);
}



bool pnSelect(pnSocketID* s)
{
    fd_set fdsRead;
    int fdscount = 0;

    timeval tv = {0,0};
    //tv.tv_sec = 0;
    //tv.tv_usec = 0;

    FD_ZERO(&fdsRead);
    FD_SET(*s, &fdsRead);

#ifndef __WIN32__
    // UNIX ONLY
    // Windows will ignore fdscount anyway
    fdscount = *s + 1;
#endif

    switch(select(fdscount, &fdsRead, (fd_set*)0, (fd_set*)0, &tv))
    {
        case 0:
            return false;
            break;
        case SOCKET_ERROR:
            pnRegisterError("Socket Select Error");
            return false;
        default:
            //return true;
            break;
    }

    if(FD_ISSET(*s, &fdsRead))
        return true;

    return false;
}

