#ifndef OPENMW_MPCELL_HPP
#define OPENMW_MPCELL_HPP

#include "ActorList.hpp"
#include "LocalActor.hpp"
#include "DedicatedActor.hpp"
#include "../mwworld/cellstore.hpp"

namespace mwmp
{
    class Cell
    {
    public:

        explicit Cell(MWWorld::CellStore* cellStore);
        ~Cell() = default;

        void updateLocal(bool forceUpdate);
        void updateDedicated(float dt);

        void readPositions(ActorList& actorList);
        void readAnimFlags(ActorList& actorList);
        void readAnimPlay(ActorList& actorList);
        void readStatsDynamic(ActorList& actorList);
        void readEquipment(ActorList& actorList);
        void readSpeech(ActorList& actorList);
        void readAI(ActorList& actorList);
        void readAttack(ActorList& actorList);
        void readCellChange(ActorList& actorList);

        void initializeLocalActor(const MWWorld::Ptr& ptr);
        void initializeLocalActors();

        void initializeDedicatedActor(const MWWorld::Ptr& ptr);
        void initializeDedicatedActors(ActorList& actorList);

        void uninitializeLocalActors();
        void uninitializeDedicatedActors();

        LocalActor *getLocalActor(const std::string &actorIndex);
        DedicatedActor *getDedicatedActor(const std::string &actorIndex);

        bool hasLocalAuthority();
        void setAuthority(const RakNet::RakNetGUID& guid);

        MWWorld::CellStore* getCellStore();
        std::string getDescription();

        bool shouldInitializeActors;

    private:
        MWWorld::CellStore* store;
        RakNet::RakNetGUID authorityGuid;

        std::map<std::string, LocalActor *> localActors;
        std::map<std::string, DedicatedActor *> dedicatedActors;

        float updateTimer;
    };
}

#endif //OPENMW_MPCELL_HPP
