#include "gameobject.hpp"

#include <algorithm>

namespace MWLogic
{
    GameObject::~GameObject()
    {
        for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
        {
            (*iter)->setParent(nullptr);
        }

        mComponents.clear();
        mProperties.clear();
    }

    void GameObject::attach(ComponentPtr component)
    {
        mComponents.push_back(component);
        component->setParent(this);
    }

    void GameObject::detach(ComponentPtr component)
    {
        component->setParent(nullptr);
        std::remove(mComponents.begin(), mComponents.end(), component);
    }

    void GameObject::attach(PropertySetPtr set)
    {
        mProperties.push_back(set);
    }

    void GameObject::signalPropertyChanged(int propertyType, int identifier)
    {
        for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
        {
            (*iter)->onPropertyChanged(propertyType, identifier);
        }
    }

    ComponentPtr GameObject::findComponent(int type, int offset)
    {
        int count = 0;

        for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter)
        {
            if ((*iter)->getClassType() == type)
            {
                if (count == offset)
                    return *iter;
                else
                    ++count;
            }
        }

        return nullptr;
    }

    PropertySetPtr GameObject::findPropertySet(int type)
    {
        for (auto iter = mProperties.begin(); iter != mProperties.end(); ++ iter)
        {
            if ((*iter)->getClassType() == type)
                return *iter;
        }

        return nullptr;
    }

    Component::Component(const char* className, int classType)
        : mName(className)
        , mType(classType)
        , mParent(nullptr)
    {
    }

    Component::~Component()
    {
    }

    const char* Component::getClassName() const
    {
        return mName;
    }

    int Component::getClassType() const
    {
        return mType;
    }

    GameObject* Component::getParent() const
    {
        return mParent;
    }

    void Component::setParent(GameObject* parent)
    {
        if (mParent)
            onDetach();

        mParent = parent;

        if (mParent)
            onAttach();
    }

    PropertySet::PropertySet(const char* className, int classType)
        : mName(className)
        , mType(classType)
        , mParent(nullptr)
    {
    }

    PropertySet::~PropertySet()
    {
    }

    const char* PropertySet::getClassName() const
    {
        return mName;
    }

    int PropertySet::getClassType() const
    {
        return mType;
    }

    void PropertySet::signalChanged(int identifier)
    {
        if (mParent)
            mParent->signalPropertyChanged(mType, identifier);
    }

    void PropertySet::setParent(GameObject* parent)
    {
        mParent = parent;
    }
}
