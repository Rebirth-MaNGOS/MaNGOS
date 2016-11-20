/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BRD_H
#define DEF_BRD_H

enum Data
{
    TYPE_RING_OF_LAW,
    TYPE_VAULT,
    TYPE_BAR,
    TYPE_BAR2,
    TYPE_BAR3,
    TYPE_BAR4,
    TYPE_BAR5,
    TYPE_TOMB_OF_SEVEN,
    TYPE_LYCEUM,
    TYPE_IRON_HALL,
    TYPE_IMPERIAL_SEAT,
    TYPE_CHAMBER_OF_ENCHANTMENT,
    TYPE_MANUFACTORY,
    TYPE_GIANT_DOOR,
    MAX_ENCOUNTER
};

enum Creatures
{
    // Bosses
    NPC_EMPEROR					        = 9019,
    NPC_PRINCESS				        = 8929,
    NPC_PHALANX					        = 9502,
    NPC_HATEREL					        = 9034,
    NPC_ANGERREL				        = 9035,
    NPC_VILEREL					        = 9036,
    NPC_GLOOMREL				        = 9037,
    NPC_SEETHREL				        = 9038,
    NPC_DOOMREL					        = 9039,
    NPC_DOPEREL				         	= 9040,
    NPC_FLAMELASH	                    = 9156,
    NPC_LORD_ROCCOR                     = 9025,
    NPC_PLUGGER_SPAZZRING               = 9499,
    NPC_LORD_ARGELMARCH                 = 8983,

    // Rare spawns
    NPC_PANZOR                          = 8923,

    // Special bosses
    // Ring of the Law (Theldren event)
    NPC_LEFTY                           = 16049,
    NPC_ROTFANG                         = 16050,
    NPC_SNOKH_BLACKSPINE                = 16051,
    NPC_MALGEN_LONGSPEAR                = 16052,
    NPC_KORV                            = 16053,
    NPC_REZZNIK                         = 16054,
    NPC_VAJASHNI                        = 16055,
    NPC_VOLIDA                          = 16058,
    NPC_THELDREN                        = 16059,
    NPC_GNASHJAW                        = 16095,        // Malgen Longspear's pet (Hyena)

    // Prisoners
    NPC_KHARAN_MIGHTHAMMER              = 9021,
    NPC_COMMANDER_GORSHAK               = 9020,
    NPC_MARSHAL_WINSDOR                 = 9023,
    NPC_MARSHAL_REGINALD_WINDSOR        = 9682,
    NPC_DUGHAL_STORMWING                = 9022,
    NPC_CREST_KILLER                    = 9680,
    NPC_TOBIAS_SEECHER                  = 9679,
    NPC_SHILL_DINGER                    = 9678,
    NPC_JAZ                             = 9681,
    NPC_OGRABISI                        = 9677,

    // Dark Keepers
    NPC_VORFALK                         = 9437,
    NPC_BETHEK                          = 9438,
    NPC_UGGEL                           = 9439,
    NPC_ZIMREL                          = 9441,
    NPC_OFGUT                           = 9442,
    NPC_PELVER                          = 9443,

    // Bar
    NPC_PRIVATE_ROCKNOT                 = 9503,
    NPC_MISTRESS_NAGMARA                = 9500,

    // Trash
    NPC_ANVILRAGE_GUARDSMAN             = 8891,
    NPC_ANVILRAGE_SOLDIER               = 8893,
    NPC_ANVILRAGE_MEDIC	                = 8894,
    NPC_ANVILRAGE_RESERVIST             = 8901,
    NPC_ANVILRAGE_OFFICER               = 8895,
    NPC_SHADOWFORGE_PEASANT             = 8896,
    NPC_SHADOWFORGE_CITIZEN             = 8902,
    NPC_SHADOWFORGE_SENATOR             = 8904,
    NPC_FIREGUARD_DESTROYER             = 8911,
    NPC_ARENA_SPECTATOR                 = 8916,
    NPC_GRIM_PATRON                     = 9545,
    NPC_GUZZLING_PATRON                 = 9547,
    NPC_HAMMERED_PATRON                 = 9554,
    NPC_WARBRINGER_CONSTRUCT            = 8905,
    NPC_BURNING_SPIRIT                  = 9178,
    NPC_DOOMFORGE_DRAGOON               = 8899,
    NPC_DOOMFORGE_ARCANESMITH           = 8900,
    NPC_WEAPON_TECHNICIAN               = 8920,
    NPC_RAGEREAVER_GOLEM                = 8906,
    NPC_DOOMGRIP                        = 9476,
    NPC_SHADOWFORGE_FLAMEKEEPER         = 9956,    

    // Triggers
    NPC_THELDREN_TRIGGER                = 16079,
};

enum GameObjects
{
    GO_CELL_DOOR__KHARAN                = 170565,
    GO_CELL_DOOR__GORSHAK               = 170564,
    GO_CELL_DOOR__WINDSOR               = 170563,
    GO_CELL_DOOR__DUGHAL                = 170562,
    GO_CELL_DOOR__CREST                 = 170567,
    GO_CELL_DOOR__TOBIAS                = 170566,
    GO_CELL_DOOR__SHILL                 = 170569,
    GO_CELL_DOOR__JAZ__OGRABISI         = 170568,
    GO_SUPPLY_ROOM_DOOR                 = 170561,
    GO_ARENA1					        = 161525,
    GO_ARENA2				            = 161522,
    GO_ARENA3					        = 161524,
    GO_ARENA4	                        = 161523,
    GO_TEMP_BRD_ARENA                   = 300065,
    GO_BANNER_OF_PROVOCATION            = 181058,
    GO_ARENA_SPOILS                     = 181074,
    GO_SHADOW_LOCK                      = 161460,
    GO_SHADOW_MECHANISM                 = 161461,
    GO_GIANT_DOOR_FAKE_COLLISION        = 161462,
    GO_SHADOW_GIANT_DOOR                = 157923,
    GO_SHADOW_DUMMY                     = 161516,
    GO_BAR_THUNDERBREW_LAGER            = 164911,
    GO_BAR_KEG_SHOT                     = 170607,
    GO_BAR_KEG_TRAP                     = 171941,
    GO_BAR_DOOR                         = 170571,
    GO_DARK_KEEPER_PORTRAIT             = 164819,
    GO_DARK_KEEPER_NAMEPLATE1           = 164820, // Vorfalk
    GO_DARK_KEEPER_NAMEPLATE2           = 164821, // Bethek
    GO_DARK_KEEPER_NAMEPLATE3           = 164822, // Uggel
    GO_DARK_KEEPER_NAMEPLATE4           = 164823, // Zimrel
    GO_DARK_KEEPER_NAMEPLATE5           = 164824, // Ofgut
    GO_DARK_KEEPER_NAMEPLATE6           = 164825, // Pelver
    GO_RELIC_COFFER_DOOR_01             = 174554,
    GO_RELIC_COFFER_DOOR_02             = 174555,
    GO_RELIC_COFFER_DOOR_03             = 174556,
    GO_RELIC_COFFER_DOOR_04             = 174557,
    GO_RELIC_COFFER_DOOR_05             = 174558,
    GO_RELIC_COFFER_DOOR_11             = 174559,
    GO_RELIC_COFFER_DOOR_06             = 174560,
    GO_RELIC_COFFER_DOOR_07             = 174561,
    GO_RELIC_COFFER_DOOR_08             = 174562,
    GO_RELIC_COFFER_DOOR_09             = 174563,
    GO_RELIC_COFFER_DOOR_10             = 174564,
    GO_RELIC_COFFER_DOOR_12             = 174566,
    GO_DARK_COFFER                      = 174565,
    GO_SECRET_DOOR                      = 174553,
    GO_DARK_IRON_DWARF_RUNE_A01         = 170578,
    GO_DARK_IRON_DWARF_RUNE_B01         = 170579,
    GO_DARK_IRON_DWARF_RUNE_C01         = 170580,
    GO_DARK_IRON_DWARF_RUNE_D01         = 170581,
    GO_DARK_IRON_DWARF_RUNE_E01         = 170582,
    GO_DARK_IRON_DWARF_RUNE_F01         = 170583,
    GO_DARK_IRON_DWARF_RUNE_G01         = 170584,
    GO_TOMB_ENTER                       = 170576,
    GO_TOMB_EXIT                        = 170577,
    GO_LYCEUM                           = 170558,
    GO_SHADOWFORGE_BRAZIER_N            = 174745,
    GO_SHADOWFORGE_BRAZIER_S            = 174744,
    GO_GOLEM_ROOM_N                     = 170573,
    GO_GOLEM_ROOM_S                     = 170574,
    GO_THRONE_ROOM                      = 170575,

    GO_SPECTRAL_CHALICE                 = 164869,
    GO_CHEST_SEVEN                      = 169243,
    
    GO_RELIC_COFFER			= 160836,
    GO_SECRET_SAFE			= 161495
};

enum Misc
{
    FACTION_FRIENDLY_FOR_ALL            = 35,
    FACTION_NEUTRAL_TO_ALL              = 7,
    FACTION_ENEMY                       = 54,
    FACTION_ENEMY_TO_ALL                = 14,
};

enum Yell
{
    // Emperor Dagran Thaurissan
    YELL_SENATOR_1         = -1230060,
    YELL_SENATOR_2         = -1230061,
    YELL_SENATOR_3         = -1230062,
    YELL_SENATOR_4         = -1230063
};

class MANGOS_DLL_DECL instance_blackrock_depths : public ScriptedInstance
{
public:
    instance_blackrock_depths(Map* pMap);
    ~instance_blackrock_depths() {}

    void Initialize();

    void OnPlayerEnter(Player* pPlayer);
    void OnPlayerLeave(Player* pPlayer);

    void OnCreatureCreate(Creature* pCreature);
    void OnCreatureDeath(Creature* pCreature);
    void OnObjectCreate(GameObject* pGo);

    void SetData(uint32 uiType, uint32 uiData);
    uint32 GetData(uint32 uiType);

    const char* Save() {
        return strInstData.c_str();
    }
    void Load(const char* chrIn);

    void Update(uint32 uiDiff);

    void DoUseFlamelashRunes();
    void DoAggroManufactory();
    void DoOpenBarDoor();
    void DoOpenToumbEntrance();

	bool m_bBridgeEventDone;

    GameObject* GetClosestRelicCoffer(GameObject* pObj);

protected:
    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    bool m_bBannerSpawned;

    bool m_bRelicCoffersRemoved;
    
    uint32 m_uiBarAleCount;
    uint32 m_uiBarrelsDestroyedCount;
    uint32 m_uiCoffersOpenedCount;
    uint32 m_uiDoomgripsBandDeadCount;
    uint32 m_uiRingOfTheLawSpecsTimer;

    GUIDList m_uiRingSpectators;
    GUIDList m_uiBarPatrons;
    GUIDList m_uiLyceumCreatures;
    GUIDList m_uiBurningSpirit;
    GUIDList m_uiManufactoryCreatures;
    GUIDList m_uiFlamelashRunes;

    std::vector<GameObject*> m_uiRelicCoffers;
    GameObject* m_goSecretSafe;
    uint8 m_uiOpenCofferDoors;
};

#endif
