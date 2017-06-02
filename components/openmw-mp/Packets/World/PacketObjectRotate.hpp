#ifndef OPENMW_PACKETOBJECTROTATE_HPP
#define OPENMW_PACKETOBJECTROTATE_HPP

#include <components/openmw-mp/Packets/World/WorldPacket.hpp>

namespace mwmp
{
    class PacketObjectRotate : public WorldPacket
    {
    public:
        PacketObjectRotate(RakNet::RakPeerInterface *peer);

        virtual void Object(WorldObject &worldObject, bool send);
    };
}

#endif //OPENMW_PACKETOBJECTROTATE_HPP
