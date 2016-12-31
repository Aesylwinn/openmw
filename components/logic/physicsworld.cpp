#include "physicsworld.hpp"

#include <btBulletDynamicsCommon.h>

namespace MWLogic
{
    PhysicsWorld::PhysicsWorld(float fixedTimeStep, int maxSubSteps)
        : mTimeStep(fixedTimeStep)
        , mMaxSubSteps(maxSubSteps)
        , mBroadphase(nullptr)
        , mCollisionConfig(nullptr)
        , mSolver(nullptr)
        , mDispatcher(nullptr)
        , mWorld(nullptr)
    {
        mBroadphase = new btDbvtBroadphase();
        mCollisionConfig = new btDefaultCollisionConfiguration();
        mSolver = new btSequentialImpulseConstraintSolver();
        mDispatcher = new btCollisionDispatcher(mCollisionConfig);

        mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
    }

    PhysicsWorld::~PhysicsWorld()
    {
        delete mWorld;
        delete mDispatcher;
        delete mSolver;
        delete mCollisionConfig;
        delete mBroadphase;
    }

    void PhysicsWorld::setGravity(float x, float y, float z)
    {
        mWorld->setGravity(btVector3(x, y, z));
    }

    void PhysicsWorld::addKinematicBody(GameObjectPtr obj)
    {
        KinematicBodyComponentPtr component = obj->findComponent<KinematicBodyComponent>(0);
        if (component)
        {
            mKinematicBodyComponents.push_front(component);
        }
    }

    void PhysicsWorld::removeKinematicBody(GameObjectPtr obj)
    {
        KinematicBodyComponentPtr component = obj->findComponent<KinematicBodyComponent>(0);
        if (component)
        {
            mKinematicBodyComponents.remove(component);
        }
    }

    void PhysicsWorld::addRigidBody(GameObjectPtr obj)
    {
        RigidBodyComponentPtr component = obj->findComponent<RigidBodyComponent>(0);
        if (component)
        {
            mRigidBodyComponents.push_front(component);
            mWorld->addRigidBody(component->getBody());
        }
    }

    void PhysicsWorld::removeRigidBody(GameObjectPtr obj)
    {
        RigidBodyComponentPtr component = obj->findComponent<RigidBodyComponent>(0);
        if (component)
        {
            mWorld->removeRigidBody(component->getBody());
            mRigidBodyComponents.remove(component);
        }
    }

    void PhysicsWorld::stepSimulation(float time)
    {
        mWorld->stepSimulation(time, mMaxSubSteps, mTimeStep);
    }

    void PhysicsWorld::preSimulationSynchronize()
    {
        for (auto iter = mRigidBodyComponents.begin(); iter != mRigidBodyComponents.end(); ++iter)
        {
            (*iter)->readProperties();
        }
    }

    void PhysicsWorld::postSimulationSynchronize()
    {
        for (auto iter = mRigidBodyComponents.begin(); iter != mRigidBodyComponents.end(); ++iter)
        {
            (*iter)->writeProperties();
        }
    }
}
