/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BRS_H
#define DEF_BRS_H

enum Data
{
    TYPE_MOR_GRAYHOOF,
    TYPE_UROK_DOOMHOWL,
    TYPE_DRAGONSPINE_HALL,
    TYPE_PYROGUARD_EMBERSEER,
    TYPE_ROOKERY,
    TYPE_REND_BLACKHAND,
    TYPE_LORD_VALTHALAK,
    TYPE_DRAKKISATH,

    MAX_ENCOUNTER
};

enum Areatriggers
{
    AT_UBRS_PREQUEST_1              = 1946,
    AT_UBRS_PREQUEST_2              = 1986,
    AT_UBRS_PREQUEST_3              = 1987,
    AT_STADIUM                      = 2026,
    AT_UBRS_ENTRY                   = 2046
};

enum Creatures
{
    // Bosses
    NPC_GIZRUL_THE_SLAVENER         = 10268,
    NPC_MOR_GRAYHOOF                = 16080,
    NPC_MOR_GRAYHOOF_TRIGGER        = 16044,
    NPC_UROK_DOOMHOWL               = 10584,
    NPC_PYROGUARD_EMBERSEER         = 9816,
    NPC_GYTH                        = 10339,
    NPC_WARCHIEF_REND_BLACKHAND     = 10429,
    NPC_LORD_VALTHALAK              = 16042,
    NPC_LORD_VALTHALAK_TRIGGER      = 16048,
    NPC_LORD_VICTOR_NEFARIUS        = 10162,
    NPC_JED_RUNEWATCHER             = 10509,

    // NPCs
    NPC_FINKLE_EINHORN              = 10776,
    NPC_VAELAN                      = 10296,

    // Trash
	NPC_FIRE_TONGUE					= 10372,
    NPC_BLACKHAND_DRAGON_HANDLER    = 10742,
    NPC_BLACKHAND_INCARCERATOR      = 10316,
    NPC_BLACKHAND_VETERAN           = 9819,
    NPC_BLACKHAND_SUMMONER          = 9818,
    NPC_BLOODAXE_VETERAN            = 9583,
    NPC_CHROMATIC_DRAGONSPAWN       = 10447,
    NPC_CHROMATIC_ELITE_GUARD       = 10814,
    NPC_CHROMATIC_WHELP             = 10442,
    NPC_ROOKERY_GUARDIAN            = 10258,
    NPC_ROOKERY_HATCHER             = 10683,
    NPC_ROOKERY_WHELP               = 10161,
    NPC_SCARSHIELD_INFILTRATOR      = 10299,
    NPC_SMOLDERTHORN_BERSERKER      = 9268,
    NPC_SPECTRAL_ASSASSIN           = 16066,
    NPC_SPIRE_SPIDERLING            = 10375,
    NPC_SPIRESTONE_WARLORD          = 9216,
    NPC_UROK_ENFORCER               = 10601,
    NPC_UROK_OGRE_MAGUS             = 10602
};

enum GameObjects
{
    GO_BRAZIER_OF_BECKONING         = 181045,
    GO_BRAZIER_OF_INVOCATION        = 181051,
    GO_UROKS_TRIBUTE_PILE           = 175621,
    GO_CHALLENGE_TO_UROK            = 175588,
    GO_CHALLENGE_TO_UROK_TRAP       = 175589,
    GO_DRAGONSPINE_HALL_RUNE_1      = 175197,
    GO_DRAGONSPINE_HALL_RUNE_2      = 175199,
    GO_DRAGONSPINE_HALL_RUNE_3      = 175195,
    GO_DRAGONSPINE_HALL_RUNE_4      = 175200,
    GO_DRAGONSPINE_HALL_RUNE_5      = 175198,
    GO_DRAGONSPINE_HALL_RUNE_6      = 175196,
    GO_DRAGONSPINE_HALL_RUNE_7      = 175194,
    GO_DRAGONSPINE_HALL_DOOR        = 175244,
    GO_EMBERSEER_DOOR_IN            = 175705,
    GO_EMBERSEER_DOOR_OUT           = 175153,
    GO_DARK_IRON_RUNE_1             = 175266,
    GO_DARK_IRON_RUNE_2             = 175267,
    GO_DARK_IRON_RUNE_3             = 175268,
    GO_DARK_IRON_RUNE_4             = 175269,
    GO_DARK_IRON_RUNE_5             = 175270,
    GO_DARK_IRON_RUNE_6             = 175271,
    GO_DARK_IRON_RUNE_7             = 175272,
    GO_ROOKERY_EGG                  = 175124,
    GO_STADIUM_PORTCULLIS           = 175185,
    GO_STADIUM_DOOR                 = 164726,
    GO_THE_FURNACE_DOOR             = 175186,
    GO_DRAKKISATH_GATE01            = 175946,
    GO_DRAKKISATH_GATE02            = 175947
};

enum Misc
{
    MAX_DRAGONSPINE_RUNE            = 7,
    FACTION_FRIENDLY_FOR_ALL        = 35,
    SAY_NEFARIAN_REND_END           = -1229015,
    SPELL_EMBERSEER_GROWING         = 16048
};

struct Loc
{
    float x, y, z;
};

class MANGOS_DLL_DECL instance_blackrock_spire : public ScriptedInstance
{
    public:
        instance_blackrock_spire(Map* pMap);
        ~instance_blackrock_spire() {}

        void Initialize();

        void OnPlayerEnter(Player* pPlayer);
        void OnPlayerLeave(Player* pPlayer);

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

        void SortDragonspineMobs();
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        bool m_bEmberseerGrowing;
        bool m_bStartUpLoader;

        uint32 m_uiEmberseerGrowingTimer;
        uint32 m_uiRookeryEggsTimer;
        uint32 m_uiStartUpLoaderTimer;

        ObjectGuid m_auiDragonspineRuneGUID[MAX_DRAGONSPINE_RUNE];

        GUIDList m_lDragonspineMobs;
        GUIDList m_lDarkIronRunes;
        GUIDList m_lIncarcerators;
        GUIDList m_lRookeryEggs;
        GUIDList m_lChromaticEliteGuards;
        GUIDList m_lTempList;
        GUIDList m_alRuneMobsGUID[MAX_DRAGONSPINE_RUNE];
};

#endif
