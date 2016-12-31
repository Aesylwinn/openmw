#ifndef OPENMW_COMPONENTS_LOGIC_PHYSICSCOMPONENTS_H
#define OPENMW_COMPONENTS_LOGIC_PHYSICSCOMPONENTS_H

#include "gameobject.hpp"
#include "gameobjecttypes.hpp"
#include "propertysets.hpp"

class btRigidBody;

namespace MWLogic
{
    class KinematicBodyComponent : public Component
    {
        public:

            MW_COMPONENT_META("KinematicBodyComponent", CT_KinematicBody)

            KinematicBodyComponent();
            ~KinematicBodyComponent();

        protected:

            virtual void onAttach();
            virtual void onDetach();

            virtual void onPropertyChanged(int type);
    };

    /// Dynamic bodies need a mass greater than 0 and a collision shape.
    class RigidBodyComponent : public Component
    {
        public:

            MW_COMPONENT_META("RigidBody", CT_RigidBody)

            RigidBodyComponent();
            ~RigidBodyComponent();

            btRigidBody* getBody();

            void readProperties();
            void writeProperties();

        protected:

            virtual void onAttach();
            virtual void onDetach();

            virtual void onPropertyChanged(int propertyType, int identifier);

        private:

            btRigidBody* mBody;

            PhysicsPropertySetPtr mPhysicsProperties;
            TransformPropertySetPtr mTransformProperties;

            bool mPropertiesDirty;
            bool mTransformDirty;
    };
}

#endif
