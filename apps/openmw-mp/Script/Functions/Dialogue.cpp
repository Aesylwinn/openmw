#include "Dialogue.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>

#include <apps/openmw-mp/Script/ScriptFunctions.hpp>
#include <apps/openmw-mp/Networking.hpp>

using namespace mwmp;

void DialogueFunctions::InitializeTopicChanges(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->topicChanges.topics.clear();
}

unsigned int DialogueFunctions::GetTopicChangesSize(unsigned short pid) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, 0);

    return player->topicChanges.count;
}

void DialogueFunctions::AddTopic(unsigned short pid, const char* topicId) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    mwmp::Topic topic;
    topic.topicId = topicId;

    player->topicChanges.topics.push_back(topic);
}

const char *DialogueFunctions::GetTopicId(unsigned short pid, unsigned int i) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, "");

    if (i >= player->topicChanges.count)
        return "invalid";

    return player->topicChanges.topics.at(i).topicId.c_str();
}

void DialogueFunctions::SendTopicChanges(unsigned short pid, bool toOthers) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_TOPIC)->setPlayer(player);
    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_TOPIC)->Send(toOthers);
}

void DialogueFunctions::PlayAnimation(unsigned short pid, const char* groupname, int mode, int count, bool persist) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->animation.groupname = groupname;
    player->animation.mode = mode;
    player->animation.count = count;
    player->animation.persist = persist;

    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_ANIM_PLAY)->setPlayer(player);
    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_ANIM_PLAY)->Send(false);
    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_ANIM_PLAY)->Send(true);
}

void DialogueFunctions::PlaySpeech(unsigned short pid, const char* sound) noexcept
{
    Player *player;
    GET_PLAYER(pid, player, );

    player->sound = sound;

    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPEECH)->setPlayer(player);
    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPEECH)->Send(false);
    mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_SPEECH)->Send(true);
}
