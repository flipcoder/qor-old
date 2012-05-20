#ifndef _AUDIO_H
#define _AUDIO_H

#include "Util.h"
#include <AL/al.h>
//#include <al/alc.h>
//#include <al/alu.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <AL/alut.h>
#include <memory>

#include "Log.h"

#include <iostream>

#include "math/common.h"

#pragma warning(disable:4996)

#define BUFFER_SIZE (4096 * 8)

class Audio
{
public:
    struct Buffer
    {
        unsigned int id;
        
        Buffer(){
            alGenBuffers(1, &id);
        }

        Buffer(std::string fn) {
            //id = 0;
            //ALenum format;
            //ALsizei size;
            //ALvoid* data;
            //ALsizei freq;
            //ALboolean loop;
            
            //alGenBuffers(1, &id);
            //if(alGetError() != AL_NO_ERROR) {
            //    id = 0;
            //    break;
            //}
            //alutLoadWAVFile((ALbyte*)fn.c_str(), &format, &data, &size, &freq, &loop);
            //alBufferData(id, format, data, size, freq);
            //alutUnloadWAV(format,data,size,freq);
            id = alutCreateBufferFromFile(fn.c_str());
        }
        virtual ~Buffer() {
            if(id)
                alDeleteBuffers(1, &id);
        }
        bool good() { return id!=0; }
    };

    struct Source
    {
        unsigned int id;
        float pitch;
        float gain;
        float rolloff;
        glm::vec3 pos;
        glm::vec3 vel;
        unsigned int buffer_id;
        enum eFlags {
            F_LOOP = BIT(0),
            F_AUTOPLAY = BIT(1)
        };
        unsigned int flags;
        Source(unsigned int _flags = 0) {
            alGenSources(1, &id);
            pitch = 1.0f;
            gain = 1.0f;
            buffer_id = 0;
            flags = _flags;
            if(flags & F_AUTOPLAY){
                refresh();
                play();
            }
        }
        virtual ~Source() {
            alDeleteSources(1, &id);
        }
        void bind(Buffer* buf) {
            if(buf)
                buffer_id = buf->id;
        }
        virtual void refresh() {
            alSourcei(id, AL_BUFFER, buffer_id);
            alSourcef(id, AL_PITCH, pitch);
            alSourcef(id, AL_GAIN, gain);
            alSourcefv(id, AL_POSITION, glm::value_ptr(pos));
            alSourcefv(id, AL_VELOCITY, glm::value_ptr(vel));
            //alSourcefv(id, AL_ROLLOFF_FACTOR, glm::value_ptr(rolloff));
            alSourcei(id, AL_LOOPING, (flags & F_LOOP) ? AL_TRUE : AL_FALSE);
        }
        virtual void play() {
            alSourcePlay(id);
        }
        bool playing() {
            ALenum state;
            alGetSourcei(id, AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }
        void pause() {
            alSourcePause(id);
        }
        void stop() { 
            alSourceStop(id);
        }

        bool good() { return id!=0; }
    };

    struct Stream : public Source
    {
        private:
            //FILE* m_File;
            OggVorbis_File m_Ogg;
            vorbis_info* m_VorbisInfo;
            vorbis_comment* m_VorbisComment;
            ALenum m_Format;
            ALuint m_Buffers[2];
            bool m_bOpen;

        public:

            Stream(std::string fn) {
                m_bOpen = false;
                do{

                    //m_File = fopen(fn.c_str(), "rb");
                    //if(!m_File)
                    //    break;
                    
                    int r;
                    if((r = ov_fopen((char*)&fn[0], &m_Ogg)) < 0)
                        break;

                    if(checkErrors())
                        break;

                    m_VorbisInfo = ov_info(&m_Ogg, -1);
                    m_VorbisComment = ov_comment(&m_Ogg, -1);
                    
                    if(checkErrors())
                        break;
                 
                    if(m_VorbisInfo->channels == 1)
                        m_Format = AL_FORMAT_MONO16;
                    else
                        m_Format = AL_FORMAT_STEREO16;
                    
                    alGenBuffers(2, m_Buffers);

                    if(checkErrors())
                        break;

                    flags |= Source::F_LOOP;

                    std::cout
                        << "version         " << m_VorbisInfo->version         << "\n"
                        << "channels        " << m_VorbisInfo->channels        << "\n"
                        << "rate (hz)       " << m_VorbisInfo->rate            << "\n"
                        << "bitrate upper   " << m_VorbisInfo->bitrate_upper   << "\n"
                        << "bitrate nominal " << m_VorbisInfo->bitrate_nominal << "\n"
                        << "bitrate lower   " << m_VorbisInfo->bitrate_lower   << "\n"
                        << "bitrate window  " << m_VorbisInfo->bitrate_window  << "\n"
                        << "\n"
                        << "vendor " << m_VorbisComment->vendor << "\n";
                        
                    for(int i = 0; i < m_VorbisComment->comments; i++)
                        std::cout << "   " << m_VorbisComment->user_comments[i] << "\n";
                        
                    std::cout << std::endl;
                                    

                    m_bOpen = true;
                } while(false);
            }

            bool update()
            {
                int processed;
                bool active = true;
             
                alGetSourcei(id, AL_BUFFERS_PROCESSED, &processed);
             
                while(processed--)
                {
                    ALuint buffer;
                    
                    alSourceUnqueueBuffers(id, 1, &buffer);
                    checkErrors();

                    active = stream(buffer);

                    alSourceQueueBuffers(id, 1, &buffer);
                    checkErrors();
                }
             
                return active;
            }

            void clear()
            {
                int queued;
                alGetSourcei(id, AL_BUFFERS_QUEUED, &queued);
                while(queued--)
                {
                    ALuint buffer;
                    alSourceUnqueueBuffers(id, 1, &buffer);
                    checkErrors();
                }
            }

            bool stream(unsigned int buffer)
            {
                char data[BUFFER_SIZE];
                int size = 0;
                int endian = 0;
                int section;
                int result;

                while(size < BUFFER_SIZE)
                {
                    result = ov_read(&m_Ogg, data + size, BUFFER_SIZE - size, endian, 2, 1, &section);
                    if(result > 0)
                        size += result;
                    else
                    {
                        if(result < 0)
                            return false;
                        else
                            break;
                    }
                }

                if(size == 0)
                    return false;

                alBufferData(buffer, m_Format, data, size, m_VorbisInfo->rate);
                return true;
            }

            virtual void refresh() {

                //if(playing())
                //{
                    update();

                    //alSourcei(id, AL_BUFFER, buffer_id);
                    alSourcef(id, AL_PITCH, pitch);
                    alSourcef(id, AL_GAIN, gain);
                    alSourcefv(id, AL_POSITION, glm::value_ptr(pos));
                    alSourcefv(id, AL_VELOCITY, glm::value_ptr(vel));
                    alSourcefv(id, AL_DIRECTION, glm::value_ptr(vel));
                    alSourcef(id, AL_ROLLOFF_FACTOR,  0.0);
                    alSourcei(id, AL_SOURCE_RELATIVE, AL_TRUE);
                    //alSourcei(id, AL_LOOPING, (flags & F_LOOP) ? AL_TRUE : AL_FALSE);
                //}
            }

            virtual void play() {
                if(playing())
                    return;
                if(!stream(m_Buffers[0]))
                    return;
                if(!stream(m_Buffers[1]))
                    return;
                    
                alSourceQueueBuffers(id, 2, m_Buffers);
                alSourcePlay(id);
            }

            bool good() { return m_bOpen; }

            bool checkErrors() {
                int error = alGetError();
                if(error != AL_NO_ERROR) {
                    std::pair<std::string, std::string> errpair = errorStringAL(error);
                    Log::get().write(std::string("OpenAL Error (") + errpair.first + "): " + errpair.second);
                    return true;
                }
                return false;
            }
            
            static std::pair<std::string, std::string> errorStringAL(int code)
            {
                switch(code)
                {
                    case AL_INVALID_NAME:
                        return {"AL_INVALID_NAME", "Invalid name."};
                    case AL_INVALID_ENUM:
                        return {"AL_INVALID_ENUM", "Invalid enum."};
                    case AL_INVALID_VALUE:
                        return {"AL_INVALID_VALUE", "Invalid value."};
                    case AL_INVALID_OPERATION:
                        return {"AL_INVALID_OPERATION", "Invalid operation."};
                    case AL_OUT_OF_MEMORY:
                        return {"AL_OUT_OF_MEMORY", "Out of memory."};
                }
                return (code!=AL_NO_ERROR) ?
                    std::pair<std::string,std::string>(std::string(""), std::string("No Error.")) :
                    std::pair<std::string,std::string>(str(code), std::string("Unknown Error Code"));
            }
            
            static std::pair<std::string,std::string> errorStringOV(int code)
            {
                switch(code)
                {
                    // libvorbis return codes http://www.xiph.org/vorbis/doc/libvorbis/return.html
                    case OV_EREAD:
                        return {"OC_EREAD","Read from media."};
                    case OV_ENOTVORBIS:
                        return {"OC_ENOTVORBIS","Not Vorbis data."};
                    case OV_EVERSION:
                        return {"OV_EVERSION", "Vorbis version mismatch."};
                    case OV_EBADHEADER:
                        return {"OV_EBADHEADER", "Invalid Vorbis header."};
                    case OV_EFAULT:
                        return {"OV_EFAULT", "Internal logic fault (bug or heap/stack corruption."};
                }
                return code ?
                    std::pair<std::string,std::string>( str(code), std::string("Unknown Error Code ") + str(code) ) :
                    std::pair<std::string,std::string>( "","No Error." );
            }

            virtual ~Stream() {
                stop();
                clear();
                alDeleteBuffers(2, m_Buffers);
                ov_clear(&m_Ogg);
            }
    };

    struct Listener
    {
        glm::vec3 pos, vel;
        glm::vec3 at;
        glm::vec3 up;
        Listener() {
            pos = glm::vec3(0.0f, 0.0f, 0.0f);
            vel = glm::vec3(0.0f, 0.0f, 0.0f);
            at = glm::vec3(0.0f, 0.0f, -1.0f);
            up = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        virtual ~Listener() {}
        void listen() {
            alListenerfv(AL_POSITION, glm::value_ptr(pos));
            alListenerfv(AL_VELOCITY, glm::value_ptr(vel));
            float ori[6];
            ori[0] = at.x; ori[1] = at.y; ori[2] = at.z;
            ori[3] = up.x; ori[4] = up.y; ori[5] = up.z;
            alListenerfv(AL_ORIENTATION, ori);
        }
    };

public:
    Audio(){
        alutInit(0, NULL);
        alGetError();
    }
    virtual ~Audio(){
        alutExit();
    }
    
    void listen(Listener* listener) {
        if(listener)
            listener->listen();
    }

    bool error() { return alGetError() != AL_NO_ERROR; }
};

#endif

