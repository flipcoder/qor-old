#include "Physics.h"
#include <glm/glm.hpp>
//#include "SceneNode.h"
#include "IPhysicsObject.h"
#include "IMeshContainer.h"
#include "NodeAttributes.h"
#include "math/common.h"

#include <iostream>
#include <memory>
using namespace std;

Physics::Physics()
{
    nullify();

    
}

Physics::~Physics()
{
    cleanup();
}

void Physics :: nullify()
{
    
}

void Physics :: cleanup()
{
    
    nullify();
}

void Physics :: logic(unsigned int advance)
{
    static float accum = 0.0f;
    const float fixed_step = 1/60.0f;
    float timestep = advance*0.001f; // msec to sec

    accum+=timestep;
    if(accum >= fixed_step)
    {
        //m_pWorld->stepSimulation(fixed_step, NUM_SUBSTEPS);
        //NewtonUpdate(m_pWorld, fixed_step);
//        //syncBody(root, SYNC_RECURSIVE);
//#ifdef _NEWTON_VISUAL_DEBUGGER
        //NewtonDebuggerServe(m_pDebugger, m_pWorld);
//#endif
        accum -= fixed_step;
    }
}

void Physics :: generate(Node* node, unsigned int flags, std::unique_ptr<glm::mat4> transform)
{
    if(!node)
        return;

    // generate identity matrix if transform was passed in as null
    // could use an auto_ptr in these situations but meh

    // TODO: If no transform is given, derive world space transform from node
    if(!transform)
        transform.reset(new glm::mat4());

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
            switch(object->getPhysicsType())
            {
                case IPhysicsObject::STATIC:
                    generateTree(node, flags, transform.get());
                    break;
                case IPhysicsObject::ACTOR:
                    generateActor(node, flags, transform.get());
                    break;
                case IPhysicsObject::DYNAMIC:
                    generateDynamic(node, flags, transform.get());
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }

    // generate children
    if(node->hasAttribute(NodeAttributes::CHILDREN) && (flags & GEN_RECURSIVE))
    {
        std::list<Node*>* children = static_cast<std::list<Node*>*>(node->getAttribute(NodeAttributes::CHILDREN));
        foreach(Node* child, (*children))
        {
            // copy current node's transform so it can be modified by child
            std::unique_ptr<glm::mat4> transform_copy(new glm::mat4(*transform));
            generate(child, flags, std::move(transform_copy));
        }
    }
    
    // delete generated identity matrix for those who passed in null matrix pointers
    //if(created_transform)
    //    delete transform;
}

void Physics :: generateActor(Node* node, unsigned int flags, glm::mat4* transform) {
    assert(node);
    assert(transform);

    IPhysicsObject* physics_object = dynamic_cast<IPhysicsObject*>(node);
    //Actor* actor = dynamic_cast<Actor*>(node);
    
    // TODO: generate code
}

void Physics :: generateTree(Node* node, unsigned int flags, glm::mat4* transform) {
    assert(node);
    assert(transform);

    // TODO: btBvhTriangleMeshShape or btMultiMaterialTriangleMeshShape

    // [Assumption]  Node contains static geometry therefore it is atleast a IMeshContainer...
    
    std::list<shared_ptr<Mesh>>* mesh_list = dynamic_cast<IMeshContainer*>(node)->getMeshes();
    if(mesh_list->empty())
        return;
    IPhysicsObject* physics_object = dynamic_cast<IPhysicsObject*>(node);
    
    // TODO: generate code    
}

void Physics :: generateDynamic(Node* node, unsigned int flags, glm::mat4* transform) {
    assert(node);
    assert(transform);

    std::list<shared_ptr<Mesh>>* mesh_list = dynamic_cast<IMeshContainer*>(node)->getMeshes();
    if(mesh_list->empty())
        return;
    IPhysicsObject* physics_object = dynamic_cast<IPhysicsObject*>(node);

    // TODO: generate code
}


// syncBody gets the data out of physics subsystem, reports it back to each node
//void Physics :: syncBody(Node* node, unsigned int flags)
//{
//    if(!node)
//        return;

//    // In here, we must read physics instructions from node, and depending on what type it is,
//    // we must deal with it differently
//    if(node->hasAttribute(NodeAttributes::PHYSICS)&&
//        ((dynamic_cast<IPhysicsObject*>(node))->getPhysicsType() != IPhysicsObject::STATIC))
//    {
//        IPhysicsObject* po = dynamic_cast<IPhysicsObject*>(node);
//        glm::mat4 body_matrix;
//        NewtonBodyGetMatrix((NewtonBody*)po->getPhysicsBody(), glm::value_ptr(body_matrix));
//        po->sync(&body_matrix);

//        // NOTE: Remember to update the transform from the object side afterwards.
//    }

//    // sync children
//    if(node->hasAttribute(NodeAttributes::CHILDREN)  && (flags & SYNC_RECURSIVE))
//    {
//        auto children = (std::list<shared_ptr<Node>>*)node->getAttribute(NodeAttributes::CHILDREN);
//        for(auto itr = children->begin();
//            itr != children->end();
//            ++itr)
//        {
//            syncBody(itr->get(), flags);
//        }
//        //foreach(Node* child, *children)
//        //    syncBody(child, flags);
//    }
//}

//btRigidBody* Physics :: addBody(btCollisionObject* obj, IPhysicsObject* pud, glm::mat4* transform, btVector3* inertia)
//{
//    //float mass = pud->mass();

//    //glm::vec3 inertia, origin;
//    //NewtonBody* body = NewtonCreateBody(m_pWorld, nc, glm::value_ptr(*transform));
//    //NewtonBodySetUserData(body, pud);
    
//    //btTransform btt;
//    //btt.setFromOpenGLMatrix(glm::value_ptr(transform));
//    //btRigidBody* body = new bt

//    //pud->setPhysicsBody(this, (void*)body, (void*)motion);
//    //m_pWorld->addRigidBody(body);
    
//    //if(mass > EPSILON)
//    //{
//    //    NewtonConvexCollisionCalculateInertialMatrix(nc, glm::value_ptr(inertia), glm::value_ptr(origin));
//    //    NewtonBodySetMassMatrix(body, mass, inertia.x * mass, inertia.y * mass, inertia.z * mass);
//    //    NewtonBodySetCentreOfMass(body, glm::value_ptr(origin));
//    //    NewtonBodySetForceAndTorqueCallback(body, (NewtonApplyForceAndTorque)&cbForceTorque);
//    //    NewtonBodySetTransformCallback(body, (NewtonSetTransform)&cbTransform);
//    //}

//    return null;
//}

//bool Physics :: deleteBody(btRigidBody* obj)
//{
//    if(!obj)
//        return false;
//    //m_pWorld->removeCollisionObject(obj);
//    //delete obj;

//    NewtonDestroyBody(m_pWorld, (NewtonBody*)obj);
//    return true;
//}

//void Physics :: cbForceTorque(const NewtonBody* body, float timestep, int threadIndex)
//{
//    float mass, ix, iy, iz;
//    NewtonBodyGetMassMatrix(body, &mass, &ix, &iy, &iz);
//    glm::vec3 force(0.0f, mass * -9.8f, 0.0f);
//    glm::vec3 omega(0.0f);
//    glm::vec3 velocity(0.0f);
//    glm::vec3 torque(0.0f);
//    NewtonBodyGetVelocity(body, glm::value_ptr(velocity));

//    IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
//    unsigned int userflags = pobj->physicsLogic(timestep, mass, force, omega, torque, velocity);

//    if(userflags & USER_FORCE)
//        NewtonBodyAddForce(body, glm::value_ptr(force));
//    if(userflags & USER_OMEGA)
//        NewtonBodySetOmega(body, glm::value_ptr(omega));
//    if(userflags & USER_TORQUE)
//        NewtonBodySetTorque(body, glm::value_ptr(torque));
//    if(userflags & USER_VELOCITY)
//        NewtonBodySetVelocity(body, glm::value_ptr(velocity));
//}

//void Physics :: cbTransform(const NewtonBody* body)
//{
//    IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
    
//    float marray[16];
//    NewtonBodyGetMatrix(body, &marray[0]);
    
//    // Note: All physics nodes should be collapsed (node transform == world transform)
//    //  so leaving this in world space is fine for now, unless in the future a better constraint system
//    //  is integrated
//    glm::mat4 m = Matrix::fromArray(marray);
//    //m.clear(glm::mat4::TRANSPOSE, m);
//    //node->sync(&m);
//    pobj->sync(&m);
//}
