/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BFD_H
#define DEF_BFD_H

enum Data
{
    TYPE_GELIHAST,
    TYPE_KELRIS,
    TYPE_SHRINE,
    TYPE_AKU_MAI,
	MAX_ENCOUNTER
};

enum Creatures
{
    NPC_TWILIGHT_LORD_KELRIS		= 4832,
    NPC_LORGUS_JETT					= 12902,
    NPC_BARON_AQUANIS				= 12876,

	FIRE_OF_AKU_MAI_MOB_CRAB		= 4823,
	FIRE_OF_AKU_MAI_MOB_TURTLE		= 4825,
	FIRE_OF_AKU_MAI_MOB_ELEMENTAL	= 4978,
	FIRE_OF_AKU_MAI_MOB_MAKURA		= 4977
};

enum GameObjects
{
    GO_SHRINE_OF_GELIHAST       = 103015,
    GO_FIRE_OF_AKU_MAI_1        = 21118,
    GO_FIRE_OF_AKU_MAI_2        = 21119,
    GO_FIRE_OF_AKU_MAI_3        = 21120,
    GO_FIRE_OF_AKU_MAI_4        = 21121,
    GO_AKU_MAI_DOOR             = 21117,
    GO_ALTAR_OF_THE_DEEPS       = 103016
};

class MANGOS_DLL_DECL instance_blackfathom_deeps : public ScriptedInstance
{
    public:
        instance_blackfathom_deeps(Map* pMap);
        ~instance_blackfathom_deeps() {}

        void Initialize();

		int OpenDoorCounter;

        void OnObjectCreate(GameObject* pGo);
		void OnCreatureDeath(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

		void SetFires(bool bState);
		void CheckFires();
    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        GUIDList m_uiFireGUID;
};

#endif
