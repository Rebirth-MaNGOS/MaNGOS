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
    ACTION_RUN_COMPLETE
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
    NPC_CRIMSON_MONK            = 11043,
    NPC_CRIMSON_SORCERER        = 10422,
    NPC_CRIMSON_BATTLE_MAGE     = 10425,
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
	SAY_WARN_BARON				= -1329020,
    SAY_RUN_START               = -1329000,
    SAY_RUN_10_UP               = -1329001,
    SAY_RUN_5_UP                = -1329002,
    SAY_RUN_TIME_UP             = -1329003,
    SAY_ASHARI_FALLEN           = -1329004,
    SAY_SQUARE_CLEARED_OUT      = -1329005,
    SAY_RAMMSTEIN_DEATH         = -1329006,
    SAY_ANNOUNCE_ZIGGURAT_1     = -1329007,
    SAY_ANNOUNCE_ZIGGURAT_2     = -1329008,
    SAY_ANNOUNCE_ZIGGURAT_3     = -1329009,
    
    // Timmy
    SAY_SPAWN_TIMMY         = -1329012,
    
    // Used in the Scarlet Bastion defense events
    YELL_BASTION_BARRICADE      = -1329023,
    YELL_BASTION_STAIRS         = -1329024,
    YELL_BASTION_ENTRANCE       = -1329025,
    YELL_BASTION_HALL_LIGHTS    = -1329026,
    YELL_BASTION_INNER_1        = -1329027,
    YELL_BASTION_INNER_2        = -1329028,    
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

struct EventLocation
{
    float m_fX, m_fY, m_fZ, m_fO;
};

static const EventLocation aStratholmeLocation[] =
{
    {3725.577f, -3599.484f, 142.367f, 0},                      // Barthilas door run
    {4068.284f, -3535.678f, 122.771f, 2.50f},               // Barthilas tele
    {4032.643f, -3378.546f, 119.752f, 4.74f},               // Ramstein summon loc
    {4032.843f, -3390.246f, 119.732f, 0},                      // Ramstein move loc
    {3969.357f, -3391.871f, 119.116f, 5.91f},               // Skeletons summon loc
    {4033.044f, -3431.031f, 119.055f, 0},                      // Skeletons move loc
    {4032.602f, -3378.506f, 119.752f, 4.74f},               // Guards summon loc
    {4044.78f,  -3333.68f,  115.53f,  4.15f},               // Ysida summon loc
    {4041.9f,   -3337.6f,   115.06f,  3.82f}                // Ysida move/death loc
};

// Scarlet Bastion defense events
enum
{
    BARRICADE           = 0,
    STAIRS              = 1,
    TIMMY               = 2,
    ENTRANCE            = 3,
    HALL_OF_LIGHTS      = 4,
    INNER_BASTION_1     = 5,
    INNER_BASTION_2     = 6,
    CRIMSON_THRONE      = 7,
    FIRST_BARRICADES    = 7,

    MAX_DEFENSE_POINTS  = 8
};

static const int ScarletEventYells[] =
{
    YELL_BASTION_BARRICADE, YELL_BASTION_STAIRS, 0, YELL_BASTION_ENTRANCE,
    YELL_BASTION_HALL_LIGHTS, YELL_BASTION_INNER_1, YELL_BASTION_INNER_2
};

static const EventLocation aDefensePoints[] =
{
    {3662.84f, -3175.15f, 126.559f, 0},                        // Last barricade before the Scarlet Bastion (Courtyard)
    {3661.57f, -3157.80f, 128.945f, 0},                        // Scarlet Bastion stairs (Courtyard)
    {3668.60f, -3183.31f, 126.215f, 0},                        // Courtyard mobs sort point (Timmy)
    {3646.49f, -3072.84f, 134.207f, 0},                        // Scarlet Bastion entrance corridor
    {3599.28f, -3107.91f, 134.204f, 0},                        // Hall of Lights
    {3485.98f, -3087.02f, 135.080f, 0},                        // Inner Bastion: first corridor
    {3436.74f, -3090.19f, 135.085f, 0},                        // Inner Bastion: Second corridor
    {3661.89f, -3192.89f, 126.691f, 0}                         // Barricades before the last one (Courtyard): in last position for conveniently iterate over the table
};

static const EventLocation aScarletGuards[] =               // Spawned when players cleared some parts of the Bastion
{
    {3577.469f, -3064.498f, 135.667f, 5.3425f},             // Hall of Lights
    {3602.198f, -3096.243f, 134.120f, 5.3425},
    {3598.152f, -3098.927f, 134.120f, 5.3425},
    {3432.967f, -3069.643f, 136.529f, 5.3425f},             // Inner Bastion
    {3441.795f, -3077.431f, 135.000f, 5.3425},
    {3437.445f, -3080.316f, 135.000f, 5.3425}
};

static const EventLocation aScourgeInvaders[] =             // Attack when players cleared some parts of the Bastion
{
    {3614.702f, -3187.642f, 131.406f, 4.024f},              // Timmy. TIMMYYYYYYY !!!
    {3647.36f, -3139.70f, 134.78f, 2.1962f},                // Entrance
    {3642.50f, -3106.24f, 134.18f, 0.5210f},
    {3547.85f, -3076.46f, 135.00f, 2.2269f},                // Inner Bastion
    {3512.21f, -3066.85f, 135.00f, 3.7136f},
    {3492.44f, -3077.72f, 135.00f, 2.1680f},                // Crimson Throne
    {3443.18f, -3083.90f, 135.01f, 2.1563f}
};

static const EventLocation aScarletLastStand[] =            // Positions remaining Scarlet defenders will move to if the position is lost
{
    {3658.43f, -3178.07f, 126.696f, 5.23599f},              // Last barricade
    {3665.62f, -3173.88f, 126.551f, 5.02655f},
    {3662.84f, -3175.15f, 126.559f, 5.11381f},
    {3656.91f, -3161.94f, 128.359f, 5.39307f},              // Bastion stairs
    {3664.29f, -3157.06f, 128.357f, 5.18363f},
    {3661.57f, -3157.80f, 128.945f, 5.23599f}
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

        bool m_bSummonAurius;
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;
        
        void DoSpawnScarletGuards(uint8 uiStep, Player* pSummoner);
        void DoSpawnScourgeInvaders(uint8 uiStep, Player* pSummoner);
        void DoMoveBackDefenders(uint8 uiStep, Creature* pCreature);
        void DoScarletBastionDefense(uint8 uiStep, Creature* pCreature);

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

        GUIDList m_suiCrimsonDefendersLowGuids[MAX_DEFENSE_POINTS];
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
