#ifndef OPENMW_COMPONENTS_LOGIC_LOGICCOMMON_H
#define OPENMW_COMPONENTS_LOGIC_LOGICCOMMON_H

#include <memory>

namespace MWLogic
{
    // Forward declarations

    class GameObject;
    class Component;
    class PropertySet;

    class KinematicBodyComponent;
    class RigidBodyComponent;

    class PhysicsPropertySet;
    class TransformPropertySet;


    // Pointer definitions

    typedef std::shared_ptr<GameObject> GameObjectPtr;
    typedef std::shared_ptr<Component> ComponentPtr;
    typedef std::shared_ptr<PropertySet> PropertySetPtr;

    typedef std::shared_ptr<KinematicBodyComponent> KinematicBodyComponentPtr;
    typedef std::shared_ptr<RigidBodyComponent> RigidBodyComponentPtr;

    typedef std::shared_ptr<PhysicsPropertySet> PhysicsPropertySetPtr;
    typedef std::shared_ptr<TransformPropertySet> TransformPropertySetPtr;

    // Temporary convenience functions. Maybe have static creation functions instead.

    template <typename T>
    std::shared_ptr<T> makePtr(T* ptr)
    {
        return std::shared_ptr<T>(ptr);
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> makePtr(Args... args)
    {
        return std::shared_ptr<T>(new T(args...));
    }
}

#endif
