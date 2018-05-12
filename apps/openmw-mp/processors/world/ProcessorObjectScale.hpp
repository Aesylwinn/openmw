#ifndef OPENMW_PROCESSOROBJECTSCALE_HPP
#define OPENMW_PROCESSOROBJECTSCALE_HPP

#include "../WorldProcessor.hpp"

namespace mwmp
{
    class ProcessorObjectScale : public WorldProcessor
    {
    public:
        ProcessorObjectScale()
        {
            BPP_INIT(ID_OBJECT_SCALE)
        }

        void Do(ObjectPacket &packet, Player &player, BaseEvent &event) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player.npc.mName.c_str());
            
            packet.Send(true);

            Script::Call<Script::CallbackIdentity("OnObjectScale")>(player.getId(), event.cell.getDescription().c_str());
        }
    };
}

#endif //OPENMW_PROCESSOROBJECTSCALE_HPP
