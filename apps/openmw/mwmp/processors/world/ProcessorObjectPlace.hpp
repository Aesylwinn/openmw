#ifndef OPENMW_PROCESSOROBJECTPLACE_HPP
#define OPENMW_PROCESSOROBJECTPLACE_HPP

#include "BaseObjectProcessor.hpp"

namespace mwmp
{
    class ProcessorObjectPlace : public BaseObjectProcessor
    {
    public:
        ProcessorObjectPlace()
        {
            BPP_INIT(ID_OBJECT_PLACE)
        }

        virtual void Do(ObjectPacket &packet, WorldEvent &event)
        {
            BaseObjectProcessor::Do(packet, event);

            event.placeObjects(ptrCellStore);
        }

    };
}

#endif //OPENMW_PROCESSOROBJECTPLACE_HPP
