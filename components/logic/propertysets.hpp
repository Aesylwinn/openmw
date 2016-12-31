#ifndef OPENMW_COMPONENTS_LOGIC_PROPERTYSETS_H
#define OPENMW_COMPONENTS_LOGIC_PROPERTYSETS_H

#include "gameobject.hpp"
#include "gameobjecttypes.hpp"

#include <memory>

class btCollisionShape;
typedef std::shared_ptr<btCollisionShape> CollisionShapePtr;

namespace MWLogic
{
    class PhysicsPropertySet : public PropertySet
    {
        public:

            MW_PROPERTYSET_META("PhysicsPropertySet", PST_Physics)

            enum Identifiers
            {
                ID_Mass,
                ID_InertialTensor,
                ID_LinearDamping,
                ID_RotationalDamping,
                ID_Friction,
                ID_CollisionShape
            };

            PhysicsPropertySet();

            /// Linear inertia
            void getMass(float& m) const;
            void setMass(float m);

            /// Rotational inertia. TODO: explain format
            void getInertialDiagonals(float& x, float &y, float &z) const;
            void setInertialDiagonals(float x, float y, float z);

            void getLinearDamping(float& d) const;
            void setLinearDamping(float d);

            void getRotationalDamping(float& d) const;
            void setRotationalDamping(float d);

            void getFriction(float& f) const;
            void setFriction(float f);

            void getCollisionShape(CollisionShapePtr& shape) const;
            void setCollisionShape(CollisionShapePtr shape);

        private:

            float mMass;

            float mInertiaX, mInertiaY, mInertiaZ;

            float mLinDamping;
            float mRotDamping;

            float mFriction;

            CollisionShapePtr mShape;
    };

    class TransformPropertySet : public PropertySet
    {
        public:

            MW_PROPERTYSET_META("TransformPropertySet", PST_Transform)

            enum Identifiers
            {
                ID_Position,
                ID_Rotation,
                ID_Scale,
                ID_LinearVelocity,
                ID_RotationalVelocity
            };

            TransformPropertySet();

            void getPosition(float& x, float& y, float& z) const;
            void setPosition(float x, float y, float z);

            void getRotation(float& w, float& x, float& y, float& z) const;
            void setRotation(float w, float x, float y, float z);

            void getScale(float& x, float& y, float& z) const;
            void setScale(float x, float y, float z);

            void getLinearVelocity(float& vx, float& vy, float& vz) const;
            void setLinearVelocity(float vx, float vy, float vz);

            void getRotationalVelocity(float& rx, float& ry, float& rz) const;
            void setRotationalVelocity(float rx, float ry, float rz);

        private:

            float mPosX, mPosY, mPosZ;
            float mRotW, mRotX, mRotY, mRotZ;
            float mScaleX, mScaleY, mScaleZ;

            float mLinVelX, mLinVelY, mLinVelZ;
            float mRotVelX, mRotVelY, mRotVelZ;
    };
}

#endif
