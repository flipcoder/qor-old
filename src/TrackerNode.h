
/**
 * @file	TrackerNode.h
 * @author	Grady O'Connell
 * @url		http://www.prolificaxis.com
 *
 * @section LICENSE
 *
 * For internal use only. Do not redistribute.
 * © 2011 Prolific Axis, LLC
 *
 * @section DESCRIPTION
 *
 * Tracker Node
 * A scene graph node, with the ability to copy other nodes' transformations.
 * A tracker node uses a defined "focus".  What the node copies from the focus
 * is defined by the a FocusFlag.  It can: attach(snap) to the position of, take
 * on the orientation of, face towards, or copy transformations exactly.
 *
 * NOTES: Uses weak_ptr to protect against potential problems with focus destruction
 *
 * FIX: Right now this only thinks in orientational-object and positional-
 * parent space.  Better spacial transformation needs to be added to the Node class.
 */

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

