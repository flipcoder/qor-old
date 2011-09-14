#include "Physics.h"
#include <glm/glm.hpp>
//#include "SceneNode.h"
#include "IMeshContainer.h"
#include "IPhysicsObject.h"
#include "NodeAttributes.h"
#include "math/common.h"

#include <iostream>
#include <memory>
using namespace std;

Physics::Physics()
{
	nullify();

	m_pWorld = NewtonCreate();
	NewtonSetPlatformArchitecture(m_pWorld, 1);
	
	glm::vec3 min(-250.0), max(250.0); //temp world sizes
	NewtonSetWorldSize(m_pWorld, &min.x, &max.x);

	// If using debugger, debugger would be initilized here
#ifdef _NEWTON_VISUAL_DEBUGGER
	m_pDebugger = NewtonDebuggerCreateServer();
#endif
	
	NewtonSetSolverModel(m_pWorld, 0);

	int default_mat_id = NewtonMaterialGetDefaultGroupID(m_pWorld);
	NewtonMaterialSetDefaultElasticity(m_pWorld, default_mat_id, default_mat_id, 0.0f);
	NewtonMaterialSetDefaultSoftness(m_pWorld, default_mat_id, default_mat_id, 0.0f);
	NewtonMaterialSetDefaultFriction(m_pWorld, default_mat_id, default_mat_id, 0.0f, 0.0f);
}

Physics::~Physics()
{
	cleanup();
}

void Physics :: nullify()
{
	m_pWorld = NULL;
	m_bError = false;
}

void Physics :: cleanup()
{
#ifdef _NEWTON_VISUAL_DEBUGGER
	NewtonDebuggerDestroyServer(m_pDebugger);
#endif
	NewtonDestroyAllBodies(m_pWorld);
	NewtonDestroy(m_pWorld);

	nullify();
}


void Physics :: logic(unsigned int advance, Node* root)
{
	static float accum = 0.0f;
	float timestep = advance*0.001f; // msec to sec

	if(accum + timestep <= 1.0f/120.0f)
	   accum+=timestep;
	else
	{
		NewtonUpdate(m_pWorld, accum + timestep);
		//syncBody(root, SYNC_RECURSIVE);
#ifdef _NEWTON_VISUAL_DEBUGGER
		NewtonDebuggerServe(m_pDebugger, m_pWorld);
#endif
		accum = 0.0f;
	}
}

void Physics :: generate(Node* node, unsigned int flags, glm::mat4* transform)
{
	if(!node)
		return;

	// generate identity matrix if transform was passed in as null
	// could use an auto_ptr in these situations but meh
	bool created_transform = false;
	if(!transform)
	{
		// TODO: If no transform is given, derive world space transform from node
		transform = new glm::mat4();
		created_transform = true; // keep track so we can delete
	}

	// apply transformation of node so the mesh vertices are correct
	*transform *= *node->matrix_c();
	//assert(transform->isIdentity());
	
	// Are there physics instructions?
	if(node->hasAttribute(NodeAttributes::PHYSICS))
	{
		IPhysicsObject* object = dynamic_cast<IPhysicsObject*>(node);

		if(object->getPhysicsBody() == NULL)
		{
			// Check if there's static geometry in this node, if so let's process it
			if(object->getPhysicsType() == IPhysicsObject::STATIC)
			{
				// Collision Tree

				// [Assumption]  Node contains static geometry therefore it is atleast a IMeshContainer...
				std::list<shared_ptr<Mesh>>* mesh_list = dynamic_cast<IMeshContainer*>(node)->getMeshes();
				if(!mesh_list->empty())
				{
					// right now, all meshes in a node are combined into one collision tree
					NewtonCollision* collision = NewtonCreateTreeCollision(m_pWorld, 0);
					NewtonTreeCollisionBeginBuild(collision);

					unsigned int face_id = 1;
					//foreach(Mesh* m, *mesh_list)
					for(auto itr = mesh_list->begin();
						itr != mesh_list->end();
						++itr)
					{
						Mesh* m = itr->get();

						// loop through current node's meshes
						for(unsigned int i=0; i<m->faces.size(); i++)
						{
							glm::vec3 verts[3];

							for(unsigned int f = 0; f<3; f++)
							{
								verts[f] = m->vertices[m->faces[i].indices[f]];
								//verts[f] *= *transform;
								//verts[f] = Matrix::mult(verts[f], glm::inverse(*transform));
							}
							NewtonTreeCollisionAddFace(collision, 3, glm::value_ptr(verts[0]), sizeof(glm::vec3), 0/* i+1 */);
							face_id++;
						}
					}

					NewtonTreeCollisionEndBuild(collision, 0);
					glm::mat4 identity;
					//addBody(collision, object, &identity); //&identity
					addBody(collision, object, transform);
					NewtonReleaseCollision(m_pWorld, collision);
				}
			}
			else if(object->getPhysicsType() == IPhysicsObject::ACTOR)
			{
				// Actor Controller
				glm::mat4 body_matrix = glm::rotate(glm::mat4(), 90.0f, Axis::Z);//(glm::mat4::ROTATE_Z, 90.0f);
				Matrix::translation(body_matrix, glm::vec3(0.0f, -object->height()/4.0f, 0.0f));
				NewtonCollision* collision = NewtonCreateCapsule(m_pWorld, object->radius(), object->height(), 1, glm::value_ptr(body_matrix));
				NewtonBody* body = addBody(collision, object, transform);

				NewtonConstraintCreateUpVector(m_pWorld, glm::value_ptr(Axis::Y), body);
				NewtonReleaseCollision(m_pWorld, collision);
			}
			else if(object->getPhysicsType() == IPhysicsObject::DYNAMIC)
			{
				// Standard dynamic physics object (entity)
				std::list<shared_ptr<Mesh>>* mesh_list = dynamic_cast<IMeshContainer*>(node)->getMeshes();
				if(!mesh_list->empty())
				{
					Mesh* m = mesh_list->front().get();
					const float tolerance = 0.002f;
					glm::mat4 identity;
					NewtonCollision* collision = NewtonCreateConvexHull(m_pWorld, m->vertices.size(),
						glm::value_ptr(m->vertices[0]), sizeof(glm::vec3), tolerance, 0, glm::value_ptr(identity));/*glm::value_ptr(*transform));*/
					addBody(collision, object, transform); // &identity
					NewtonReleaseCollision(m_pWorld, collision);
				}
			}
		}
	}

	// generate children
	if(node->hasAttribute(NodeAttributes::CHILDREN)  && (flags & GEN_RECURSIVE))
	{
		std::list<Node*>* children = static_cast<std::list<Node*>*>(node->getAttribute(NodeAttributes::CHILDREN));
		foreach(Node* child, (*children))
		{
			// copy current node's transform so it can be modified by child
			glm::mat4* transform_copy = new glm::mat4(*transform);
			generate(child, flags, transform_copy);
			delete transform_copy;
		}
	}
	
	// delete generated identity matrix for those who passed in null matrix pointers
	if(created_transform)
		delete transform;
}

// syncBody gets the data out of physics subsystem, reports it back to each node
void Physics :: syncBody(Node* node, unsigned int flags)
{
	if(!node)
		return;

	// In here, we must read physics instructions from node, and depending on what type it is,
	// we must deal with it differently
	if(node->hasAttribute(NodeAttributes::PHYSICS)&&
		((dynamic_cast<IPhysicsObject*>(node))->getPhysicsType() != IPhysicsObject::STATIC))
	{
		IPhysicsObject* po = dynamic_cast<IPhysicsObject*>(node);
		glm::mat4 body_matrix;
		NewtonBodyGetMatrix((NewtonBody*)po->getPhysicsBody(), glm::value_ptr(body_matrix));
		po->sync(&body_matrix);

		// NOTE: Remember to update the transform from the object side afterwards.
	}

	// sync children
	if(node->hasAttribute(NodeAttributes::CHILDREN)  && (flags & SYNC_RECURSIVE))
	{
		auto children = (std::list<shared_ptr<Node>>*)node->getAttribute(NodeAttributes::CHILDREN);
		for(auto itr = children->begin();
			itr != children->end();
			++itr)
		{
			syncBody(itr->get(), flags);
		}
		//foreach(Node* child, *children)
		//    syncBody(child, flags);
	}
}

NewtonBody* Physics :: addBody(NewtonCollision* nc, IPhysicsObject* pud, glm::mat4* transform)
{
	float mass = pud->mass();

	glm::vec3 inertia, origin;
	NewtonBody* body = NewtonCreateBody(m_pWorld, nc, glm::value_ptr(*transform));
	NewtonBodySetUserData(body, pud);

	//shared_ptr<Physics> shared_this(shared_from_this());
	//weak_ptr<Physics> weak_this(shared_this);
	pud->setPhysicsBody(this, (void*)body);
	
	if(mass > EPSILON)
	{
		NewtonConvexCollisionCalculateInertialMatrix(nc, glm::value_ptr(inertia), glm::value_ptr(origin));
		NewtonBodySetMassMatrix(body, mass, inertia.x * mass, inertia.y * mass, inertia.z * mass);
		NewtonBodySetCentreOfMass(body, glm::value_ptr(origin));
		NewtonBodySetForceAndTorqueCallback(body, (NewtonApplyForceAndTorque)&cbForceTorque);
		NewtonBodySetTransformCallback(body, (NewtonSetTransform)&cbTransform);
	}

	return body;
}

bool Physics :: deleteBody(void* pud)
{
	if(!pud)
		return false;
	NewtonDestroyBody(m_pWorld, (NewtonBody*)pud);
	return true;
}

void Physics :: cbForceTorque(const NewtonBody* body, float timestep, int threadIndex)
{
	float mass, ix, iy, iz;
	NewtonBodyGetMassMatrix(body, &mass, &ix, &iy, &iz);
	glm::vec3 force(0.0f, mass * -9.8f, 0.0f);
	glm::vec3 omega(0.0f);
	glm::vec3 velocity(0.0f);
	glm::vec3 torque(0.0f);
	NewtonBodyGetVelocity(body, glm::value_ptr(velocity));

	IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
	unsigned int userflags = pobj->physicsLogic(timestep, mass, force, omega, torque, velocity);

	if(userflags & USER_FORCE)
		NewtonBodyAddForce(body, glm::value_ptr(force));
	if(userflags & USER_OMEGA)
		NewtonBodySetOmega(body, glm::value_ptr(omega));
	if(userflags & USER_TORQUE)
		NewtonBodySetTorque(body, glm::value_ptr(torque));
	if(userflags & USER_VELOCITY)
		NewtonBodySetVelocity(body, glm::value_ptr(velocity));
}

void Physics :: cbTransform(const NewtonBody* body)
{
	IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
	
	float marray[16];
	NewtonBodyGetMatrix(body, &marray[0]);
	// TODO: World Space :(
	glm::mat4 m = Matrix::fromArray(marray);
	//m.clear(glm::mat4::TRANSPOSE, m);
	//node->sync(&m);
	
	pobj->sync(&m);
}

