#ifndef PN_H_INCLUDED
#define PN_H_INCLUDED

#ifdef _MSC_VER
	#ifndef __WIN32__
		#define __WIN32__
	#endif
#endif

#ifdef __WIN32__
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <winsock2.h>
    typedef int socklen_t;
    #define pnSleep(x) Sleep(x)
    //#define pnClear() system("cls")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #define closesocket close
    #define ioctlsocket ioctl
    typedef int SOCKET;
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
    #define pnSleep(x) usleep( (x)*1000 )
    //#define pnClear() system("clear")
#endif

#include <cstring>
#include <string>

// ProlificNet Types
typedef unsigned char pnByte;
typedef unsigned int pnUInt;
typedef unsigned short pnUShort;

typedef sockaddr_in pnAddrInfo;
typedef SOCKET pnSocketID;

/*#ifdef __WIN32__
    typedef BOOL bool;
    #define true TRUE
    #define false FALSE
#else*/
    //typedef bool bool;
    //#define true true
    //#define false false
//#endif


#define PN_RECV_ERROR -1
#define PN_RECV_DISCONNECT 0

#define PN_ACCEPT_INVALID INVALID_SOCKET

// ProlificPlay Functions

// pnInit : Initializes ProlificNet. Returns true if successful
bool pnInit();
// pnClose : Closes ProlificNet
void pnClose();

// These functions are used inside of ProlificNet and are not
// intended for any other use.
void pnRegisterError(std::string err);
void pnOutput(std::string str);

std::string pnLastError();

//int pn_ftoi(float b);
//float pn_itof(int b);

// pnSelect: Returns whether data is waiting to be read on the socket
// with the respective socket id.  For selecting pnSockets,
// use the dataReady() method instead.
bool pnSelect(pnSocketID* s);

class pnSocket;
class pnBuffer;
class pnAddr;


class pnSocket
{
    protected:
        pnSocketID m_sSocket;
        bool m_bOpen;

    public:
        pnSocket(){
            m_bOpen = false;
        }
        virtual ~pnSocket(){
            if(m_bOpen)
                closesocket(m_sSocket);
        };
        
        bool isOpen() { return m_bOpen; }

        virtual bool openSocket() = 0;

        pnSocketID get() { return m_sSocket; }

        bool setUsingID(pnSocketID s){
            if(s!=INVALID_SOCKET)
            {
                m_sSocket = s;
                m_bOpen = true;
                return true;
            }
            return false;
        }


        bool bindPort(pnUShort port);
        void closeSocket();
        bool dataReady();

        bool connectTo(const char* ip, pnUShort port);

        void sendPacket(pnBuffer* data);
        int recvPacket(pnBuffer* data);

		bool select() { return pnSelect(&m_sSocket); }
        //std::string ip();
        //pnUInt port();

};


// UDP Socket Class
class pnUDPSocket : public pnSocket
{
    public:

        pnUDPSocket(){}
        virtual ~pnUDPSocket(){
			//if(m_bOpen)
            //    closesocket(m_sSocket);
		}

        virtual bool openSocket();

        void sendPacketTo(pnBuffer* data, pnAddr& addr);
        int recvPacketFrom(pnBuffer* data, pnAddr& addr);

        //bool sendTo();
        //bool recvFrom();


};

class pnTCPSocket : public pnSocket
{
    public:

        pnTCPSocket(){}
        virtual ~pnTCPSocket(){
			//if(m_bOpen)
            //    closesocket(m_sSocket);
		}

        virtual bool openSocket();

        pnSocketID acceptConnection(pnAddr& connectionAddr);
        bool listenFor(int backlog);
};


// pnBuffer: ProlificNet's Buffer Class
//  Used for storing data of different data types in a single
//  array of bytes. Buffers can be utilized as packets, using
//  the pnSocket methods sendPacket() and recvPacket()
class pnBuffer
{
    public:
        pnByte* buf;
        int size; // Size of data in buffer
        int allocatedSize; // Full capacity of the buffer, in bytes
        bool overflow;
        
        //static void instance(int r)
        //{
        //    static unsigned int inst = 0;
        //    inst += r;
        //    //std::cout<<"pnBuffer instances: "<<inst<<std::endl;
        //}

        pnBuffer(){
			buf = NULL;
            allocatedSize = 0;
            size = 0;
            overflow = false;
        }

        pnBuffer(int alloc){
			buf = NULL;
            allocatedSize = 0;
            size=0;
            overflow = false;
            allocate(alloc);
        }

        pnBuffer(const pnBuffer& old) {
            buf = NULL;
            
            size = old.size;
            allocatedSize = old.allocatedSize;
            overflow = old.overflow;
            
            allocate(old.allocatedSize);
            for(int i=0; i<size; i++)
                buf[i] = old.buf[i];
        }

        virtual ~pnBuffer(){
            freeMemory();
        }

        void clear(){
            if(buf)
                memset(buf, 0, sizeof(buf));
            size = 0;
            overflow = false;
        }

        void freeMemory(){
            if(buf)
            {
                //instance(-1);
                delete [] buf;
                buf = NULL;
            }
        }

        int spaceLeft(){
            if(overflow)
                return 0;
            return (allocatedSize-size);
        }

        pnByte* getSpace(int len);
        void    allocate(int bytes);

        void    writeRaw(void* data, int len);
        void    writeByte(pnByte b);
        void    writeShort(short b);
        void    writeInt(int b);
        //void    writeFloat(float b);
        void    writeString(const char* b, int len = -1);

        void*   readRaw(int& pos);
        pnByte  readByte(int& pos);
        short   readShort(int& pos);
        int     readInt(int& pos);
        //float   readFloat(int& pos);
		std::string readString(int& pos, int length = -1);
};

class pnAddr
{
	public:
    
        pnAddrInfo m_sockAddr;
        //std::string m_ip;
	
    
        pnAddr(){
            clear();
        }
        ~pnAddr() {}
        
        /*pnAddr(const pnAddr& a){
            clear();
            setFromInfo(a.getInfo());
        }*/

        void clear() {
            memset(&m_sockAddr, 0, sizeof(m_sockAddr));
            m_sockAddr.sin_family = AF_INET;
            memset(m_sockAddr.sin_zero, '\0', sizeof(m_sockAddr.sin_zero));
        }

        void set(const char* ip, pnUShort port){
            //m_ip = ip;
            m_sockAddr.sin_addr.s_addr = inet_addr(ip);
            m_sockAddr.sin_port = htons(port);
        }

        void setIP(const char* ip){
            m_sockAddr.sin_addr.s_addr = inet_addr(ip);
        }
        void setPort(pnUShort port){
            m_sockAddr.sin_port = htons(port);
        }

        void setFromInfo(pnAddrInfo* info){
            if(!info)
                return;
            m_sockAddr.sin_family = info->sin_family;
			m_sockAddr.sin_addr.s_addr = info->sin_addr.s_addr;
            m_sockAddr.sin_port = info->sin_port;
            memset(m_sockAddr.sin_zero, '\0', sizeof(m_sockAddr.sin_zero));
        }

        std::string ip() {
            return (std::string)(inet_ntoa(m_sockAddr.sin_addr));
        };
        
        pnUShort getPort() { return m_sockAddr.sin_port; }

        pnAddrInfo* getInfo() { return &m_sockAddr; }
};

#endif
