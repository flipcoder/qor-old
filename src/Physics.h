#ifndef _PHYSICS_H
#define _PHYSICS_H

#include <memory>
#include <newton/Newton.h>
#include "Log.h"
#include "Util.h"
#include "Node.h"

class IPhysicsObject;
class Node;
//class Scene;

class Physics// : public std::enable_shared_from_this<Physics>
{
private:

	NewtonWorld* m_pWorld;
#ifndef _NEWTON_VISUAL_DEBUGGER
	void* m_pDebugger;
#endif
	bool m_bError;

public:

	Physics();
	virtual ~Physics();

	void nullify();
	void cleanup();

	/*! Physics Logic
	 * \param advance Ticks (in ms) to advanced simulation.
	 * \param root Root of physics node which can be automatically synced
	 */
	void logic(unsigned int advance, Node* root);

	enum GenerateFlags {
		GEN_RECURSIVE = BIT(0)
	};
	/*! Generate Physics
	 *  \param node Generates physics for a specific node.
	 *  \param flags GenerationFlags
	 *  \param matrix Current transformation matrix
	 */
	void generate(Node* node, unsigned int flags = 0, glm::mat4* transform = NULL);

	enum SyncFlags {
		SYNC_RECURSIVE = BIT(0)
	};
	/*! Sync Body
	 *  Syncronizes a single nodes' transformation matrix with the equivalent physics body.
	 *  \param node The node to be syncronized
	 *  \param flags Syncronization options (use SyncFlags)
	 *  \param matrix Current transformation matrix
	 */
	void syncBody(Node* node, unsigned int flags = 0);

	bool error() { return m_bError; }

	// IPhysicsObject param is required to also be of type Node*
	NewtonBody* addBody(NewtonCollision* nc, IPhysicsObject* pud, glm::mat4* transform);
	bool deleteBody(void* pud);

	enum {
		USER_FORCE = BIT(0),
		USER_OMEGA = BIT(1),
		USER_TORQUE = BIT(2),
		USER_VELOCITY = BIT(3)
	};
	static void cbForceTorque(const NewtonBody* body, float timestep, int threadIndex);
	static void cbTransform(const NewtonBody* body);
	
};

#endif

