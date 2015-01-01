/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SUNKENTEMPLE_H
#define DEF_SUNKENTEMPLE_H

enum Data
{
    TYPE_ATALALARION,
    TYPE_ATALAI_DEFENDERS,
    TYPE_JAMMALAN_THE_PROPHET,
    TYPE_SHADE_OF_ERANIKUS,
    TYPE_MALFURION,
    TYPE_AVATAR_OF_HAKKAR,
    MAX_ENCOUNTER,
};

enum Statues
{
    ATALAI_STATUE_S                 = 0x001,
    ATALAI_STATUE_N                 = 0x002,
    ATALAI_STATUE_SW                = 0x004,
    ATALAI_STATUE_SE                = 0x008,
    ATALAI_STATUE_NW                = 0x010,
    ATALAI_STATUE_NE                = 0x020,
    ATALAI_STATUE_ALL               = ATALAI_STATUE_S | ATALAI_STATUE_N | ATALAI_STATUE_SW |
                                    ATALAI_STATUE_SE | ATALAI_STATUE_NW | ATALAI_STATUE_NE
};

enum Creatures
{
    NPC_ATALALARION                 = 8580,
    NPC_GASHER                      = 5713,
    NPC_HUKKU                       = 5715,
    NPC_LORO                        = 5714,
    NPC_MIJAN                       = 5717,
    NPC_ZOLO                        = 5712,
    NPC_ZULLOR                      = 5716,
    NPC_JAMMALAN_THE_PROPHET        = 5710,
    NPC_OGOM_THE_WRETCHED           = 5711,
    NPC_ATALAI_DEATHWALKER          = 5271,
    NPC_ATALAI_HIGH_PRIEST          = 5273,
    NPC_ATALAI_PRIEST               = 5269,
    NPC_MUMMIFIED_ATALAI            = 5263,
    NPC_DREAMSCYTHE                 = 5721,
    NPC_WEAVER                      = 5720,
    NPC_MORPHAZ                     = 5719,
    NPC_HAZZAS                      = 5722,
    NPC_NIGHTMARE_SCALEBANE         = 5277,
    NPC_NIGHTMARE_SUPPRESSOR        = 8497,
    NPC_NIGHTMARE_WANDERER          = 5283,
    NPC_NIGHTMARE_WHELP             = 8319,
    NPC_NIGHTMARE_WYRMKIN           = 5280,
    NPC_SHADE_OF_ERANIKUS           = 5709,
    NPC_SHADE_OF_HAKKAR             = 8440,
    NPC_AVATAR_OF_HAKKAR            = 8443,
    NPC_HAKKARI_BLOODKEEPER         = 8438,
    NPC_HAKKARI_MINION              = 8437,
    NPC_MALFURION                   = 15362
};

enum GameObjects
{
    GO_ATALAI_STATUE_S              = 148830,
    GO_ATALAI_STATUE_N              = 148831,
    GO_ATALAI_STATUE_SW             = 148832,
    GO_ATALAI_STATUE_SE             = 148833,
    GO_ATALAI_STATUE_NW             = 148834,
    GO_ATALAI_STATUE_NE             = 148835,
    GO_IDOL_OF_HAKKAR               = 148838,
    GO_FORCE_FIELD                  = 149431,
    GO_ETERNAL_FLAME_1              = 148418,
    GO_ETERNAL_FLAME_2              = 148419,
    GO_ETERNAL_FLAME_3              = 148420,
    GO_ETERNAL_FLAME_4              = 148421,
    GO_EVIL_GOD_SUMMONING_CIRCLE    = 148998,
    GO_SANCTUM_DOOR_1               = 149432,
    GO_SANCTUM_DOOR_2               = 149433,
    GO_SANCTUM_OF_THE_FALLEN_GOD    = 300019
};

enum Factions
{
    FACTION_FRIENDLY                = 35,
    FACTION_DRAGONKIN               = 50
};

enum Misc
{
    MAX_DEFENDER                    = 6,

    ITEM_HAKKARI_BLOOD              = 10460,
    SAY_ZONE_JAMMAL                 = -1109006
};

struct Loc
{
    float x, y, z, o;
};

class MANGOS_DLL_DECL instance_sunken_temple : public ScriptedInstance
{
    public:
        instance_sunken_temple(Map* pMap);
        ~instance_sunken_temple() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

        void HandleAtalaiStatue(uint32 uiEntry);
        void SummonAtalalarion(GameObject* pGo);
        void CallForHelp(uint32 uiCalllerEntry, GUIDList lCallings);
        void HandleAvatarEventWave();
        uint32 GetWaveCount() { return m_uiWaveCount;}

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        bool m_bIsSerpentSummoning;

        uint32 m_uiSerpent;
        uint32 m_uiSerpentSummonTimer;
        uint32 m_uiStatueOrder;
        uint32 m_uiWaveCount;

        GUIDList m_lTrollGUID;
        GUIDList m_lDragonkinGUID;
        GUIDList m_lStatueGUID;
        GUIDList m_lEternalFlameGUID;
};

#endif
