#ifndef OPENMW_COMPONENTS_LOGIC_PHYSICSWORLD_H
#define OPENMW_COMPONENTS_LOGIC_PHYSICSWORLD_H

#include "gameobject.hpp"
#include "physicscomponents.hpp"

#include <list>
#include <memory>

class btBroadphaseInterface;
class btCollisionConfiguration;
class btConstraintSolver;
class btDiscreteDynamicsWorld;
class btDispatcher;

namespace MWLogic
{
    class PhysicsWorld
    {
        public:

            /// /param maxSubSteps determines when the simulation will slow down if it cannot keep up.
            PhysicsWorld(float fixedTimeStep, int maxSubSteps);
            ~PhysicsWorld();

            void setGravity(float x, float y, float z);

            void addKinematicBody(GameObjectPtr obj);
            void removeKinematicBody(GameObjectPtr obj);

            void addRigidBody(GameObjectPtr obj);
            void removeRigidBody(GameObjectPtr obj);

            /// Advances the simulation by the given time in seconds.
            void stepSimulation(float time);

            /// Writes GameObject changes to PhysicsWorld state.
            void preSimulationSynchronize();
            /// Writes simulation changes to GameObjects.
            void postSimulationSynchronize();

        private:

            float mTimeStep;
            int mMaxSubSteps;

            btBroadphaseInterface* mBroadphase;
            btCollisionConfiguration* mCollisionConfig;
            btConstraintSolver* mSolver;
            btDispatcher* mDispatcher;

            btDiscreteDynamicsWorld* mWorld;

            std::list<KinematicBodyComponentPtr> mKinematicBodyComponents;
            std::list<RigidBodyComponentPtr> mRigidBodyComponents;
    };
}

#endif
