#ifndef OPENMW_PROCESSORVIDEOPLAY_HPP
#define OPENMW_PROCESSORVIDEOPLAY_HPP

#include "../WorldProcessor.hpp"

namespace mwmp
{
    class ProcessorVideoPlay final: public WorldProcessor
    {
    public:
        ProcessorVideoPlay()
        {
            BPP_INIT(ID_VIDEO_PLAY)
        }

        virtual void Do(ObjectPacket &packet, WorldEvent &event)
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "Received %s", strPacketID.c_str());
            event.playVideo();
        }
    };
}

#endif //OPENMW_PROCESSORVIDEOPLAY_HPP
