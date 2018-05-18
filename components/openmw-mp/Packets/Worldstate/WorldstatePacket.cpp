#include <components/openmw-mp/NetworkMessages.hpp>
#include <PacketPriority.h>
#include <RakPeer.h>
#include "WorldstatePacket.hpp"

using namespace mwmp;

WorldstatePacket::WorldstatePacket(RakNet::RakPeerInterface *peer) : BasePacket(peer)
{
    packetID = 0;
    priority = HIGH_PRIORITY;
    reliability = RELIABLE_ORDERED;
    orderChannel = CHANNEL_WORLDSTATE;
    this->peer = peer;
}

void WorldstatePacket::setWorldstate(BaseWorldstate *worldstate)
{
    this->worldstate = worldstate;
    guid = worldstate->guid;
}

BaseWorldstate *WorldstatePacket::getWorldstate()
{
    return worldstate;
}
