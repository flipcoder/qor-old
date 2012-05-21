#ifndef _INDICATOR_H
#define _INDICATOR_H

#include <memory>
#include "Node.h"
#include "Texture.h"

class Indicator : public Node
{
    public:
        virtual ~Indicator() {}
        void setIndicator(std::shared_ptr<Texture>& tex) {
            m_spTexture = tex;
        }
        void clearIndicator() {
            m_spTexture.reset();
        }

        virtual void renderSelf(IPartitioner* partitioner, unsigned int flags) const;
        virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::LIGHT; }
        virtual std::string nodeTypeString() const { return "light"; }

        virtual const AABB* box(unsigned int flags = 0) const { return NULL; }
        virtual AABB* box(unsigned int flags = 0) { return NULL; }

    private:
        std::shared_ptr<Texture> m_spTexture;
};

#endif

