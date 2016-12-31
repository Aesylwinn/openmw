#include <components/client/client.hpp>
#include <components/client/clientoptions.hpp>
#include <components/misc/log.hpp>
#include <components/misc/osglog.hpp>

//
#include <components/logic/gameobject.hpp>
#include <components/logic/physicscomponents.hpp>
#include <components/logic/physicsworld.hpp>
#include <components/logic/propertysets.hpp>

#include <btBulletDynamicsCommon.h>

#include <sstream>
//

#include <fstream>

int main(int argc, char* argv[])
{
    std::ofstream logFile("client.log");
    Misc::Log::create(Misc::Log::LV_DebugInfo, logFile);
    Misc::OSGLog::setup();

    MWClient::ClientOptions clientOptions;
    MWClient::Client client;

    client.init(clientOptions);

    while (!client.shouldShutdown())
    {
        client.update();
    }

    client.cleanup();

    //

    {
        using namespace MWLogic;

        CollisionShapePtr sphereShape(new btSphereShape(1.));

        PhysicsPropertySetPtr physicsProperties = makePtr<PhysicsPropertySet>();
        physicsProperties->setMass(100);
        physicsProperties->setCollisionShape(sphereShape);

        TransformPropertySetPtr transformProperties = makePtr<TransformPropertySet>();
        transformProperties->setPosition(0, 100, 0);

        RigidBodyComponentPtr rigidBodyComponent = makePtr<RigidBodyComponent>();

        GameObjectPtr obj = makePtr<GameObject>();

        obj->attach(physicsProperties);
        obj->attach(transformProperties);

        obj->attach(rigidBodyComponent);



        PhysicsWorld physicsWorld(1/30.0, 2);
        physicsWorld.setGravity(0, -10, 0);

        physicsWorld.addRigidBody(obj);



        for (int i = 0; i < 30; ++i)
        {
            physicsWorld.preSimulationSynchronize();
            physicsWorld.stepSimulation(1/30.0);
            physicsWorld.postSimulationSynchronize();

            float x, y, z;
            transformProperties->getPosition(x, y, z);

            std::stringstream ss;
            ss << "Position: " << x << ", " << y << ", " << z;

            Misc::Log::get().writeStatusInfo("Physics Object", ss.str());
        }

        physicsWorld.removeRigidBody(obj);
    }
    //

    Misc::OSGLog::setEnabled(false);
    Misc::Log::destroy();

    return 0;
}
