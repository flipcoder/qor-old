#include "pn.h"

// Base Socket Methods

bool pnSocket :: bindPort(pnUShort port)
{
    sockaddr_in sAddr;

    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sAddr.sin_port = htons(port);
    memset(sAddr.sin_zero, '\0', sizeof(sAddr.sin_zero));

    if(bind(m_sSocket, (sockaddr*)&sAddr, sizeof(sAddr)) == SOCKET_ERROR)
    {
        pnRegisterError("Could not bind socket to port");
        return false;
    }

    return true;
}

void pnSocket :: closeSocket()
{
    if(m_bOpen)
    {
        closesocket(m_sSocket);
        m_bOpen = false;
    }
}

bool pnSocket :: dataReady()
{
    return (pnSelect(&m_sSocket)==true);
}

bool pnSocket :: connectTo(const char* ip, pnUShort port)
{
    sockaddr_in destAddr;
    hostent* he;
    char* connectip;

    he = gethostbyname((char*)ip);
    if(he==NULL)
    {
        // use normal IP
        connectip = (char*)ip;
    }
    else
    {
        // use hostname IP
        connectip = (char*)inet_ntoa(*(in_addr*)he->h_addr);
    }

    inet_ntoa(*(in_addr*)he->h_addr);

    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    destAddr.sin_addr.s_addr = inet_addr(connectip);
    memset(&(destAddr.sin_zero), '\0', sizeof(destAddr.sin_zero));

    if(connect(m_sSocket, (sockaddr*)&destAddr, sizeof(destAddr))==SOCKET_ERROR)
        return false;
    return true;
}

void pnSocket :: sendPacket(pnBuffer* data)
{
    int bytesSent = 0;
    int bytesLeft = data->size;
    int n = 0;

    while(bytesSent < data->size)
    {
        n = send(m_sSocket, (char*)data->buf, bytesLeft, 0);
        if(n==SOCKET_ERROR)
            return;
        bytesSent+=n;
        bytesLeft-=n;
    }
}

int pnSocket :: recvPacket(pnBuffer* data)
{
    int n = 0;

    n = recv(m_sSocket, (char*)data->buf, data->allocatedSize-1, 0);
    if(n==SOCKET_ERROR) {
        pnRegisterError(pnLastError());
        return -1;
    }
    data->size = n;

    return n;
}



// UDP Socket

bool pnUDPSocket :: openSocket()
{
    if(m_bOpen)
        closeSocket();

    //int PN_SOCKET_YES = 1;
    unsigned long PN_SOCKET_BLOCK = 1L;

    m_sSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_sSocket == INVALID_SOCKET)
    {
        m_bOpen = false;
        pnRegisterError("openSocket (Invalid Socket)");
        return m_bOpen;
    }
    else
        m_bOpen = true;

    //if(setsockopt(m_sSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&PN_SOCKET_YES, sizeof(int)))
    //    pnRegisterError("openSocket (setsockopt)");

    if(ioctlsocket(m_sSocket, FIONBIO, &PN_SOCKET_BLOCK) == SOCKET_ERROR)
        pnRegisterError("openSocket (ioctl)");

    return m_bOpen;
}

void pnUDPSocket :: sendPacketTo(pnBuffer* data, pnAddr& addr)
{
    pnAddrInfo& caddr = *addr.getInfo();
    //int bytesSent = 0;
    int bytesLeft = data->size;
    int n;

    while(bytesLeft > 0)
    {
        n = sendto(m_sSocket, (char*)data->buf, bytesLeft, 0,
            (sockaddr*)&caddr, sizeof(caddr));
        if(n==SOCKET_ERROR) {
			pnRegisterError("send Socket Error");
            return;
		}
        //bytesSent+=n;
        bytesLeft-=n;
    }
}

int pnUDPSocket :: recvPacketFrom(pnBuffer* data, pnAddr& addr)
{
    pnAddrInfo& caddr = *addr.getInfo();
    socklen_t caddr_size = sizeof(caddr);
    int n = 0;

    n = recvfrom(m_sSocket, (char*)data->buf, data->allocatedSize-1, 0,
        (sockaddr*)&caddr, &caddr_size);

    if(n==SOCKET_ERROR) {
		pnRegisterError(pnLastError());
        return -1;
	}

    data->size = n;
    return n;
}


// TCP Socket

bool pnTCPSocket :: openSocket()
{
    if(m_bOpen)
        closeSocket();

    m_sSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_sSocket == INVALID_SOCKET)
        m_bOpen = false;
    else
        m_bOpen = true;


    return m_bOpen;
}

pnSocketID pnTCPSocket :: acceptConnection(pnAddr& connectionAddr)
{
    pnAddrInfo* caddr = connectionAddr.getInfo();

    socklen_t addrSize = sizeof(*caddr);

    SOCKET fd = accept(m_sSocket,
        (sockaddr *)caddr,//zxc
        &addrSize);

    if(fd==INVALID_SOCKET)
    {
        pnRegisterError("Invalid Socket");
        return PN_ACCEPT_INVALID;
    }

    return fd;
}

bool pnTCPSocket :: listenFor(int backlog)
{
    return (listen(m_sSocket,backlog)!=SOCKET_ERROR);
}

// Buffer


