#include "propertysets.hpp"

namespace MWLogic
{
    PhysicsPropertySet::PhysicsPropertySet()
        : PropertySet(ClassName, ClassType)
        , mMass(0)
        , mInertiaX(0)
        , mInertiaY(0)
        , mInertiaZ(0)
        , mLinDamping(0)
        , mRotDamping(0)
        , mFriction(0.5)
        , mShape(nullptr)
    {
    }

    void PhysicsPropertySet::getMass(float& m) const
    {
        m = mMass;
    }

    void PhysicsPropertySet::setMass(float m)
    {
        mMass = m;

        signalChanged(ID_Mass);
    }

    void PhysicsPropertySet::getInertialDiagonals(float& x, float& y, float& z) const
    {
        x = mInertiaX;
        y = mInertiaY;
        z = mInertiaZ;
    }

    void PhysicsPropertySet::setInertialDiagonals(float x, float y, float z)
    {
        mInertiaX = x;
        mInertiaY = y;
        mInertiaZ = z;

        signalChanged(ID_InertialTensor);
    }

    void PhysicsPropertySet::getLinearDamping(float& d) const
    {
        d = mLinDamping;
    }

    void PhysicsPropertySet::setLinearDamping(float d)
    {
        mLinDamping = d;

        signalChanged(ID_LinearDamping);
    }

    void PhysicsPropertySet::getRotationalDamping(float& d) const
    {
        d = mRotDamping;
    }

    void PhysicsPropertySet::setRotationalDamping(float d)
    {
        mRotDamping = d;

        signalChanged(ID_RotationalDamping);
    }

    void PhysicsPropertySet::getFriction(float& f) const
    {
        f = mFriction;
    }

    void PhysicsPropertySet::setFriction(float f)
    {
        mFriction = f;

        signalChanged(ID_Friction);
    }

    void PhysicsPropertySet::getCollisionShape(CollisionShapePtr& shape) const
    {
        shape = mShape;
    }

    void PhysicsPropertySet::setCollisionShape(CollisionShapePtr shape)
    {
        mShape = shape;

        signalChanged(ID_CollisionShape);
    }


    TransformPropertySet::TransformPropertySet()
        : PropertySet(ClassName, ClassType)
        , mPosX(0)
        , mPosY(0)
        , mPosZ(0)
        , mRotW(1)
        , mRotX(0)
        , mRotY(0)
        , mRotZ(0)
        , mScaleX(0)
        , mScaleY(0)
        , mScaleZ(0)
        , mLinVelX(0)
        , mLinVelY(0)
        , mLinVelZ(0)
        , mRotVelX(0)
        , mRotVelY(0)
        , mRotVelZ(0)
    {
    }

    void TransformPropertySet::getPosition(float& x, float& y, float& z) const
    {
        x = mPosX;
        y = mPosY;
        z = mPosZ;
    }

    void TransformPropertySet::setPosition(float x, float y, float z)
    {
        mPosX = x;
        mPosY = y;
        mPosZ = z;

        signalChanged(ID_Position);
    }

    void TransformPropertySet::getRotation(float& w, float& x, float& y, float& z) const
    {
        w = mRotW;
        x = mRotX;
        y = mRotY;
        z = mRotZ;
    }

    void TransformPropertySet::setRotation(float w, float x, float y, float z)
    {
        mRotW = w;
        mRotX = x;
        mRotY = y;
        mRotZ = z;

        signalChanged(ID_Rotation);
    }

    void TransformPropertySet::getScale(float& x, float& y, float& z) const
    {
        x = mScaleX;
        y = mScaleY;
        z = mScaleZ;
    }

    void TransformPropertySet::setScale(float x, float y, float z)
    {
        mScaleX = x;
        mScaleY = y;
        mScaleZ = z;

        signalChanged(ID_Scale);
    }

    void TransformPropertySet::getLinearVelocity(float& vx, float& vy, float& vz) const
    {
        vx = mLinVelX;
        vy = mLinVelY;
        vz = mLinVelZ;
    }

    void TransformPropertySet::setLinearVelocity(float vx, float vy, float vz)
    {
        mLinVelX = vx;
        mLinVelY = vy;
        mLinVelZ = vz;

        signalChanged(ID_LinearVelocity);
    }

    void TransformPropertySet::getRotationalVelocity(float& rx, float& ry, float& rz) const
    {
        rx = mRotVelX;
        ry = mRotVelY;
        rz = mRotVelZ;
    }

    void TransformPropertySet::setRotationalVelocity(float rx, float ry, float rz)
    {
        mRotVelX = rx;
        mRotVelY = ry;
        mRotVelZ = rz;

        signalChanged(ID_RotationalVelocity);
    }
}
