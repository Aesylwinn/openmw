#ifndef OPENMW_PACKETPLAYERTOPIC_HPP
#define OPENMW_PACKETPLAYERTOPIC_HPP

#include <components/openmw-mp/Packets/Player/PlayerPacket.hpp>

namespace mwmp
{
    class PacketPlayerTopic final: public PlayerPacket
    {
    public:
        PacketPlayerTopic(RakNet::RakPeerInterface *peer);

        void Packet(RakNet::BitStream *bs, bool send) override;
    };
}

#endif //OPENMW_PACKETPLAYERTOPIC_HPP
