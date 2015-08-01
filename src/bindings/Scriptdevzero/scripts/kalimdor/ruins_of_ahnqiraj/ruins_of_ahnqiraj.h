/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_RUINS_OF_AHNQIRAJ_H
#define DEF_RUINS_OF_AHNQIRAJ_H

enum Data
{
    TYPE_KURINNAXX,
    TYPE_RAJAXX,
    TYPE_MOAM,
    TYPE_BURU,
    TYPE_AYAMISS,
    TYPE_OSSIRIAN,
	MAX_ENCOUNTER
};

enum Creatures
{
    NPC_KURINNAXX                 = 15348,
    NPC_RAJAXX                    = 15341,
    NPC_GENERAL_ANDOROV           = 15471,
    NPC_COLONEL_ZERRAN            = 15385,
    NPC_MAJOR_PAKKON              = 15388,
    NPC_MAJOR_YEGGETH             = 15386,
    NPC_CAPTAIN_XURREM            = 15390,
    NPC_CAPTAIN_DRENN             = 15389,
    NPC_CAPTAIN_TUUBID            = 15392,
    NPC_CAPTAIN_QEEZ              = 15391,
    NPC_KALDOREI_ELITE            = 15473,
    NPC_MOAM                      = 15340,
    NPC_BURU                      = 15370,
    NPC_AYAMISS                   = 15369,
    NPC_OSSIRIAN                  = 15339,
    
    // Trash and Adds
    NPC_SWARMGUARD_NEEDLER        = 15344,
    NPC_QIRAJI_WARRIOR            = 15387,
    NPC_MANA_FIEND                = 15527,
    NPC_HIVEZARA_WASP             = 15325,
    NPC_HIVEZARA_SWARMER          = 15546,
	NPC_HIVEZARA_SWARMER_1		  = 155460,
    NPC_HIVEZARA_LARVA            = 15555,
    NPC_ANUB_WARRIOR              = 15537,
    NPC_ANUB_SWARM                = 15538,
    NPC_AHNQIRAJ_TRIGGER          = 15426,
    NPC_BURU_EGG                  = 15514,
    NPC_BURU_EGG_TRIGGER          = 15964,
    NPC_HIVEZARA_HATCHLING        = 15521,
    NPC_OSSIRIAN_CRYSTAL_TRIGGER  = 15590,
    NPC_TORNADO                   = 15428,
};

enum GameObjects
{
    GO_SAND_TRAP                  = 180647,
    GO_OSSIRIAN_CRYSTAL           = 180619,
};

enum Misc
{
    AREAID_WATCHERS_TERRACE       = 3453,
    FACTION_CENARION_CIRCLE       = 609,
    YELL_OSSIRIAN_BREACHED        = -1509022,
};

struct Loc
{
    float x, y, z, o;
    uint32 id;
};

// For the NPCs at Kurinaxx.
static Loc NPCs[]=
{
    {-8871.37f, 1650.34f, 21.38f, 5.49f, NPC_KALDOREI_ELITE},
    {-8872.51f, 1648.88f, 21.38f, 5.62f, NPC_KALDOREI_ELITE},   
    {-8874.36f, 1646.08f, 21.38f, 5.69f, NPC_KALDOREI_ELITE},
    {-8875.29f, 1644.89f, 21.38f, 5.69f, NPC_KALDOREI_ELITE},
    {-8873.42f, 1647.67f, 21.38f, 5.69f, NPC_GENERAL_ANDOROV},
};

class MANGOS_DLL_DECL instance_ruins_of_ahnqiraj : public ScriptedInstance
{
    public:
        instance_ruins_of_ahnqiraj(Map* pMap);
        ~instance_ruins_of_ahnqiraj() {}

        void Initialize();

		bool IsEncounterInProgress() const;

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);
		GUIDList& GetBuruEggs()
		{
			return m_lBuruEggs;
		}
		GUIDList& GetTornadoes()
		{
			return m_lTornadoes;
		}
		GUIDList m_lBuruEggs;
		GUIDList m_lTornadoes;
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;		
};

#endif
