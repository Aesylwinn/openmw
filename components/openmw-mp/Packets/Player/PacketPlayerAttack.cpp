//
// Created by koncord on 13.01.16.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerAttack.hpp"

using namespace mwmp;

PacketPlayerAttack::PacketPlayerAttack(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_ATTACK;
}

void PacketPlayerAttack::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);

    RW(player->attack.target.guid, send);

    RW(player->attack.spellId, send);
    RW(player->attack.type, send);
    RW(player->attack.success, send);
    RW(player->attack.damage, send);
    //
    RW(player->attack.pressed, send);
    RW(player->attack.knockdown, send);
    RW(player->attack.block, send);
}
