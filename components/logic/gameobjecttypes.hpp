#ifndef OPENMW_COMPONENTS_LOGIC_GAMEOBJECTTYPES_H
#define OPENMW_COMPONENTS_LOGIC_GAMEOBJECTTYPES_H

namespace MWLogic
{
    enum PropertySetTypes
    {
        PST_None,

        PST_Physics,
        PST_Transform
    };

    enum ComponentTypes
    {
        CT_None,

        CT_KinematicBody,
        CT_RigidBody
    };
}

#endif
