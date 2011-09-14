#ifndef _SELECTORNODE_H
#define _SELECTORNODE_H

// A Selector Node is a visual bounding box applied to objects that are selected in the editor

#include "../Node.h"

class SelectorNode : public Node
{
public:
	SelectorNode() {}
	virtual ~SelectorNode() {}

	virtual void logicSelf(unsigned int advance);
	virtual void renderSelf(Frustum* view = NULL, unsigned int flags = 0) const;
};

#endif

