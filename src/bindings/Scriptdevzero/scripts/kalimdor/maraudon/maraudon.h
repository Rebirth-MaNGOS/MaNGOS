/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_MARAUDON_H
#define DEF_MARAUDON_H

enum Type
{
    TYPE_CELEBRAS_THE_CURSED,
	MAX_ENCOUNTER
};

enum Creatures
{
    NPC_CELEBRAS_THE_REDEEMED     = 13716,
    NPC_MESHLOK_THE_HARVESTER     = 12237,
	NPC_LARVA                     = 12218
};

enum GameObjects
{
	GO_LARVA_SPEWER = 178559
};

class MANGOS_DLL_DECL instance_maraudon : public ScriptedInstance
{
    public:
        instance_maraudon(Map* pMap);
        ~instance_maraudon() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
		void OnObjectCreate(GameObject* pGo);

		void Update(uint32 uiDiff);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

		ObjectGuid m_uiLarvaSpewerGUID;
		uint32 m_uiSpawnTimer;
};

#endif
