#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "GfxAPI.h"
#include <string>

class Texture
{
	public:

		enum TransOptions{
			NO_TRANS=0,
			TRANS
		};

		enum MipmapOptions{
			SINGLE_IMAGE=0,
			MIPMAPPED
		};

		enum WrapOptions {
			REPEAT = 0,
			CLAMP
		};

		enum FlipOptions {
			NO_FLIP = 0,
			FLIPPED
		};

		GLuint id;

		Texture(unsigned int _id = 0):
			id(_id) {}

		Texture(std::string fn,
			MipmapOptions mip = SINGLE_IMAGE,
			WrapOptions wrap = REPEAT,
			FlipOptions flip = NO_FLIP)
		{
			loadTex(fn,mip,wrap,flip);
		}
		virtual ~Texture() { unload(); }

		unsigned int loadTex(std::string fn,
			MipmapOptions mip = SINGLE_IMAGE,
			WrapOptions wrap = REPEAT,
			FlipOptions flip = NO_FLIP);

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

