//
// Created by koncord on 07.01.16.
//

#ifndef OPENMW_BASEPLAYER_HPP
#define OPENMW_BASEPLAYER_HPP

#include <components/esm/npcstats.hpp>
#include <components/esm/loadcell.hpp>
#include <components/esm/loadnpc.hpp>
#include <components/esm/creaturestats.hpp>
#include <components/esm/loadclas.hpp>
#include <components/esm/loadspel.hpp>
#include <RakNetTypes.h>

namespace mwmp
{
    class Attack
    {
    public:
        RakNet::RakNetGUID target;
        RakNet::RakNetGUID attacker;
        char type; // 0 - melee, 1 - magic, 2 - throwable
        enum TYPE
        {
            MELEE = 0,
            MAGIC,
            THROWABLE
        };
        std::string refid; // id of spell (e.g. "fireball")
        char success;
        char block;
        float damage;
        char pressed;
        char knockdown;
    };

    struct Item
    {
        std::string refid;
        int count;
        int health;
        inline bool operator==(const Item& rhs)
        {
            return refid == rhs.refid && count == rhs.count && health == rhs.health;
        }
    };

    struct JournalItem
    {
        std::string quest;
        int index;
        enum JOURNAL_ITEM_TYPE
        {
            ENTRY = 0,
            INDEX = 1
        };

        ESM::Cell actorCell;
        ESM::CellRef actorCellRef;

        int type; // 0 - An entire entry, 1 - An index
    };

    struct JournalChanges
    {
        std::vector<JournalItem> journalItems;
        unsigned int count;
    };

    struct InventoryChanges
    {
        std::vector<Item> items;
        unsigned int count;
        enum ACTION_TYPE
        {
            SET = 0,
            ADD,
            REMOVE
        };
        int action; //0 - Clear and set in entirety, 1 - Add item, 2 - Remove item
    };

    struct SpellbookChanges
    {
        std::vector<ESM::Spell> spells;
        unsigned int count;
        enum ACTION_TYPE
        {
            SET = 0,
            ADD,
            REMOVE
        };
        int action; //0 - Clear and set in entirety, 1 - Add spell, 2 - Remove spell
    };

    struct CellStateChanges
    {
        std::vector<ESM::Cell> cells;
        unsigned int count;

        enum ACTION_TYPE
        {
            LOAD = 0,
            UNLOAD = 1
        };

        int action; // 0 - Load a cell, 1 - Unload a cell
    };

    class BasePlayer
    {
    public:

        struct CGStage
        {
            int current, end;
        };

        struct GUIMessageBox
        {
            int id;
            int type;
            enum GUI_TYPE
            {
                MessageBox = 0,
                CustomMessageBox,
                InputDialog,
                PasswordDialog,
                ListBox
            };
            std::string label;
            std::string buttons;

            std::string data;
        };

        BasePlayer(RakNet::RakNetGUID guid) : guid(guid)
        {
            inventoryChanges.action = 0;
            inventoryChanges.count = 0;
            spellbookChanges.action = 0;
            spellbookChanges.count = 0;
        }

        BasePlayer()
        {

        }

        RakNet::RakNetGUID guid;
        GUIMessageBox guiMessageBox;
        ESM::Class charClass;
        int month;
        int day;
        double hour;
        InventoryChanges inventoryChanges;
        SpellbookChanges spellbookChanges;
        JournalChanges journalChanges;
        CellStateChanges cellStateChanges;
        bool consoleAllowed;
        bool ignorePosPacket;
        ESM::ActiveSpells activeSpells;

        ESM::Position position;
        ESM::Position direction;
        ESM::Cell cell;
        ESM::NPC npc;
        ESM::NpcStats npcStats;
        ESM::CreatureStats creatureStats;
        Item equipedItems[19];
        unsigned int movementFlags;
        char movementAnim;
        char drawState;
        Attack attack;
        std::string birthsign;
        std::string chatMessage;
        CGStage charGenStage;
        std::string passw;
    };
}

#endif //OPENMW_BASEPLAYER_HPP
