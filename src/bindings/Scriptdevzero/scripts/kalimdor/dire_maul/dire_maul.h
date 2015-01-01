/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_DIREMAUL_H
#define DEF_DIREMAUL_H

enum Data
{
    // East
    TYPE_PUSILLIN,
    TYPE_ZEVRIM_THORNHOOF,
    TYPE_IRONBARK_THE_REDEEMED,
    TYPE_ALZZIN_THE_WILDSHAPER,
    TYPE_ISALIEN,

    // North
    TYPE_STOMPER_KREEG,
    TYPE_GUARD_MOLDAR,
    TYPE_GUARD_FENGUS,
    TYPE_GUARD_SLIPKIK,
    TYPE_CAPTAIN_KROMCRUSH,
    TYPE_CHORUSH_THE_OBSERVER,
    TYPE_KING_GORDOK,
    TYPE_TRIBUTE_RUN,
    TYPE_KNOT_THIMBLEJACK,

    // West
    TYPE_TENDRIS_WARPWOOD,
    TYPE_BARRIER,
    TYPE_IMMOLTHAR,
    TYPE_PRINCE_TORTHELDRIN,
	MAX_ENCOUNTER
};

enum Creatures
{
    // East
    NPC_PUSILLIN                = 14354,
    NPC_OLD_IRONBARK            = 11491,
    NPC_IRONBARK_THE_REDEEMED   = 14241,
    NPC_WILDSPAWN_IMP           = 13276,
    NPC_ZEVRIM_THORNHOOF        = 11490,
    NPC_HYDROSPAWN              = 13280,
    NPC_MASSIVE_GEYSER          = 14122,
    NPC_HYDROLING               = 14350,
    NPC_LETHTENDRIS             = 14327,
    NPC_PIMGIB                  = 14349,
    NPC_ALZZINS_THE_WILDSHAPER  = 11492,
    NPC_ALZZINS_MINION          = 11460,
    NPC_ISALIEN                 = 16097,
    NPC_EMPYREAN                = 16098,

    // North
    NPC_STOMPER_KREEG           = 14322,
    NPC_GUARD_MOLDAR            = 14326,
    NPC_GUARD_FENGUS            = 14321,
    NPC_GUARD_SLIPKIK           = 14323,
    NPC_CAPTAIN_KROMCRUSH       = 14325,
    NPC_KING_GORDOK             = 11501,
    NPC_CHORUSH_THE_OBSERVER    = 14324,
    NPC_KNOT_THIMBLEJACK        = 14338,
    NPC_MIZZLE_THE_CRAFTY       = 14353,
    NPC_GORDOK_BRUTE            = 11441,
    NPC_GORDOK_MAULER           = 11442,
    NPC_GORDOK_MAGELORD         = 11444,
    NPC_GORDOK_CAPTAIN          = 11445,
    NPC_GORDOK_SPIRIT           = 11446,
    NPC_GORDOK_WARLOCK          = 11448,
    NPC_GORDOK_REAVER           = 11450,
    NPC_GORDOK_BUSHWACKER       = 14351,
    NPC_GORDOK_MASTIFF          = 13036,

    // West
    NPC_TENDRIS_WARPWOOD        = 11489,
    NPC_ANCIENT_EQUINE_SPIRIT   = 14566,
    NPC_PROTECTOR               = 11459,
    NPC_IMMOLTHAR               = 11496,
    NPC_EYE_OF_IMMOLTHAR        = 14396,
    NPC_PRINCE_TORTHELDRIN      = 11486,
    NPC_ARCANE_ABERRATION       = 11480,
    NPC_MANA_REMNANT            = 11483,
};

enum GameObjects
{
    // East
    GO_CONSERVATORY_DOOR        = 176907,
    GO_CRUMBLE_WALL             = 177220,
    GO_CORRUPTED_CRYSTAL_VINE   = 179502,
    GO_FELVINE_SHARD            = 179559,
    GO_BRAZIER_OF_BECKONING     = 181045,
    GO_BRAZIER_OF_INVOCATION    = 181051,

    // North
    GO_GORDOK_COURTYARD_DOOR    = 177219,
    GO_GORDOK_INNER_DOOR        = 177217,
    GO_GORDOK_TRIBUTE1          = 179564,
	GO_GORDOK_TRIBUTE2          = 300250,
	GO_GORDOK_TRIBUTE3          = 300251,
	GO_GORDOK_TRIBUTE4          = 300252,
	GO_GORDOK_TRIBUTE5          = 300253,
    GO_KNOT_THIMBLEJACK_CACHE   = 179501,
	GO_BROKEN_TRAP				= 179485,

    // West
    GO_PYLON_1                  = 177257,
    GO_PYLON_2                  = 177258,
    GO_PYLON_3                  = 177259,
    GO_PYLON_4                  = 179504,
    GO_PYLON_5                  = 179505,
    GO_FORCE_FIELD              = 179503,
    GO_THE_PRINCES_CHEST        = 179545,
};

enum Misc
{
    FACTION_FRIENDLY            = 35,
    FACTION_HOSTILE             = 14,
    SAY_REDEEMED                = -1429005,
    SPELL_KING_OF_GORDOK        = 22799,
};

struct Loc
{
    float x, y, z, o;
};

class MANGOS_DLL_DECL instance_dire_maul : public ScriptedInstance
{
    public:
        instance_dire_maul(Map* pMap);
        ~instance_dire_maul() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
		void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		void CallProtectors();
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		uint32 m_uiPylonCount;

		GUIDList m_uiAlzzinsMinionGUID;
		GUIDList m_uiGordokGUID;
		GUIDList m_uiGordokTributeGUID;
		GUIDList m_uiProtectorGUID;
		GUIDList m_uiShardGUID;
		GUIDList m_uiPyramideTrash;
};

#endif
