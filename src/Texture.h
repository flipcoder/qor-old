#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "GfxAPI.h"
#include "IFallible.h"
#include <string>

class Texture : public IFallible
{
    public:

        enum Flags {
            TRANSPARENT = BIT(0),
            MIPMAP  = BIT(1),
            CLAMP = BIT(2)
            //FLIP = BIT(3)
        };

        const static unsigned int DEFAULT_FLAGS = TRANSPARENT | MIPMAP;

        GLuint id;

        Texture(unsigned int _id = 0):
            id(_id) {}

        Texture(std::string fn, unsigned int flags = DEFAULT_FLAGS)
            //MipmapOptions mip = MIPMAPPED,
            //WrapOptions wrap = REPEAT,
            //FlipOptions flip = NO_FLIP)
        {
            loadTex(fn, flags);
        }
        virtual ~Texture() { unload(); }

        unsigned int loadTex(std::string fn, unsigned int flags = DEFAULT_FLAGS);

        void unload();

        void bind(unsigned int layer = 0);

        bool good() { return (id!=0); }
        unsigned int getID() { return id; }

        unsigned reserveID() {
            unsigned int nid = id;
            id = 0;
            return nid;
        }

        static void unbind(/*unsigned int nlayers = 1*/);
};

#endif

