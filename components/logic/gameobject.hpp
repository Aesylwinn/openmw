#ifndef OPENMW_COMPONENTS_LOGIC_GAMEOBJECT_H
#define OPENMW_COMPONENTS_LOGIC_GAMEOBJECT_H

#include "logiccommon.hpp"

#include <memory>
#include <vector>

#define MW_COMPONENT_META(name, type)               \
static constexpr const char* ClassName = name;      \
static constexpr int ClassType = type;

#define MW_PROPERTYSET_META(name, type)             \
static constexpr const char* ClassName = name;      \
static constexpr int ClassType = type;

namespace MWLogic
{
    /// A container for potentially interacting components. Thread safety requires external synchronization.
    class GameObject
    {
        public:

            virtual ~GameObject();

            void attach(ComponentPtr component);
            void detach(ComponentPtr component);
            template <typename Derived>
            std::shared_ptr<Derived> findComponent(int offset);

            /// Once attached, the PropertySet is a permament feature. It cannot be removed.
            void attach(PropertySetPtr set);
            template <typename Derived>
            std::shared_ptr<Derived> findPropertySet();

            /// Propogates the signal to any listening components
            void signalPropertyChanged(int propertyType, int identifier);

        private:

            ComponentPtr findComponent(int type, int offset);
            PropertySetPtr findPropertySet(int type);

            std::vector<ComponentPtr> mComponents;
            std::vector<PropertySetPtr> mProperties;
    };

    /// A class that is used to update or read shared properties.
    class Component
    {
        public:

            Component(const char* className, int classType);
            virtual ~Component();

            const char* getClassName() const;
            int getClassType() const;

            GameObject* getParent() const;

        protected:

            /// Called when a parent is gained
            virtual void onAttach() = 0;
            /// Called when a parent is lost
            virtual void onDetach() = 0;

            /// Called when a parent object property is changed. Ideally, this only sets a flag.
            virtual void onPropertyChanged(int propertyType, int identifier) = 0;

        private:

            /// Must only be called by the GameObject class
            void setParent(GameObject* parent);

            const char* mName;
            int mType;

            GameObject* mParent;

            friend class GameObject;
    };

    /// A class that is used to set or retrieve shared properties. An instance of this must only be attached to a
    /// single GameObject.
    class PropertySet
    {
        public:

            PropertySet(const char* className, int classType);
            virtual ~PropertySet();

            const char* getClassName() const;
            int getClassType() const;

        protected:

            /// Used to alert when a set has changed
            void signalChanged(int identifier);

        private:

            /// Should only be called by the GameObject class
            void setParent(GameObject* parent);

            const char* mName;
            int mType;

            GameObject* mParent;

            friend class GameObject;
    };



    template <typename Derived>
    std::shared_ptr<Derived> GameObject::findComponent(int offset)
    {
        return std::static_pointer_cast<Derived>(findComponent(Derived::ClassType, offset));
    }

    template <typename Derived>
    std::shared_ptr<Derived> GameObject::findPropertySet()
    {
        return std::static_pointer_cast<Derived>(findPropertySet(Derived::ClassType));
    }
}

#endif
