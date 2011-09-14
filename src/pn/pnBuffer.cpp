#include "pn.h"

void pnBuffer :: allocate(int bytes)
{
    freeMemory();
    buf = new pnByte[bytes];
    if(!buf)
        pnRegisterError("pnBuffer: Failed to allocate memory.");
    memset(buf, 0, sizeof(buf));
    allocatedSize = bytes;
    size=0;
    
    instance(1);
}

pnByte* pnBuffer :: getSpace(int len)
{
    if(spaceLeft() < len) { // spaceLeft() <= 0
        overflow = true;
        return NULL;
    }

    int idx = size;
    size += len;

    return &buf[idx];
}

void pnBuffer :: writeRaw(void* data, int len)
{
    pnByte* ba;
    ba = getSpace(len);
    if(!ba)
        return;

    memcpy(ba, data, len);
    //else
    //    pnRegisterError("Warning: Data write failed due to buffer overflow.");
}

void pnBuffer :: writeByte(pnByte b)
{
    pnByte* ba;
    ba = getSpace(1);
    if(!ba)
        return;

    ba[0] = b;
}

void pnBuffer :: writeShort(short b)
{
    pnByte* ba;
    ba = getSpace(2);
    if(!ba)
        return;

    ba[0] = b & 0xff;
    ba[1] = (b>>8);
}

void pnBuffer :: writeInt(int b)
{
    pnByte* ba;
    ba = getSpace(4);
    if(!ba)
        return;

    ba[0] = b & 0xff;
    ba[1] = (b>>8) & 0xff;
    ba[2] = (b>>16) & 0xff;
    ba[3] = (b>>24);
}

/*void pnBuffer :: writeFloat(float b)
{
    writeInt( pn_ftoi(b) );
}*/

void pnBuffer :: writeString(const char* b, int length)
{
    int len = (length==-1)?(strlen(b)):length;
    if(!b)
        return;

    writeRaw((void*)b, len);
    writeByte(0);
}


void*   pnBuffer :: readRaw(int& pos)
{
    if((pos<0) || (pos > size)) {
        pnRegisterError("Warning: Prevented Overflow");
        overflow = true;
        return NULL;
    }

    return (void*)&buf[pos];
}

pnByte  pnBuffer :: readByte(int& pos)
{
    if((pos<0) || (pos > (size-1)))
    {
         pnRegisterError("Warning: Prevented Overflow");
         overflow = true;
         return 0;
    }

    return (pnByte)buf[pos++];
}

short   pnBuffer :: readShort(int& pos)
{
    if(pos<0 || pos > (size-2))
    {
         pnRegisterError("Warning: Prevented Overflow");
         overflow = true;
         return 0;
    }

    short n = (buf[pos]) |
            (buf[pos+1]<<8);

    pos+=2;
    return n;
}

int     pnBuffer :: readInt(int& pos)
{
    if(pos<0 || pos > (size-4))
    {
         pnRegisterError("Warning: Prevented Overflow");
         overflow = true;
         return 0;
    }

    int n = (buf[pos]) +
            (buf[pos+1]<<8) +
            (buf[pos+2]<<16) +
            (buf[pos+3]<<24);

    pos+=4;
    return n;
}

std::string pnBuffer :: readString(int& pos, int length)
{
    int len = (length==-1)?(strlen((char*)&buf[pos])):length;
    if(pos<0 || pos > (size - len))
    {
        pnRegisterError("Warning: Prevented Overflow");
        overflow = true;
        return "";
    }
    pos += len;

	std::string s = (char*)&buf[pos-len];
    return s;
}


/*float   pnBuffer :: readFloat(int& pos)
{
    return pn_itof(readInt(pos));
}*/
