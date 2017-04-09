//
// Created by koncord on 29.04.16.
//

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>
#include <components/openmw-mp/NetworkMessages.hpp>

void ScriptFunctions::SendMessage(unsigned short pid, const char *message, bool broadcast) noexcept
{
    Player *player;
    GET_PLAYER(pid, player,);

    player->chatMessage = message;

    LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "System: %s", message);

    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE)->setPlayer(player);

    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE)->Send(false);
    if (broadcast)
        mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE)->Send(true);
}

void ScriptFunctions::CleanChat(unsigned short pid)
{

}

void ScriptFunctions::CleanChat()
{

}
