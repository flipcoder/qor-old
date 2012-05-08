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
    m_spCollisionConfig.reset(new btDefaultCollisionConfiguration());
    m_spDispatcher.reset(new btCollisionDispatcher(m_spCollisionConfig.get()));
    //m_spBroadphase.reset(new btDbvtBroadphase());
    btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
    m_spBroadphase.reset(new btAxisSweep3(worldMin, worldMax));
    m_spSolver.reset(new btSequentialImpulseConstraintSolver());
    m_spWorld.reset(new btDiscreteDynamicsWorld(
        m_spDispatcher.get(),
        m_spBroadphase.get(),
        m_spSolver.get(),
        m_spCollisionConfig.get()
    ));
    m_spWorld->setGravity(btVector3(0.0, -9.8, 0.0));

//    m_pWorld = NewtonCreate();
//    NewtonSetPlatformArchitecture(m_pWorld, 1);
    
//    glm::vec3 min(-250.0), max(250.0); //temp world sizes
//    NewtonSetWorldSize(m_pWorld, &min.x, &max.x);

//    // If using debugger, debugger would be initilized here
//#ifdef _NEWTON_VISUAL_DEBUGGER
//    m_pDebugger = NewtonDebuggerCreateServer();
//#endif
    
//    NewtonSetSolverModel(m_pWorld, 0);

//    int default_mat_id = NewtonMaterialGetDefaultGroupID(m_pWorld);
//    NewtonMaterialSetDefaultElasticity(m_pWorld, default_mat_id, default_mat_id, 0.0f);
//    NewtonMaterialSetDefaultSoftness(m_pWorld, default_mat_id, default_mat_id, 0.0f);
//    NewtonMaterialSetDefaultFriction(m_pWorld, default_mat_id, default_mat_id, 0.0f, 0.0f);
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
//#ifdef _NEWTON_VISUAL_DEBUGGER
//    NewtonDebuggerDestroyServer(m_pDebugger);
//#endif
//    NewtonDestroyAllBodies(m_pWorld);
//    NewtonDestroy(m_pWorld);
}

void Physics :: logic(unsigned int advance)
{
    static float accum = 0.0f;
    float timestep = advance*0.001f; // msec to sec

    //if(accum + timestep < 1.0f/120.0f)
    //    accum+=timestep;
    //else
    //{
        m_spWorld->stepSimulation(/*accum + */timestep, NUM_SUBSTEPS);
//        NewtonUpdate(m_pWorld, accum + timestep);
//        //syncBody(root, SYNC_RECURSIVE);
//#ifdef _NEWTON_VISUAL_DEBUGGER
//        NewtonDebuggerServe(m_pDebugger, m_pWorld);
//#endif
    //    accum = 0.0f;
    //}
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

    btTransform bt_transform = Physics::toBulletTransform(*transform);
    //bt_transform.setIdentity();
    //bt_transform.setOrigin(btVector3(0.0, 0.0, 0.0));
    std::unique_ptr<btPairCachingGhostObject> ghost(new btPairCachingGhostObject()); // ->object
    ghost->setWorldTransform(bt_transform);
    std::unique_ptr<btGhostPairCallback> ghost_callback(new btGhostPairCallback()); // -
    m_spBroadphase->getOverlappingPairCache()->setInternalGhostPairCallback(ghost_callback.get());
    std::unique_ptr<btCollisionShape> shape(new btCapsuleShape(physics_object->radius(), physics_object->height())); //-
    ghost->setCollisionShape(shape.get());
    ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
    btScalar step_height(0.35);
    std::unique_ptr<btKinematicCharacterController> character( // -> interface
        new btKinematicCharacterController(
            ghost.get(),
            ((btConvexShape*)shape.get()),
            step_height
        )
    );

    //character->setUpAxis(1);

    physics_object->addCollisionShape(shape);
    physics_object->setGhostPairCallback(ghost_callback);
    std::unique_ptr<btCollisionObject> body(std::move(ghost));

    m_spWorld->addCollisionObject(
        body.get(),
        btBroadphaseProxy::CharacterFilter,
        btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter
    );

    std::unique_ptr<btActionInterface> interface(std::move(character));
    m_spWorld->addAction(interface.get());
    m_spBroadphase->getOverlappingPairCache()->cleanProxyFromPairs(
        body->getBroadphaseHandle(),
        m_spDispatcher.get()
    );
    ((btKinematicCharacterController*)interface.get())->reset();
    ((btKinematicCharacterController*)interface.get())->warp(Physics::toBulletVector(node->position()));

    physics_object->setBody(body);
    physics_object->setAction(interface);
    physics_object->setPhysics(this);

    // Without character controller:
    
    //IPhysicsObject* physics_object = dynamic_cast<IPhysicsObject*>(node);
    //std::unique_ptr<btCollisionShape> shape(new btCapsuleShape(physics_object->radius(), physics_object->height()));
    //btVector3 inertia(0,0,0);
    //shape->calculateLocalInertia(physics_object->mass(), inertia);
    //btRigidBody::btRigidBodyConstructionInfo info(
    //    physics_object->mass(),
    //    physics_object, // inherits btMotionState
    //    shape.get(),
    //    inertia
    //);
    //std::unique_ptr<btCollisionObject> body(new btRigidBody(info));
    //((btRigidBody*)body.get())->setAngularFactor(0.0);
    //((btRigidBody*)body.get())->setSleepingThresholds(0.0, 0.0);
    
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
    
    //TODO: move this ptr into an object that actually persists
    std::unique_ptr<btTriangleMesh> triangles(new btTriangleMesh());

    unsigned int face_id = 1;
    //    //foreach(Mesh* m, *mesh_list)
    // loop through current node's meshes
    for(auto itr = mesh_list->begin();
        itr != mesh_list->end();
        ++itr)
    {
        Mesh* m = itr->get();

        // for each face in inside mesh, add it to triangles list
        for(unsigned int i=0; i<m->faces.size(); i++)
        {
            glm::vec3 verts[3];
            for(unsigned int f = 0; f<3; f++)
                verts[f] = m->vertices[m->faces[i].indices[f]];
            triangles->addTriangle(
                toBulletVector(verts[0]),
                toBulletVector(verts[1]),
                toBulletVector(verts[2])
            );
            face_id++;
        }
    }

    //TODO: move this ptr into an object that actually persists
    // use triangles list to build rigidbody info for bullet
    std::unique_ptr<btCollisionShape> shape(new btBvhTriangleMeshShape(triangles.get(),true,true));
    btRigidBody::btRigidBodyConstructionInfo info(
        0.0, // no mass
        physics_object, // inherits btMotionState
        shape.get()
    );
    std::unique_ptr<btCollisionObject> body(new btRigidBody(info));
    std::unique_ptr<btStridingMeshInterface> interface(std::move(triangles));
    physics_object->addStridingMeshInterface(interface);
    physics_object->addCollisionShape(shape);
    physics_object->setBody(body);
    physics_object->setPhysics(this);
    m_spWorld->addRigidBody((btRigidBody*)physics_object->getBody());
}

void Physics :: generateDynamic(Node* node, unsigned int flags, glm::mat4* transform) {
    assert(node);
    assert(transform);

    // TODO: btConvexHull

    //std::list<shared_ptr<Mesh>>* mesh_list = dynamic_cast<IMeshContainer*>(node)->getMeshes();
    //if(mesh_list->empty())
    //    return;
    //IPhysicsObject* physics_object = dynamic_cast<IPhysicsObject*>(node);

    //std::unique_ptr<btTriangleMesh> triangles(new btTriangleMesh());

    //unsigned int face_id = 1;
    //for(auto itr = mesh_list->begin();
    //    itr != mesh_list->end();
    //    ++itr)
    //{
    //    Mesh* m = itr->get();
    //    // for each face in inside mesh, add it to triangles list
    //    for(unsigned int i=0; i<m->faces.size(); i++)
    //    {
    //        glm::vec3 verts[3];
    //        for(unsigned int f = 0; f<3; f++)
    //            verts[f] = m->vertices[m->faces[i].indices[f]];
    //        triangles->addTriangle(
    //            toBulletVector(verts[0]),
    //            toBulletVector(verts[1]),
    //            toBulletVector(verts[2])
    //        );
    //        face_id++;
    //    }
    //    std::unique_ptr<btCollisionObject> body(new btConvexHull());
    //}

    
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
//    //m_spWorld->addRigidBody(body);
    
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

bool Physics :: deleteBody(btRigidBody* obj)
{
    if(!obj)
        return false;
    m_spWorld->removeCollisionObject(obj);
    delete obj;
    //NewtonDestroyBody(m_pWorld, (NewtonBody*)obj);
    return true;
}

//void Physics :: cbForceTorque(const NewtonBody* body, float timestep, int threadIndex)
//{
    //float mass, ix, iy, iz;
    //NewtonBodyGetMassMatrix(body, &mass, &ix, &iy, &iz);
    //glm::vec3 force(0.0f, mass * -9.8f, 0.0f);
    //glm::vec3 omega(0.0f);
    //glm::vec3 velocity(0.0f);
    //glm::vec3 torque(0.0f);
    //NewtonBodyGetVelocity(body, glm::value_ptr(velocity));

    //IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
    //unsigned int userflags = pobj->physicsLogic(timestep, mass, force, omega, torque, velocity);

    //if(userflags & USER_FORCE)
    //    NewtonBodyAddForce(body, glm::value_ptr(force));
    //if(userflags & USER_OMEGA)
    //    NewtonBodySetOmega(body, glm::value_ptr(omega));
    //if(userflags & USER_TORQUE)
    //    NewtonBodySetTorque(body, glm::value_ptr(torque));
    //if(userflags & USER_VELOCITY)
    //    NewtonBodySetVelocity(body, glm::value_ptr(velocity));
//}

//void Physics :: cbTransform(const NewtonBody* body)
//{
//    IPhysicsObject* pobj = (IPhysicsObject*)NewtonBodyGetUserData(body);
    
//    float marray[16];
//    NewtonBodyGetMatrix(body, &marray[0]);
//    // TODO: World Space :(
//    glm::mat4 m = Matrix::fromArray(marray);
//    //m.clear(glm::mat4::TRANSPOSE, m);
//    //node->sync(&m);
    
//    pobj->sync(&m);
//}
