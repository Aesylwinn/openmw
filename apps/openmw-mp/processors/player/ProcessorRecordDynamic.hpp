#ifndef OPENMW_PROCESSORPLAYERRECORDDYNAMIC_HPP
#define OPENMW_PROCESSORPLAYERRECORDDYNAMIC_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerRecordDynamic final : public PlayerProcessor
    {
    public:
        ProcessorPlayerRecordDynamic()
        {
            BPP_INIT(ID_RECORD_DYNAMIC)
        }

        void Do(PlayerPacket &packet, const std::shared_ptr<Player> &player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            Script::Call<Script::CallbackIdentity("OnRecordDynamic")>(player.getId());
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERRECORDDYNAMIC_HPP
