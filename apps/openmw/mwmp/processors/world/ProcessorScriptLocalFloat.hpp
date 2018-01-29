#ifndef OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP
#define OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP

#include "BaseObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorScriptLocalFloat final: public BaseObjectProcessor
    {
    public:
        ProcessorScriptLocalFloat()
        {
            BPP_INIT(ID_SCRIPT_LOCAL_FLOAT)
        }

        virtual void Do(WorldPacket &packet, WorldEvent &event)
        {
            BaseObjectProcessor::Do(packet, event);

            event.setLocalFloats(ptrCellStore);
        }
    };
}

#endif //OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP
