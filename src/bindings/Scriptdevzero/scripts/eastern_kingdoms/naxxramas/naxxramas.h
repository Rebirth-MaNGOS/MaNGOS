/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_NAXXRAMAS_H
#define DEF_NAXXRAMAS_H

enum
{
    MAX_ENCOUNTER               = 15,

    // Kel'Thuzad
    SAY_KELTHUZAD_CAT_DIED      = -1533089,
    
    TYPE_ANUB_REKHAN            = 1,
    TYPE_FAERLINA               = 2,
    TYPE_MAEXXNA                = 3,

    TYPE_NOTH                   = 4,
    TYPE_HEIGAN                 = 5,
    TYPE_LOATHEB                = 6,

    TYPE_RAZUVIOUS              = 7,
    TYPE_GOTHIK                 = 8,
    TYPE_FOUR_HORSEMEN          = 9,

    TYPE_PATCHWERK              = 10,
    TYPE_GROBBULUS              = 11,
    TYPE_GLUTH                  = 12,
    TYPE_THADDIUS               = 13,
    TYPE_STALAGG                = 14,
    TYPE_FEUGEN                 = 15,

    TYPE_SAPPHIRON              = 16,
    TYPE_KELTHUZAD              = 17,

    MAX_HEIGAN_TRAP_AREAS       = 4,
    
    NPC_ANUB_REKHAN             = 15956,
    NPC_FAERLINA                = 15953,

    NPC_THADDIUS                = 15928,
    NPC_STALAGG                 = 15929,
    NPC_FEUGEN                  = 15930,
    NPC_TESLA_COIL          = 16218,

    NPC_ZELIEK                  = 16063,
    NPC_THANE                   = 16064,
    NPC_BLAUMEUX                = 16065,
    NPC_MOGRAINE              = 16062,
    
    // horseman spirits
    NPC_SPIRIT_OF_BLAUMEUX    = 16776,
    NPC_SPIRIT_OF_MOGRAINE   = 16775,
    NPC_SPIRIT_OF_KORTHAZZ    = 16778,
    NPC_SPIRIT_OF_ZELIREK     = 16777,
    
    NPC_SAPPHIRON               = 15989,
    NPC_KELTHUZAD               = 15990,
    NPC_MR_BIGGLESWORTH         = 16998,
    NPC_LIVING_POISON = 16027,

    // Gothik
    NPC_GOTHIK                  = 16060,
    NPC_SUB_BOSS_TRIGGER        = 16137,                    //summon locations
    NPC_UNREL_TRAINEE           = 16124,
    NPC_UNREL_DEATH_KNIGHT      = 16125,
    NPC_UNREL_RIDER             = 16126,
    NPC_SPECT_TRAINEE           = 16127,
    NPC_SPECT_DEATH_KNIGHT      = 16148,
    NPC_SPECT_RIDER             = 16150,
    NPC_SPECT_HORSE             = 16149,
    
    // End boss adds
    NPC_SOLDIER_FROZEN          = 16427,
    NPC_UNSTOPPABLE_ABOM        = 16428,
    NPC_SOUL_WEAVER             = 16429,
    NPC_GUARDIAN                = 16441,

    // Arachnid Quarter
    GO_ARAC_ANUB_DOOR           = 181126,                   //encounter door
    GO_ARAC_ANUB_GATE           = 181195,                   //open after boss is dead
    GO_ARAC_FAER_WEB            = 181235,                   //encounter door
    GO_ARAC_FAER_DOOR           = 194022,                   //after faerlina, to outer ring
    GO_ARAC_MAEX_INNER_DOOR     = 181197,                   //encounter door
    GO_ARAC_MAEX_OUTER_DOOR     = 181209,                   //right before maex

    // Plague Quarter
    GO_PLAG_SLIME01_DOOR        = 181198,                   //not used
    GO_PLAG_SLIME02_DOOR        = 181199,                   //not used
    GO_PLAG_NOTH_ENTRY_DOOR     = 181200,                   //encounter door
    GO_PLAG_NOTH_EXIT_DOOR      = 181201,                   //exit, open when boss dead
    GO_PLAG_HEIG_ENTRY_DOOR     = 181202,
    GO_PLAG_HEIG_EXIT_DOOR      = 181203,                   //exit, open when boss dead
    GO_PLAG_LOAT_DOOR           = 181241,                   //encounter door

    // Military Quarter
    GO_MILI_GOTH_ENTRY_GATE     = 181124,                   //open after razuvious died
    GO_MILI_GOTH_EXIT_GATE      = 181125,                   //exit, open at boss dead
    GO_MILI_GOTH_COMBAT_GATE    = 181170,                   //used while encounter is in progress
    GO_MILI_HORSEMEN_DOOR       = 181119,                   //encounter door

    GO_CHEST_HORSEMEN_NORM      = 181366,                   //four horsemen event, DoRespawnGameObject() when event == DONE
    GO_CHEST_HORSEMEN_HERO      = 193426,

    // Construct Quarter
    GO_CONS_PATH_EXIT_DOOR      = 181123,
    GO_CONS_GLUT_EXIT_DOOR      = 181120,
    GO_CONS_THAD_DOOR           = 181121,                   // Thaddius enc door
    GO_CONS_NOX_TESLA_FEUGEN    = 181477,
    GO_CONS_NOX_TESLA_STALAGG   = 181478,


    // Frostwyrm Lair
    GO_KELTHUZAD_WATERFALL_DOOR = 181225,                   // exit, open after sapphiron is dead
    GO_KELTHUZAD_EXIT_DOOR      = 181228,
    GO_KELTHUZAD_WINDOW_1       = 181402,
    GO_KELTHUZAD_WINDOW_2       = 181403,
    GO_KELTHUZAD_WINDOW_3       = 181404,
    GO_KELTHUZAD_WINDOW_4       = 181405,


    // Eyes
    GO_ARAC_EYE_RAMP            = 181212,
    GO_PLAG_EYE_RAMP            = 181211,
    GO_MILI_EYE_RAMP            = 181210,
    GO_CONS_EYE_RAMP            = 181213,

    // Portals
    GO_ARAC_PORTAL              = 181575,
    GO_PLAG_PORTAL              = 181577,
    GO_MILI_PORTAL              = 181578,
    GO_CONS_PORTAL              = 181576,

    AREATRIGGER_FROSTWYRM       = 4120,                    //not needed here, but AT to be scripted
    AREATRIGGER_KELTHUZAD       = 4112,
    AREATRIGGER_GOTHIK          = 4116
};

struct GothTrigger
{
    bool bIsRightSide;
    bool bIsAnchorHigh;
};

struct Loc
{
    float x, y, z, o;
};

static const float aSapphPositions[4] = {3521.48f, -5234.87f, 137.626f, 4.53329f};

// Used in Construct Quarter for the deadly blobs continuously spawning in Patcherk corridor
static const Loc aLivingPoisonPositions[6] =
{
    {3128.692f, -3119.211f, 293.346f, 4.725505f},
    {3154.432f, -3125.669f, 293.408f, 4.456693f},
    {3175.614f, -3134.716f, 293.282f, 4.244928f},
    {3128.709f, -3157.404f, 293.3238f, 4.725505f},
    {3145.881f, -3158.563f, 293.3216f, 4.456693f},
    {3157.736f, -3164.859f, 293.2874f, 4.244928f},
};

class MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
    public:
        instance_naxxramas(Map* pMap);
        ~instance_naxxramas() {}

        void Initialize();

        bool IsEncounterInProgress() const;

        void OnPlayerEnter(Player* pPlayer);
        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);
        
        void OnCreatureDeath(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);
        
        // Heigan
        void DoTriggerHeiganTraps(Creature* pHeigan, uint32 uiAreaIndex);
        
        // goth
        void SetGothTriggers();
        Creature* GetClosestAnchorForGoth(Creature* pSource, bool bRightSide);
        void GetGothSummonPointCreatures(std::list<Creature*> &lList, bool bRightSide);
        bool IsInRightSideGothArea(Unit* pUnit);

        // thaddius
        void GetThadTeslaCreatures(GUIDList& lList) { lList = m_lThadTeslaCoilList; };
        
        // kel
        void SetChamberCenterCoords(float fX, float fY, float fZ);
        void GetChamberCenterCoords(float &fX, float &fY, float &fZ) { fX = m_fChamberCenterX; fY = m_fChamberCenterY; fZ = m_fChamberCenterZ; }

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;
        
        GUIDList m_lThadTeslaCoilList;

        GUIDList m_lGothTriggerList;
        UNORDERED_MAP<ObjectGuid, GothTrigger> m_mGothTriggerMap;
        
        GUIDList m_alHeiganTrapGuids[MAX_HEIGAN_TRAP_AREAS];
        
        float m_fChamberCenterX;
        float m_fChamberCenterY;
        float m_fChamberCenterZ;
        
        uint32 m_uiSapphSpawnTimer;
        uint32 m_uiLivingPoisonTimer;
};

#endif
