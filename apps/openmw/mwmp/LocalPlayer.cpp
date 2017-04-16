//
// Created by koncord on 14.01.16.
//

#include <components/misc/rng.hpp>
#include <components/esm/esmwriter.hpp>
#include <components/openmw-mp/Log.hpp>

#include "../mwbase/environment.hpp"
#include "../mwbase/journal.hpp"

#include "../mwclass/creature.hpp"
#include "../mwclass/npc.hpp"

#include "../mwdialogue/dialoguemanagerimp.hpp"

#include "../mwgui/inventorywindow.hpp"
#include "../mwgui/windowmanagerimp.hpp"

#include "../mwinput/inputmanagerimp.hpp"

#include "../mwmechanics/aitravel.hpp"
#include "../mwmechanics/creaturestats.hpp"
#include "../mwmechanics/mechanicsmanagerimp.hpp"
#include "../mwmechanics/spellcasting.hpp"

#include "../mwscript/scriptmanagerimp.hpp"

#include "../mwstate/statemanagerimp.hpp"

#include "../mwworld/cellstore.hpp"
#include "../mwworld/customdata.hpp"
#include "../mwworld/inventorystore.hpp"
#include "../mwworld/manualref.hpp"
#include "../mwworld/player.hpp"
#include "../mwworld/worldimp.hpp"

#include "LocalPlayer.hpp"
#include "Networking.hpp"
#include "CellController.hpp"
#include "Main.hpp"

using namespace mwmp;
using namespace std;

LocalPlayer::LocalPlayer()
{
    charGenStage.current = 0;
    charGenStage.end = 1;
    consoleAllowed = true;
    ignorePosPacket = false;
}

LocalPlayer::~LocalPlayer()
{

}

Networking *LocalPlayer::getNetworking()
{
    return mwmp::Main::get().getNetworking();
}

MWWorld::Ptr LocalPlayer::getPlayerPtr()
{
    return MWBase::Environment::get().getWorld()->getPlayerPtr();
}

void LocalPlayer::update()
{
    updateCell();
    updatePosition();
    updateAnimFlags();
    updateAttackState();
    updateDeadState();
    updateEquipment();
    updateStatsDynamic();
    updateAttributes();
    updateSkills();
    updateLevel();
}

void LocalPlayer::charGen(int stageFirst, int stageEnd)
{
    charGenStage.current = stageFirst;
    charGenStage.end = stageEnd;
}

bool LocalPlayer::charGenThread()
{
    MWBase::WindowManager *windowManager = MWBase::Environment::get().getWindowManager();

    // If we haven't finished CharGen and we're in a menu, it must be
    // one of the CharGen menus, so go no further until it's closed
    if (windowManager->isGuiMode() && charGenStage.end != 0)
        return false;

    // If the current stage of CharGen is not the last one,
    // move to the next one
    else if (charGenStage.current < charGenStage.end)
    {
        switch (charGenStage.current)
        {
        case 0:
            windowManager->pushGuiMode(MWGui::GM_Name);
            break;
        case 1:
            windowManager->pushGuiMode(MWGui::GM_Race);
            break;
        case 2:
            windowManager->pushGuiMode(MWGui::GM_Class);
            break;
        case 3:
            windowManager->pushGuiMode(MWGui::GM_Birth);
            break;
        default:
            windowManager->pushGuiMode(MWGui::GM_Review);
            break;
        }
        getNetworking()->getPlayerPacket(ID_PLAYER_CHARGEN)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_CHARGEN)->Send();
        charGenStage.current++;

        return false;
    }

    // If we've reached the last stage of CharGen, send the
    // corresponding packets and mark CharGen as finished
    else if (charGenStage.end != 0)
    {
        MWBase::World *world = MWBase::Environment::get().getWorld();
        MWWorld::Ptr player = world->getPlayerPtr();
        npc = *player.get<ESM::NPC>()->mBase;
        birthsign = world->getPlayer().getBirthSign();

        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_BASEINFO to server with my CharGen info");
        getNetworking()->getPlayerPacket(ID_PLAYER_BASEINFO)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_BASEINFO)->Send();

        // Send stats packets if this is the 2nd round of CharGen that
        // only happens for new characters
        if (charGenStage.end != 1)
        {
            updateStatsDynamic(true);
            updateAttributes(true);
            updateSkills(true);
            updateLevel(true);
            sendClass();
            sendSpellbook();
            getNetworking()->getPlayerPacket(ID_PLAYER_CHARGEN)->setPlayer(this);
            getNetworking()->getPlayerPacket(ID_PLAYER_CHARGEN)->Send();
        }

        sendCellStates();

        // Set the last stage variable to 0 to indicate that CharGen is finished
        charGenStage.end = 0;
    }

    return true;
}

bool LocalPlayer::hasFinishedCharGen()
{
    return charGenStage.end == 0;
}

void LocalPlayer::updateStatsDynamic(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();

    MWMechanics::CreatureStats *ptrCreatureStats = &player.getClass().getCreatureStats(player);
    MWMechanics::DynamicStat<float> health(ptrCreatureStats->getHealth());
    MWMechanics::DynamicStat<float> magicka(ptrCreatureStats->getMagicka());
    MWMechanics::DynamicStat<float> fatigue(ptrCreatureStats->getFatigue());

    static MWMechanics::DynamicStat<float> oldHealth(ptrCreatureStats->getHealth());
    static MWMechanics::DynamicStat<float> oldMagicka(ptrCreatureStats->getMagicka());
    static MWMechanics::DynamicStat<float> oldFatigue(ptrCreatureStats->getFatigue());

    static float timer = 0;
    const float timeoutSec = 0.5;

    if ((timer += MWBase::Environment::get().getFrameDuration()) >= timeoutSec || forceUpdate)
    {
        if (oldHealth != health || oldMagicka != magicka || oldFatigue != fatigue || forceUpdate)
        {
            oldHealth = health;
            oldMagicka = magicka;
            oldFatigue = fatigue;

            health.writeState(creatureStats.mDynamic[0]);
            magicka.writeState(creatureStats.mDynamic[1]);
            fatigue.writeState(creatureStats.mDynamic[2]);

            timer = 0;

            getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->setPlayer(this);
            getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->Send();
        }
    }
}

void LocalPlayer::updateAttributes(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();
    const MWMechanics::NpcStats &ptrNpcStats = player.getClass().getNpcStats(player);
    bool attributesChanged = false;

    for (int i = 0; i < 8; ++i)
    {
        if (ptrNpcStats.getAttribute(i).getBase() != creatureStats.mAttributes[i].mBase)
        {
            ptrNpcStats.getAttribute(i).writeState(creatureStats.mAttributes[i]);
            attributesChanged = true;
        }
    }

    if (attributesChanged || forceUpdate)
    {
        getNetworking()->getPlayerPacket(ID_PLAYER_ATTRIBUTE)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_ATTRIBUTE)->Send();
    }
}

void LocalPlayer::updateSkills(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();
    const MWMechanics::NpcStats &ptrNpcStats = player.getClass().getNpcStats(player);

    // Track whether skills have changed their values, but not whether
    // progress towards skill increases has changed (to not spam server
    // with packets every time tiny progress is made)
    bool skillsChanged = false;

    for (int i = 0; i < 27; ++i)
    {
        if (ptrNpcStats.getSkill(i).getBase() != npcStats.mSkills[i].mBase)
        {
            ptrNpcStats.getSkill(i).writeState(npcStats.mSkills[i]);
            skillsChanged = true;
        }
        // If we only have skill progress, remember it for future packets,
        // but don't send a packet just because of this
        else if (ptrNpcStats.getSkill(i).getProgress() != npcStats.mSkills[i].mProgress)
        {
            ptrNpcStats.getSkill(i).writeState(npcStats.mSkills[i]);
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (ptrNpcStats.getSkillIncrease(i) != npcStats.mSkillIncrease[i]) {
            npcStats.mSkillIncrease[i] = ptrNpcStats.getSkillIncrease(i);
        }
    }

    if (skillsChanged || forceUpdate)
    {
        npcStats.mLevelProgress = ptrNpcStats.getLevelProgress();
        getNetworking()->getPlayerPacket(ID_PLAYER_SKILL)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_SKILL)->Send();
    }
}

void LocalPlayer::updateLevel(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();
    const MWMechanics::NpcStats &ptrNpcStats = player.getClass().getNpcStats(player);

    if (ptrNpcStats.getLevel() != creatureStats.mLevel || forceUpdate)
    {
        creatureStats.mLevel = ptrNpcStats.getLevel();
        getNetworking()->getPlayerPacket(ID_PLAYER_LEVEL)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_LEVEL)->Send();

        // Also update skills to refresh level progress and attribute bonuses
        // for next level up
        updateSkills(true);
    }
}

void LocalPlayer::updatePosition(bool forceUpdate)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    const MWMechanics::Movement &move = player.getClass().getMovementSettings(player);

    static bool posWasChanged = false;

    static bool isJumping = false;
    static bool sentJumpEnd = true;

    ESM::Position ptrPos = player.getRefData().getPosition();

    const bool posIsChanging = (move.mPosition[0] != 0 || move.mPosition[1] != 0 || move.mPosition[2] != 0
                               || move.mRotation[0] != 0 || move.mRotation[1] != 0 || move.mRotation[2] != 0);

    if (posIsChanging || posWasChanged || forceUpdate)
    {
        posWasChanged = posIsChanging;

        if (!isJumping && !world->isOnGround(player) && !world->isFlying(player))
        {
            isJumping = true;
        }

        position = ptrPos;

        direction.pos[0] = move.mPosition[0];
        direction.pos[1] = move.mPosition[1];
        direction.pos[2] = move.mPosition[2];

        getNetworking()->getPlayerPacket(ID_PLAYER_POS)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_POS)->Send();
    }
    else if (isJumping && world->isOnGround(player))
    {
        isJumping = false;
        sentJumpEnd = false;
    }
    // Packet with jump end position has to be sent one tick after above check
    else if (!sentJumpEnd)
    {
        sentJumpEnd = true;
        position = ptrPos;
        getNetworking()->getPlayerPacket(ID_PLAYER_POS)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_POS)->Send();
    }
}

void LocalPlayer::updateCell(bool forceUpdate)
{
    const ESM::Cell *ptrCell = MWBase::Environment::get().getWorld()->getPlayerPtr().getCell()->getCell();
    bool cellChanged = false;

    // Send a packet to server to update this LocalPlayer's cell if:
    // 1) forceUpdate is true
    // 2) The LocalPlayer's cell name does not equal the World Player's cell name
    // 3) The LocalPlayer's exterior cell coordinates do not equal the World Player's
    //    exterior cell coordinates
    if (forceUpdate)
    {
        cellChanged = true;
    }
    else if (!Misc::StringUtils::ciEqual(ptrCell->mName, cell.mName))
    {
        cellChanged = true;
    }
    else if (ptrCell->isExterior())
    {
        if (ptrCell->mData.mX != cell.mData.mX)
        {
            cellChanged = true;
        }
        else if (ptrCell->mData.mY != cell.mData.mY)
        {
            cellChanged = true;
        }
    }

    if (cellChanged)
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_CELL_CHANGE to server");

        LOG_APPEND(Log::LOG_INFO, "- Moved from %s to %s", cell.getDescription().c_str(), ptrCell->getDescription().c_str());

        cell = *ptrCell;

        // Make sure the position is updated before a cell packet is sent, or else
        // cell change events in server scripts will have the wrong player position
        updatePosition(true);

        getNetworking()->getPlayerPacket(ID_PLAYER_CELL_CHANGE)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_CELL_CHANGE)->Send();

        // Also force an update to skills (to send all progress to skill increases)
        updateSkills(true);

        // Also check if the inventory needs to be updated
        updateInventory();
    }
}

void LocalPlayer::updateChar()
{
    MWBase::Environment::get().getMechanicsManager()->setPlayerRace(
        npc.mRace,
        npc.isMale(),
        npc.mHead,
        npc.mHair
    );

    MWBase::Environment::get().getMechanicsManager()->setPlayerBirthsign(birthsign);

    MWBase::Environment::get().getWindowManager()->getInventoryWindow()->rebuildAvatar();
}

void LocalPlayer::updateEquipment(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();

    static bool equipChanged = false;

    if (forceUpdate)
        equipChanged = true;

    MWWorld::InventoryStore &invStore = player.getClass().getInventoryStore(player);
    for (int slot = 0; slot < MWWorld::InventoryStore::Slots; slot++)
    {
        MWWorld::ContainerStoreIterator it = invStore.getSlot(slot);
        if (it != invStore.end() && !::Misc::StringUtils::ciEqual(it->getCellRef().getRefId(), equipedItems[slot].refId))
        {
            equipChanged = true;

            equipedItems[slot].refId = it->getCellRef().getRefId();
            equipedItems[slot].charge = it->getCellRef().getCharge();
            if (slot == MWWorld::InventoryStore::Slot_CarriedRight)
            {
                MWMechanics::WeaponType weaptype;
                MWMechanics::getActiveWeapon(player.getClass().getCreatureStats(player), player.getClass().getInventoryStore(player), &weaptype);
                if (weaptype != MWMechanics::WeapType_Thrown)
                    equipedItems[slot].count = 1;
            }
            else
                equipedItems[slot].count = invStore.count(it->getCellRef().getRefId());
        }
        else if (it == invStore.end() && !equipedItems[slot].refId.empty())
        {
            equipChanged = true;
            equipedItems[slot].refId = "";
            equipedItems[slot].count = 0;
            equipedItems[slot].charge = 0;
        }
    }

    if (equipChanged)
    {
        getNetworking()->getPlayerPacket(ID_PLAYER_EQUIPMENT)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_EQUIPMENT)->Send();
        equipChanged = false;
    }
}

void LocalPlayer::updateInventory(bool forceUpdate)
{
    static bool invChanged = false;

    if (forceUpdate)
        invChanged = true;

    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWWorld::InventoryStore &ptrInventory = ptrPlayer.getClass().getInventoryStore(ptrPlayer);
    mwmp::Item item;

    if (!invChanged)
    {
        for (vector<Item>::iterator iter = inventoryChanges.items.begin(); iter != inventoryChanges.items.end(); ++iter)
        {
            MWWorld::ContainerStoreIterator result(ptrInventory.begin());
            for (; result != ptrInventory.end(); ++result)
            {
                item.refId = result->getCellRef().getRefId();
                if (item.refId.find("$dynamic") != string::npos) // skip generated items (self enchanted for e.g.)
                    continue;

                item.count = result->getRefData().getCount();
                item.charge = result->getCellRef().getCharge();

                if (item == (*iter))
                    break;
            }
            if (result == ptrInventory.end())
            {
                invChanged = true;
                break;
            }
        }
    }

    if (!invChanged)
    {
        for (MWWorld::ContainerStoreIterator iter(ptrInventory.begin()); iter != ptrInventory.end(); ++iter)
        {
            item.refId = iter->getCellRef().getRefId();
            if (item.refId.find("$dynamic") != string::npos) // skip generated items (self enchanted for e.g.)
                continue;

            item.count = iter->getRefData().getCount();
            item.charge = iter->getCellRef().getCharge();

            vector<Item>::iterator result = inventoryChanges.items.begin();

            for (; result != inventoryChanges.items.end(); result++)
            {
                if ((*result) == item)
                    break;
            }

            if (result == inventoryChanges.items.end())
            {
                invChanged = true;
                break;
            }
        }
    }

    if (!invChanged)
        return;

    invChanged = false;

    sendInventory();
}

void LocalPlayer::updateAttackState(bool forceUpdate)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = getPlayerPtr();

    using namespace MWMechanics;

    static bool attackPressed = false; // prevent flood
    MWMechanics::DrawState_ state = player.getClass().getNpcStats(player).getDrawState();
    //player.getClass().hit(player, 1, ESM::Weapon::AT_Chop);
    if (world->getPlayer().getAttackingOrSpell() && !attackPressed)
    {
        MWWorld::Ptr weapon = MWWorld::Ptr(); // hand-to-hand
                                              //player.getClass().onHit(player, 0.5, true, weapon, 0, 1);
        if (state == MWMechanics::DrawState_Spell)
        {
            const string &spell = MWBase::Environment::get().getWindowManager()->getSelectedSpell();

            attack.attacker = guid;
            attack.type = Attack::MAGIC;
            attack.pressed = true;
            attack.refid = spell;
        }
        else if (state == MWMechanics::DrawState_Weapon)
        {
            //PrepareAttack(2);
        }
        attackPressed = true;
    }
    else if (!world->getPlayer().getAttackingOrSpell() && attackPressed)
    {
        if (/*state == MWMechanics::DrawState_Spell ||*/ state == MWMechanics::DrawState_Weapon)
        {
            //localNetPlayer->getAttack()->success = false;
            //SendAttack(0);
        }
        attackPressed = false;
    }
}

void LocalPlayer::updateDeadState(bool forceUpdate)
{
    MWWorld::Ptr player = getPlayerPtr();

    MWMechanics::NpcStats *ptrNpcStats = &player.getClass().getNpcStats(player);
    static bool isDead = false;

    if (ptrNpcStats->isDead() && !isDead)
    {
        creatureStats.mDead = true;

        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_DEATH to server about myself");
        getNetworking()->getPlayerPacket(ID_PLAYER_DEATH)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_DEATH)->Send();
        isDead = true;
    }
    else if (ptrNpcStats->getHealth().getCurrent() > 0 && isDead)
        isDead = false;
}

void LocalPlayer::updateAnimFlags(bool forceUpdate)
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();


    MWMechanics::NpcStats ptrNpcStats = player.getClass().getNpcStats(player);
    using namespace MWMechanics;

    static bool wasRunning = ptrNpcStats.getMovementFlag(CreatureStats::Flag_Run);
    static bool wasSneaking = ptrNpcStats.getMovementFlag(CreatureStats::Flag_Sneak);
    static bool wasForceJumping = ptrNpcStats.getMovementFlag(CreatureStats::Flag_ForceJump);
    static bool wasForceMoveJumping = ptrNpcStats.getMovementFlag(CreatureStats::Flag_ForceMoveJump);

    bool isRunning = ptrNpcStats.getMovementFlag(CreatureStats::Flag_Run);
    bool isSneaking = ptrNpcStats.getMovementFlag(CreatureStats::Flag_Sneak);
    bool isForceJumping = ptrNpcStats.getMovementFlag(CreatureStats::Flag_ForceJump);
    bool isForceMoveJumping = ptrNpcStats.getMovementFlag(CreatureStats::Flag_ForceMoveJump);
    
    isFlying = world->isFlying(player);
    bool isJumping = !world->isOnGround(player) && !isFlying;

    // We need to send a new packet at the end of jumping and flying too,
    // so keep track of what we were doing last frame
    static bool wasJumping = false;
    static bool wasFlying = false;

    MWMechanics::DrawState_ currentDrawState = player.getClass().getNpcStats(player).getDrawState();
    static MWMechanics::DrawState_ lastDrawState = player.getClass().getNpcStats(player).getDrawState();

    if (wasRunning != isRunning
        || wasSneaking != isSneaking || wasForceJumping != isForceJumping
        || wasForceMoveJumping != isForceMoveJumping || lastDrawState != currentDrawState
        || wasJumping || isJumping || wasFlying || isFlying
        || forceUpdate)
    {
        wasSneaking = isSneaking;
        wasRunning = isRunning;
        wasForceJumping = isForceJumping;
        wasForceMoveJumping = isForceMoveJumping;
        lastDrawState = currentDrawState;
        
        wasFlying = isFlying;
        wasJumping = isJumping;

        movementFlags = 0;

#define __SETFLAG(flag, value) (value) ? (movementFlags | flag) : (movementFlags & ~flag)

        movementFlags = __SETFLAG(CreatureStats::Flag_Sneak, isSneaking);
        movementFlags = __SETFLAG(CreatureStats::Flag_Run, isRunning);
        movementFlags = __SETFLAG(CreatureStats::Flag_ForceJump, isForceJumping);
        movementFlags = __SETFLAG(CreatureStats::Flag_ForceJump, isJumping);
        movementFlags = __SETFLAG(CreatureStats::Flag_ForceMoveJump, isForceMoveJumping);

#undef __SETFLAG

        if (currentDrawState == MWMechanics::DrawState_Nothing)
            drawState = 0;
        else if (currentDrawState == MWMechanics::DrawState_Weapon)
            drawState = 1;
        else if (currentDrawState == MWMechanics::DrawState_Spell)
            drawState = 2;

        if (isJumping)
            updatePosition(true); // fix position after jump;

        getNetworking()->getPlayerPacket(ID_PLAYER_ANIM_FLAGS)->setPlayer(this);
        getNetworking()->getPlayerPacket(ID_PLAYER_ANIM_FLAGS)->Send();
    }
}

void LocalPlayer::addItems()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWWorld::ContainerStore &ptrStore = ptrPlayer.getClass().getContainerStore(ptrPlayer);

    for (unsigned int i = 0; i < inventoryChanges.count; i++)
    {
        mwmp::Item item = inventoryChanges.items.at(i);
        MWWorld::Ptr itemPtr = *ptrStore.add(item.refId, item.count, ptrPlayer);
        if (item.charge != -1)
            itemPtr.getCellRef().setCharge(item.charge);
    }
}

void LocalPlayer::addSpells()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWMechanics::Spells &ptrSpells = ptrPlayer.getClass().getCreatureStats(ptrPlayer).getSpells();

    for (vector<ESM::Spell>::const_iterator spell = spellbookChanges.spells.begin(); spell != spellbookChanges.spells.end(); spell++)
        ptrSpells.add(spell->mId);
        
}

void LocalPlayer::addJournalItems()
{
    for (unsigned int i = 0; i < journalChanges.count; i++)
    {
        mwmp::JournalItem journalItem = journalChanges.journalItems.at(i);
        
        if (journalItem.type == JournalItem::ENTRY)
        {
            MWWorld::CellStore *ptrCellStore = Main::get().getCellController()->getCellStore(journalItem.actorCell);

            if (!ptrCellStore) continue;

            MWWorld::Ptr ptrFound = ptrCellStore->searchExact(journalItem.actorCellRef.mRefID,
                journalItem.actorCellRef.mRefNum.mIndex, journalItem.actorCellRef.mMpNum);

            if (!ptrFound)
            {
                ptrFound = getPlayerPtr();
            }

            MWBase::Environment::get().getJournal()->addEntry(journalItem.quest, journalItem.index, ptrFound);
        }
        else
        {
            MWBase::Environment::get().getJournal()->setJournalIndex(journalItem.quest, journalItem.index);
        }
    }
}

void LocalPlayer::removeItems()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWWorld::ContainerStore &ptrStore = ptrPlayer.getClass().getContainerStore(ptrPlayer);

    for (unsigned int i = 0; i < inventoryChanges.count; i++)
    {
        mwmp::Item item = inventoryChanges.items.at(i);
        ptrStore.remove(item.refId, item.count, ptrPlayer);
    }
}

void LocalPlayer::removeSpells()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWMechanics::Spells &ptrSpells = ptrPlayer.getClass().getCreatureStats(ptrPlayer).getSpells();

    for (vector<ESM::Spell>::const_iterator spell = spellbookChanges.spells.begin(); spell != spellbookChanges.spells.end(); spell++)
    {
        ptrSpells.remove(spell->mId);

        MWBase::WindowManager *wm = MWBase::Environment::get().getWindowManager();
        if (spell->mId == wm->getSelectedSpell())
            wm->unsetSelectedSpell();
    }
}

void LocalPlayer::setDynamicStats()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    MWMechanics::CreatureStats *ptrCreatureStats = &player.getClass().getCreatureStats(player);
    MWMechanics::DynamicStat<float> dynamicStat;

    for (int i = 0; i < 3; ++i)
    {
        dynamicStat = ptrCreatureStats->getDynamic(i);
        dynamicStat.setBase(creatureStats.mDynamic[i].mBase);
        dynamicStat.setCurrent(creatureStats.mDynamic[i].mCurrent);
        ptrCreatureStats->setDynamic(i, dynamicStat);
    }
}

void LocalPlayer::setAttributes()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    MWMechanics::CreatureStats *ptrCreatureStats = &player.getClass().getCreatureStats(player);
    MWMechanics::AttributeValue attributeValue;

    for (int i = 0; i < 8; ++i)
    {
        attributeValue.readState(creatureStats.mAttributes[i]);
        ptrCreatureStats->setAttribute(i, attributeValue);
    }
}

void LocalPlayer::setSkills()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    MWMechanics::NpcStats *ptrNpcStats = &player.getClass().getNpcStats(player);
    MWMechanics::SkillValue skillValue;

    for (int i = 0; i < 27; ++i)
    {
        skillValue.readState(npcStats.mSkills[i]);
        ptrNpcStats->setSkill(i, skillValue);
    }

    for (int i = 0; i < 8; ++i)
    {
        ptrNpcStats->setSkillIncrease(i, npcStats.mSkillIncrease[i]);
    }

    ptrNpcStats->setLevelProgress(npcStats.mLevelProgress);
}

void LocalPlayer::setLevel()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    MWMechanics::CreatureStats *ptrCreatureStats = &player.getClass().getCreatureStats(player);
    ptrCreatureStats->setLevel(creatureStats.mLevel);
}

void LocalPlayer::setPosition()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();

    // If we're ignoring this position packet because of an invalid cell change,
    // don't make the next one get ignored as well
    if (ignorePosPacket)
    {
        ignorePosPacket = false;
    }
    else
    {
        world->getPlayer().setTeleported(true);
        world->moveObject(player, position.pos[0], position.pos[1], position.pos[2]);
        world->rotateObject(player, position.rot[0], position.rot[1], position.rot[2]);
    }

    updatePosition(true);

    // Make sure we update our draw state, or we'll end up with the wrong one
    updateAnimFlags(true);
}

void LocalPlayer::setCell()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    MWWorld::Ptr player = world->getPlayerPtr();
    ESM::Position pos;

    // To avoid crashes, close any container menus this player may be in
    if (MWBase::Environment::get().getWindowManager()->containsMode(MWGui::GM_Container))
    {
        MWBase::Environment::get().getWindowManager()->removeGuiMode(MWGui::GM_Container);
        MWBase::Environment::get().getWindowManager()->setDragDrop(false);
    }

    world->getPlayer().setTeleported(true);

    int x = cell.mData.mX;
    int y = cell.mData.mY;

    if (cell.isExterior())
    {
        world->indexToPosition(x, y, pos.pos[0], pos.pos[1], true);
        pos.pos[2] = 0;

        pos.rot[0] = pos.rot[1] = pos.rot[2] = 0;

        world->changeToExteriorCell(pos, true);
        world->fixPosition(player);
    }
    else if (world->findExteriorPosition(cell.mName, pos))
    {
        world->changeToExteriorCell(pos, true);
        world->fixPosition(player);
    }
    else
    {
        try
        {
            world->findInteriorPosition(cell.mName, pos);
            world->changeToInteriorCell(cell.mName, pos, true);
        }
        // If we've been sent to an invalid interior, ignore the incoming
        // packet about our position in that cell
        catch (std::exception&)
        {
            LOG_APPEND(Log::LOG_INFO, "%s", "- Cell doesn't exist on this client");
            ignorePosPacket = true;
        }
    }

    updateCell(true);
}

void LocalPlayer::setClass()
{
    if (charClass.mId.empty()) // custom class
    {
        charClass.mData.mIsPlayable = 0x1;
        MWBase::Environment::get().getMechanicsManager()->setPlayerClass(charClass);
        MWBase::Environment::get().getWindowManager()->setPlayerClass(charClass);
    }
    else
    {
        MWBase::Environment::get().getMechanicsManager()->setPlayerClass(charClass.mId);

        const ESM::Class *existingCharClass = MWBase::Environment::get().getWorld()->getStore().get<ESM::Class>().find(charClass.mId);

        if (existingCharClass)
            MWBase::Environment::get().getWindowManager()->setPlayerClass(charClass);
    }
}

void LocalPlayer::setEquipment()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();

    MWWorld::InventoryStore &ptrInventory = ptrPlayer.getClass().getInventoryStore(ptrPlayer);

    for (int slot = 0; slot < MWWorld::InventoryStore::Slots; slot++)
    {
        mwmp::Item *currentItem = &equipedItems[slot];

        if (!currentItem->refId.empty())
        {
            MWWorld::ContainerStoreIterator it = ptrInventory.begin();
            for (; it != ptrInventory.end(); ++it) // find item in inventory
            {
                if (::Misc::StringUtils::ciEqual(it->getCellRef().getRefId(), currentItem->refId))
                    break;
            }
            if (it == ptrInventory.end()) // if not exists add item
                ptrInventory.equip(
                    slot,
                    ptrInventory.ContainerStore::add(
                        equipedItems[slot].refId.c_str(),
                        equipedItems[slot].count,
                        ptrPlayer),
                    ptrPlayer);
            else
                ptrInventory.equip(slot, it, ptrPlayer);
        }
        else
        {
            ptrInventory.unequipSlot(slot, ptrPlayer);
        }
    }
}

void LocalPlayer::setInventory()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWWorld::ContainerStore &ptrStore = ptrPlayer.getClass().getContainerStore(ptrPlayer);

    // Clear items in inventory
    ptrStore.clear();

    // Proceed by adding items
    addItems();

    // Don't automatically setEquipment() here, or the player could end
    // up getting a new set of their starting clothes, or other items
    // supposed to no longer exist
    //
    // Instead, expect server scripts to do that manually
}

void LocalPlayer::setSpellbook()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWMechanics::Spells &ptrSpells = ptrPlayer.getClass().getCreatureStats(ptrPlayer).getSpells();

    // Clear spells in spellbook, while ignoring abilities, powers, etc.
    while (true)
    {
        MWMechanics::Spells::TIterator iter = ptrSpells.begin();
        for (; iter != ptrSpells.end(); iter++)
        {
            const ESM::Spell *spell = iter->first;
            if (spell->mData.mType == ESM::Spell::ST_Spell)
            {
                ptrSpells.remove(spell->mId);
                break;
            }
        }
        if (iter == ptrSpells.end())
            break;
    }

    // Proceed by adding spells
    addSpells();
}

void LocalPlayer::sendClass()
{
    MWBase::World *world = MWBase::Environment::get().getWorld();
    const ESM::NPC *cpl = world->getPlayerPtr().get<ESM::NPC>()->mBase;
    const ESM::Class *cls = world->getStore().get<ESM::Class>().find(cpl->mClass);

    if (cpl->mClass.find("$dynamic") != string::npos) // custom class
    {
        charClass.mId = "";
        charClass.mName = cls->mName;
        charClass.mDescription = cls->mDescription;
        charClass.mData = cls->mData;
    }
    else
        charClass.mId = cls->mId;

    getNetworking()->getPlayerPacket(ID_PLAYER_CHARCLASS)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_CHARCLASS)->Send();
}

void LocalPlayer::sendInventory()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWWorld::InventoryStore &ptrInventory = ptrPlayer.getClass().getInventoryStore(ptrPlayer);
    mwmp::Item item;

    inventoryChanges.items.clear();

    for (MWWorld::ContainerStoreIterator iter(ptrInventory.begin()); iter != ptrInventory.end(); ++iter)
    {
        item.refId = iter->getCellRef().getRefId();
        if (item.refId.find("$dynamic") != string::npos) // skip generated items (self enchanted for e.g.)
            continue;

        item.count = iter->getRefData().getCount();
        item.charge = iter->getCellRef().getCharge();

        inventoryChanges.items.push_back(item);
    }

    inventoryChanges.count = (unsigned int) inventoryChanges.items.size();
    inventoryChanges.action = InventoryChanges::SET;
    getNetworking()->getPlayerPacket(ID_PLAYER_INVENTORY)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_INVENTORY)->Send();
}

void LocalPlayer::sendSpellbook()
{
    MWWorld::Ptr ptrPlayer = getPlayerPtr();
    MWMechanics::Spells &ptrSpells = ptrPlayer.getClass().getCreatureStats(ptrPlayer).getSpells();

    spellbookChanges.spells.clear();

    // Send spells in spellbook, while ignoring abilities, powers, etc.
    for (MWMechanics::Spells::TIterator iter = ptrSpells.begin(); iter != ptrSpells.end(); ++iter)
    {
        const ESM::Spell *spell = iter->first;

        if (spell->mData.mType == ESM::Spell::ST_Spell)
        {
            spellbookChanges.spells.push_back(*spell);
        }
    }

    spellbookChanges.action = SpellbookChanges::SET;
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->Send();
}

void LocalPlayer::sendCellStates()
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Sending ID_PLAYER_CELL_STATE to server");
    getNetworking()->getPlayerPacket(ID_PLAYER_CELL_STATE)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_CELL_STATE)->Send();

}

void LocalPlayer::sendSpellAddition(std::string id)
{
    if (id.find("$dynamic") != string::npos) // skip custom spells
        return;

    spellbookChanges.spells.clear();

    ESM::Spell spell;
    spell.mId = id;
    spellbookChanges.spells.push_back(spell);

    spellbookChanges.action = SpellbookChanges::ADD;
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->Send();
}

void LocalPlayer::sendSpellRemoval(std::string id)
{
    if (id.find("$dynamic") != string::npos) // skip custom spells
        return;

    spellbookChanges.spells.clear();

    ESM::Spell spell;
    spell.mId = id;
    spellbookChanges.spells.push_back(spell);

    spellbookChanges.action = SpellbookChanges::REMOVE;
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_SPELLBOOK)->Send();
}

void LocalPlayer::sendSpellAddition(const ESM::Spell &spell)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Not implemented.");
}

void LocalPlayer::sendSpellRemoval(const ESM::Spell &spell)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Not implemented.");
}

void LocalPlayer::sendJournalEntry(const std::string& quest, int index, const MWWorld::Ptr& actor)
{
    journalChanges.journalItems.clear();

    mwmp::JournalItem journalItem;
    journalItem.type = JournalItem::ENTRY;
    journalItem.quest = quest;
    journalItem.index = index;

    if (actor.mCell != nullptr)
        journalItem.actorCell = *actor.mCell->getCell();

    journalItem.actorCellRef.mRefID = actor.getCellRef().getRefId();
    journalItem.actorCellRef.mRefNum = actor.getCellRef().getRefNum();

    journalChanges.journalItems.push_back(journalItem);

    getNetworking()->getPlayerPacket(ID_PLAYER_JOURNAL)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_JOURNAL)->Send();
}

void LocalPlayer::sendJournalIndex(const std::string& quest, int index)
{
    journalChanges.journalItems.clear();

    mwmp::JournalItem journalItem;
    journalItem.type = JournalItem::INDEX;
    journalItem.quest = quest;
    journalItem.index = index;

    journalChanges.journalItems.push_back(journalItem);

    getNetworking()->getPlayerPacket(ID_PLAYER_JOURNAL)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_JOURNAL)->Send();
}

void LocalPlayer::sendAttack(Attack::TYPE type)
{
    MWMechanics::DrawState_ state = getPlayerPtr().getClass().getNpcStats(getPlayerPtr()).getDrawState();

    attack.type = type;
    attack.pressed = false;

    getNetworking()->getPlayerPacket(ID_PLAYER_ATTACK)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_ATTACK)->Send();
}

void LocalPlayer::clearCellStates()
{
    cellStateChanges.cellStates.clear();
}

void LocalPlayer::clearCurrentContainer()
{
    currentContainer.refId = "";
    currentContainer.refNumIndex = 0;
    currentContainer.mpNum = 0;
}

void LocalPlayer::storeCellState(ESM::Cell cell, int stateType)
{
    std::vector<CellState>::iterator iter;

    for (iter = cellStateChanges.cellStates.begin(); iter != cellStateChanges.cellStates.end(); )
    {
        // If there's already a cell state recorded for this particular cell,
        // remove it
        if (cell.getDescription() == (*iter).cell.getDescription())
        {
            iter = cellStateChanges.cellStates.erase(iter);
        }
        else
            ++iter;
    }

    CellState cellState;
    cellState.cell = cell;
    cellState.type = stateType;

    cellStateChanges.cellStates.push_back(cellState);
}

void LocalPlayer::storeCurrentContainer(const MWWorld::Ptr &container, bool loot)
{
    currentContainer.refId = container.getCellRef().getRefId();
    currentContainer.refNumIndex = container.getCellRef().getRefNum().mIndex;
    currentContainer.mpNum = container.getCellRef().getMpNum();
    currentContainer.loot = loot;
}

void LocalPlayer::prepareAttack(Attack::TYPE type, bool state)
{
    if (attack.pressed == state && type != Attack::MAGIC)
        return;

    MWMechanics::DrawState_ dstate = getPlayerPtr().getClass().getNpcStats(getPlayerPtr()).getDrawState();

    if (dstate == MWMechanics::DrawState_Spell)
    {
        const string &spell = MWBase::Environment::get().getWindowManager()->getSelectedSpell();
        attack.success = Misc::Rng::roll0to99() < MWMechanics::getSpellSuccessChance(spell, getPlayerPtr());
        state = true;
        attack.refid = spell;
    }
    else
    {
        attack.success = false;
    }

    attack.pressed = state;
    attack.type = type;
    attack.knockdown = false;
    attack.block = false;
    attack.target = RakNet::RakNetGUID();
    attack.attacker = guid;

    getNetworking()->getPlayerPacket(ID_PLAYER_ATTACK)->setPlayer(this);
    getNetworking()->getPlayerPacket(ID_PLAYER_ATTACK)->Send();
}
