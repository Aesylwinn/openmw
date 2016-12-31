#include "physicscomponents.hpp"

#include <btBulletDynamicsCommon.h>

namespace MWLogic
{
    RigidBodyComponent::RigidBodyComponent()
        : Component(ClassName, ClassType)
        , mBody(nullptr)
        , mPhysicsProperties(nullptr)
        , mTransformProperties(nullptr)
        , mPropertiesDirty(false)
        , mTransformDirty(false)
    {
        mBody = new btRigidBody(0, nullptr, nullptr);
    }

    RigidBodyComponent::~RigidBodyComponent()
    {
        delete mBody;
    }

    btRigidBody* RigidBodyComponent::getBody()
    {
        return mBody;
    }

    void RigidBodyComponent::readProperties()
    {
        if (mPropertiesDirty)
        {
            float a, b, x, y, z;
            CollisionShapePtr shape;

            mPhysicsProperties->getMass(a);
            mPhysicsProperties->getInertialDiagonals(x, y, z);
            mBody->setMassProps(a, btVector3(x, y, z));

            mPhysicsProperties->getLinearDamping(a);
            mPhysicsProperties->getRotationalDamping(b);
            mBody->setDamping(a, b);

            mPhysicsProperties->getFriction(a);
            mBody->setFriction(a);

            mPhysicsProperties->getCollisionShape(shape);
            mBody->setCollisionShape(shape.get());

            mPropertiesDirty = false;
        }

        if (mTransformDirty)
        {
            float a, b, c, d, x, y, z;

            mTransformProperties->getPosition(x, y, z);
            mTransformProperties->getRotation(a, b, c, d);
            mBody->setWorldTransform(btTransform(btQuaternion(b, c, d, a), btVector3(x, y, z)));

            mTransformProperties->getLinearVelocity(x, y, z);
            mBody->setLinearVelocity(btVector3(x, y, z));

            mTransformProperties->getRotationalVelocity(x, y, z);
            mBody->setAngularVelocity(btVector3(x, y, z));

            mTransformDirty = false;
        }
    }

    void RigidBodyComponent::writeProperties()
    {
        btVector3 vec;
        btQuaternion quat;

        vec = mBody->getWorldTransform().getOrigin();
        mTransformProperties->setPosition(vec.x(), vec.y(), vec.z());

        quat = mBody->getWorldTransform().getRotation();
        mTransformProperties->setRotation(quat.w(), quat.x(), quat.y(), quat.z());

        vec = mBody->getLinearVelocity();
        mTransformProperties->setLinearVelocity(vec.x(), vec.y(), vec.z());

        vec = mBody->getAngularVelocity();
        mTransformProperties->setRotationalVelocity(vec.x(), vec.y(), vec.z());
    }

    void RigidBodyComponent::onAttach()
    {
        mPhysicsProperties = getParent()->findPropertySet<PhysicsPropertySet>();
        mTransformProperties = getParent()->findPropertySet<TransformPropertySet>();

        mPropertiesDirty = true;
        mTransformDirty = true;
        readProperties();
    }

    void RigidBodyComponent::onDetach()
    {
        mPhysicsProperties = nullptr;
        mTransformProperties = nullptr;
    }

    void RigidBodyComponent::onPropertyChanged(int propertyType, int identifier)
    {
        switch (propertyType)
        {
            case PST_Physics:
                mPropertiesDirty = true;
                break;

            case PST_Transform:
                mTransformDirty = true;
                break;

            default: break;
        }
    }
}
