/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_STRATHOLME_H
#define DEF_STRATHOLME_H

enum Data
{
    TYPE_BARON_RUN,
    TYPE_MAGISTRATE,
    TYPE_BARONESS_ANASTARI,
    TYPE_NERUBENKAN,
    TYPE_MALEKI_THE_PALLID,
    TYPE_SLAUGHTER_SQUARE,
    TYPE_BARON_RIVENDARE,
    TYPE_JARIEN,
    TYPE_SOTHOS,
    TYPE_POSTBOX,
    TYPE_TIMMY_THE_CRUEL,
    TYPE_THE_UNFORGIVEN,
    TYPE_AURIUS,
	MAX_ENCOUNTER
};

enum Actions
{
    ACTION_RUN_START,
    ACTION_RUN_ENCOUNTER,
    ACTION_RUN_COMPLETE,
    ACTION_AURIUS
};

enum Creatures
{
    // Bosses
    NPC_BARON_RIVENDARE         = 10440,
    NPC_MAGISTRATE_BARTHILAS    = 10435,
    NPC_RAMSTEIN                = 10439,
    NPC_POSTMASTER_MALOWN       = 11143,
    NPC_TIMMY_THE_CRUEL         = 10808,
    NPC_THE_UNFORGIVEN          = 10516,
    NPC_DATHROHAN               = 10812,
    NPC_BALNAZZAR               = 10813,
    NPC_JARIEN                  = 16101,
    NPC_SOTHOS                  = 16102,

    // Trash
    NPC_ASHARI_CRYSTAL          = 10415,
    NPC_BILE_SLIME              = 10697,
    NPC_BILE_SPEWER             = 10416,    
    NPC_VENOM_BELCHER           = 10417,
    NPC_BLACK_GUARD_SENTRY      = 10394,
    NPC_CRIMSON_HAMMERSMITH     = 11120,
    NPC_BLACK_GUARD_SWORDSMITH  = 11121,
    NPC_CRIMSON_CONJUROR        = 10419,
    NPC_CRIMSON_INITIATE        = 10420,
    NPC_CRIMSON_GALLANT         = 10424,
    NPC_CRIMSON_GUARDSMAN       = 10418,
    NPC_CRIMSON_RIFLEMAN        = 11054,
    NPC_MINDLESS_UNDEAD         = 11030,
    NPC_MINDLESS_SKELETON       = 11197,
    NPC_PLAGUED_INSECT          = 10461,
    NPC_PLAGUED_MAGGOT          = 10536,
    NPC_PLAGUED_RAT             = 10441,
    NPC_SKELETAL_GUARDIAN       = 10390,
    NPC_SKELETAL_BERSERKER      = 10391,
    NPC_UNDEAD_POSTMAN          = 11142,
    NPC_UNDEAD_SCARAB           = 10876,
    NPC_THUZADIN_ACOLYTE        = 10399,
    NPC_VENGEFUL_PHANTOM        = 10387,

    // Npcs
    NPC_AURIUS                  = 10917,
    NPC_SPIRIT_OF_JARIEN        = 16103,
    NPC_YSIDA                   = 16031,
    NPC_YSIDA_TRIGGER           = 16100,
};

enum GameObjects
{
    GO_SERVICE_ENTRANCE         = 175368,
    GO_GAUNTLET_INNER_GATE      = 175356,
    GO_GAUNTLET_OUTER_GATE      = 175357,         // Starts baron Run
    GO_ZIGGURAT_DOOR_1          = 175380,         // Baroness
    GO_ZIGGURAT_DOOR_2          = 175379,         // Nerub'enkan
    GO_ZIGGURAT_DOOR_3          = 175381,         // Maleki The Pallid
    GO_ZIGGURAT_DOOR_4          = 175405,         // Rammstein
    GO_ZIGGURAT_DOOR_5          = 175796,         // Baron
    GO_PORT_GAUNTLET            = 175374,         // Port from gauntlet to slaugther
    GO_PORT_SLAUGTHER           = 175373,         // Port at slaugther
    GO_PORT_TRAP_SQUARE1        = 175350,
    GO_PORT_TRAP_SQUARE2        = 175351,
    GO_PORT_TRAP_GAUNTLET1      = 175354,
    GO_PORT_TRAP_GAUNTLET2      = 175355,
    GO_PORT_ELDERS              = 175377,         // Port at elders square
    GO_BRAZIER_OF_BECKONING     = 181045,
    GO_BRAZIER_OF_INVOCATION    = 181051,
    GO_HEIRLOOMS                = 181083,         // Jarien & Sothos heirlooms
	GO_YSIDAS_CAGE				= 181071
};

enum Misc
{
    MAX_ZIGGURAT                = 3,
    QUEST_AURIUS_RECKONING      = 5125,
    QUEST_DEAD_MAN_PLEA         = 8945,
    SPELL_BARON_ULTIMATUM       = 27861,
	SPELL_BARON_RUN_REP			= 27773
};

enum ZoneYells
{
    SAY_RUN_START               = -1329000,
    SAY_RUN_10_UP               = -1329001,
    SAY_RUN_5_UP                = -1329002,
    SAY_RUN_TIME_UP             = -1329003,
    SAY_ASHARI_FALLEN           = -1329004,
    SAY_SQUARE_CLEARED_OUT      = -1329005,
    SAY_RAMMSTEIN_DEATH         = -1329006,
    SAY_ANNOUNCE_ZIGGURAT_1     = -1329007,
    SAY_ANNOUNCE_ZIGGURAT_2     = -1329008,
    SAY_ANNOUNCE_ZIGGURAT_3     = -1329009
};

struct Locations
{
    float x, y, z;
    uint32 add1, add2;                            // Traps: add1, add2 bars entries
};

struct MobInfo
{
    MobInfo(const ObjectGuid _guid, float _dist)
        : guid(_guid), dist(_dist) {}

    ObjectGuid guid;
    float dist;
};



typedef std::list<MobInfo> MobInfoList;

class MANGOS_DLL_DECL instance_stratholme : public ScriptedInstance
{
    public:
        instance_stratholme(Map* pMap);
        ~instance_stratholme() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

        void DoSortZiggurats();
        void FillAbomPullList();
        void OpenSlaugtherSquare();
        void PlayersRun(uint32 uiAction);

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        bool m_bFiveUp;
        bool m_bTenUp;
        bool m_bAbomPull;
        bool m_bBaronRun;
        bool m_bBaronWarn;
        bool m_bZigguratDoor;

        uint32 m_uiAbCount;
        uint32 m_uiNextPull;
        uint32 m_uiAbomPullTimer;
        uint32 m_uiBaronRunTimer;
        uint32 m_uiBaronWarnTimer;
        uint32 m_uiZigguratDoorTimer;

        ObjectGuid m_auiCrystalSortedGUID[MAX_ZIGGURAT];
        ObjectGuid m_auiZigguratDoorGUID[MAX_ZIGGURAT];

        MobInfoList m_AbominationPull;

        GUIDList m_lAbominationGUID;
        GUIDList m_lAcolyteGUID;
        GUIDList m_lCrystalGUID;
        GUIDList m_lSentryGUID;
        GUIDList m_alZigguratAcolyteGUID[MAX_ZIGGURAT];
};

#endif
