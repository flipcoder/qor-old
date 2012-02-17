#ifndef _TRACKERNODE_H
#define _TRACKERNODE_H

#include <memory>
#include "Node.h"

class TrackerNode : public Node
{
    public:

        enum FocusFlag {
            FOCUS_NONE, // no focus
            FOCUS_ATTACH, // snaps to the position of the focus
            FOCUS_WATCH, // turn towards the focus (unimp)
            FOCUS_ORIENT, // mimics focus orientation
            FOCUS_COPY // copies focus orientation and position
        };

    protected:

        std::weak_ptr<Node> m_wpFocus;
        mutable bool m_bNeedsUpdate;

        FocusFlag m_FocusFlag;

    public:
        TrackerNode();
        TrackerNode(std::weak_ptr<Node> nfocus, FocusFlag ff);
        virtual glm::mat4* matrix();
        void update();
        void focus(std::weak_ptr<Node> nfocus, FocusFlag ff);
};

#endif

