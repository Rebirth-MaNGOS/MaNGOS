/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ONYXIASLAIR_H
#define DEF_ONYXIASLAIR_H

class TemporarySummon;

enum Data
{
    TYPE_ONYXIA,
    MAX_ENCOUNTER
};

enum Creatures
{
    NPC_ONYXIA                  = 10184,
    NPC_ONYXIA_WARDER           = 12129,
    NPC_ONYXIA_WHELP            = 11262,
	NPC_ONYXIA_TRIGGER			= 12758,
	NPC_ONYXIA_WHELP_WEST		= 100001
};

enum GameObjects
{
    GO_WHELP_EGG				= 176511
};

enum WorldSpells
{
	SPELL_ERUPTION				= 17731
};

#define SUMMON_PLAYER_TRIGGER_DISTANCE 85.0f    // yards

class MANGOS_DLL_DECL instance_onyxias_lair : public ScriptedInstance
{
    public:
        instance_onyxias_lair(Map* pMap);
        ~instance_onyxias_lair() {}

        void Initialize();

		bool IsEncounterInProgress() const;

        void OnCreatureCreate(Creature* pCreature);
		void OnCreatureDeath(Creature *pCreature);
		void OnObjectCreate(GameObject *pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		void ShuffleFissures();
		bool FissureSurge(Creature *onyxia);
    protected:
		std::list<GameObject*> m_lavaFissures;
		std::list<GameObject*> m_liveLavaFissures[3];
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		GUIDList m_uiOnyxiaWarderGUID;
		std::list<TemporarySummon*> m_onyxianWhelps;
		GUIDList m_eggGuids;
};

#endif
