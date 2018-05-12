#ifndef OPENMW_PROCESSOROBJECTANIMPLAY_HPP
#define OPENMW_PROCESSOROBJECTANIMPLAY_HPP

#include "BaseObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorObjectAnimPlay : public BaseObjectProcessor
    {
    public:
        ProcessorObjectAnimPlay()
        {
            BPP_INIT(ID_OBJECT_ANIM_PLAY)
        }

        virtual void Do(ObjectPacket &packet, WorldEvent &event)
        {
            BaseObjectProcessor::Do(packet, event);

            event.animateObjects(ptrCellStore);
        }
    };
}

#endif //OPENMW_PROCESSOROBJECTANIMPLAY_HPP
