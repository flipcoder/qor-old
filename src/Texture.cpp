#include "Texture.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <string>
#include "Renderer.h"

unsigned int Texture :: loadTex(std::string fn,
    //TransOptions trans,
    MipmapOptions mip,
    WrapOptions wrap,
    FlipOptions flip)
{
	//return 1; // bypass textures


    ILuint tempImage;
    GLuint solutionTexture;

    ilGenImages(1,&tempImage);
    glGenTextures(1,&solutionTexture);

    ilBindImage(tempImage);
    if(!ilLoadImage((char*)fn.c_str())){
		id = 0;
		return 0;
	}

    //if(!flip)
    //    iluFlipImage();
	//if(!trans)
	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();
		
	//ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	//wglPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPushAttrib(GL_ENABLE_BIT);
    glBindTexture(GL_TEXTURE_2D,solutionTexture);

    //if(mip)
    //{
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST_MIPMAP_LINEAR);
    //}
    //else
    //{
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    //}
	
	float filter;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, filter);
	

    if(wrap==REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    }

    if(mip){

		// GLU version:
        // gluBuild2DMipmaps(GL_TEXTURE_2D,ilGetInteger(IL_IMAGE_BPP),ilGetInteger(IL_IMAGE_WIDTH),
            //ilGetInteger(IL_IMAGE_HEIGHT),ilGetInteger(IL_IMAGE_FORMAT),GL_UNSIGNED_BYTE,ilGetData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

		glTexImage2D(GL_TEXTURE_2D,0,ilGetInteger(IL_IMAGE_BPP),ilGetInteger(IL_IMAGE_WIDTH),
           ilGetInteger(IL_IMAGE_HEIGHT),0,ilGetInteger(IL_IMAGE_FORMAT),GL_UNSIGNED_BYTE,ilGetData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{

        glTexImage2D(GL_TEXTURE_2D,0,ilGetInteger(IL_IMAGE_BPP),ilGetInteger(IL_IMAGE_WIDTH),
            ilGetInteger(IL_IMAGE_HEIGHT),0,ilGetInteger(IL_IMAGE_FORMAT),GL_UNSIGNED_BYTE,ilGetData());
	}

    ilDeleteImages(1,&tempImage);

    id = solutionTexture;

    return id;
}


void Texture :: unload()
{
    if(id)
    {
        glDeleteTextures(1,&id);
        id = 0;
    }
}
void Texture :: bind(unsigned int layer)
{
	//glActiveTexture(GL_TEXTURE0 + layer);
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,id);
	Renderer::get().bindTexture(this, layer);
}
void Texture :: unbind()
{
	Renderer::get().unbindTextures();
	//for(unsigned int i=0; i < nlayers; ++i) {
	//    glActiveTexture(GL_TEXTURE0 + i);
	//    glBindTexture(GL_TEXTURE_2D, 0);
	//    glDisable(GL_TEXTURE_2D);
	//}
	//glActiveTexture(GL_TEXTURE0);
}

