#ifndef _SOUND_H
#define _SOUND_H

#include <memory>
#include "Audio.h"
#include "Node.h"

class Sound : public Node
{
    protected:

        std::unique_ptr<Audio::Source> m_spSource;
        std::unique_ptr<Audio::Buffer> m_spBuffer;

    public:

        Sound(Audio::Buffer* buf, unsigned int flags = 0) {
            m_spBuffer.reset(buf);
            m_spSource.reset(new Audio::Source(flags));
            m_spSource->bind(m_spBuffer.get());
            m_spSource->refresh();
            m_spSource->play();
        }
        virtual ~Sound() {}

        virtual void logicSelf(unsigned int advance) {
            m_spSource->refresh();
        }
        virtual void renderSelf(Partitioner* partitioner, unsigned int flags = 0) const {}

        Audio::Source* source() { return m_spSource.get(); }
        Audio::Buffer* buffer() { return m_spBuffer.get(); }

        virtual std::string nodeTypeString() { return "sound"; }
        virtual SCOPED_ENUM_TYPE(NodeType) nodeType() { return NodeType::SOUND; }
};

#endif

