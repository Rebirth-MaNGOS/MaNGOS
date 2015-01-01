/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_SCARLETM_H
#define DEF_SCARLETM_H

enum Data
{
    TYPE_MOGRAINE_AND_WHITE_EVENT,
    TYPE_ASHBRINGER_EVENT,
    TYPE_ASH_WELCOME_YELL,
    MAX_ENCOUNTER,
};

enum Creatures
{
    // Boss
    NPC_MOGRAINE                    = 3976,
    NPC_WHITEMANE                   = 3977,
    NPC_FAIRBANKS                   = 4542,

    // Rare
    NPC_FALLEN_CHAMPION             = 6488,
    NPC_IRONSPINE                   = 6489,
    NPC_AZSHIR_THE_SLEEPLESS        = 6490,

    // Other
    NPC_VORREL                      = 3981,     // horde quest giver
    NPC_SPIRIT_OF_MOGRAINE          = 16775,    // ashbringer event

    // Trash
    NPC_SCARLET_SORCERER            = 4294,
    NPC_SCARLET_MYRMIDON            = 4295,
    NPC_SCARLET_DEFENDER            = 4298,
    NPC_SCARLET_WIZARD              = 4300,
    NPC_SCARLET_CENTURION           = 4301,
    NPC_SCARLET_CHAMPION            = 4302,
    NPC_SCARLET_ABBOT               = 4303,
    NPC_SCARLET_MONK                = 4540,
    NPC_SCARLET_CHAPLAIN            = 4299,
};

enum GameObjects
{
    GO_CHAPEL_DOOR                  = 104591,
    GO_HIGH_INQUISITORS_DOOR        = 104600,
};

enum Misc
{
    FACTION_FRIENDLY                = 35,

    // Ashbringer
    SPELL_ASHBRINGER                = 28282,
    SPELL_SUMMON_SPIRIT_OF_MOGRAINE = 28928,
    SPELL_MOGRAINE_LIGHTING_BOLT    = 28167,        // wrong spell id
};

enum eAshbringer
{
    SAY_SCARLET_MOB_ASHBRINGER_1    = -1189029,
    SAY_SCARLET_MOB_ASHBRINGER_2    = -1189030,
    SAY_SCARLET_MOB_ASHBRINGER_3    = -1189031,
    SAY_SCARLET_MOB_ASHBRINGER_4    = -1189032,
    SAY_SCARLET_MOB_ASHBRINGER_5    = -1189033,
    SAY_SCARLET_MOB_ASHBRINGER_6    = -1189034,
    SAY_SCARLET_MOB_ASHBRINGER_7    = -1189035,
    SAY_MOGRAINE_ENTER_INSTANCE     = -1189036,
};

class MANGOS_DLL_DECL instance_scarlet_monastery : public ScriptedInstance
{
    public:
        instance_scarlet_monastery(Map* pMap);
        ~instance_scarlet_monastery() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		Player* GetAshbringerPlayerInInstance();

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		bool m_bRareBossSpawned;
		bool m_bMograinesWelcomeYell;

		uint8 m_uiRareBossesCounter;
		
		GUIDList lCathedralMobs;
};

#endif
