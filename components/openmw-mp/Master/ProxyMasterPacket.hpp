//
// Created by koncord on 22.04.17.
//

#ifndef OPENMW_PROXYMASTERPACKET_HPP
#define OPENMW_PROXYMASTERPACKET_HPP

#include <components/openmw-mp/Packets/BasePacket.hpp>
#include "MasterData.hpp"
#include <iostream>

namespace mwmp
{
    class ProxyMasterPacket : public BasePacket
    {
    private:
        explicit ProxyMasterPacket(RakNet::RakPeerInterface *peer) : BasePacket(peer)
        {
        }

    public:
        template<class Packet>
        static void addServer(Packet *packet, QueryData &server, bool send)
        {
            using namespace std;

            int32_t rulesSize = server.rules.size();
            packet->RW(rulesSize, send);

            if (rulesSize > 2000)
                rulesSize = 0;

            map<string, ServerRule>::iterator ruleIt;
            if (send)
                ruleIt = server.rules.begin();

            while (rulesSize--)
            {
                ServerRule *rule = nullptr;
                string key;
                if (send)
                {
                    key = ruleIt->first;
                    rule = &ruleIt->second;
                }

                packet->RW(key, send);
                if (!send)
                {
                    ruleIt = server.rules.insert(pair<string, ServerRule>(key, ServerRule())).first;
                    rule = &ruleIt->second;
                }

                packet->RW(rule->type, send);

                if (rule->type == 's')
                    packet->RW(rule->str, send);
                else
                    packet->RW(rule->val, send);

                if (send)
                    ruleIt++;
            }

            vector<string>::iterator plIt;

            if (send)
                plIt = server.players.begin();
            else
                server.players.clear();

            int32_t playersCount = server.players.size();
            packet->RW(playersCount, send);
            if (playersCount > 2000)
                playersCount = 0;

            while (playersCount--)
            {
                string player;
                if (send)
                    player = *plIt;

                packet->RW(player, send);

                if (!send)
                    server.players.push_back(player);
                else
                    plIt++;
            }

            int32_t pluginsCount = server.plugins.size();
            packet->RW(pluginsCount, send);

            if (pluginsCount > 2000)
                pluginsCount = 0;

            vector<Plugin>::iterator pluginIt;

            if (send)
                pluginIt = server.plugins.begin();
            else
                server.plugins.clear();

            while (pluginsCount--)
            {
                Plugin plugin;
                if (send)
                    plugin = *pluginIt;

                packet->RW(plugin.name, send);
                packet->RW(plugin.hash, send);
                if (!send)
                    server.plugins.push_back(plugin);
                else
                    pluginIt++;
            }
        }
    };
}

#endif //OPENMW_PROXYMASTERPACKET_HPP
