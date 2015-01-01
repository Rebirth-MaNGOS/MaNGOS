/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ULDAMAN_H
#define DEF_ULDAMAN_H

enum Data
{
    TYPE_ALTAR_DOORS,
    TYPE_ARCHAEDAS,
    TYPE_IRONAYA,
    MAX_ENCOUNTER
};

enum Creatures
{
    NPC_STONE_KEEPER                = 4857,
    NPC_EARTHEN_CUSTODIAN           = 7309,
    NPC_EARTHEN_GUARDIAN            = 7076,
    NPC_EARTHEN_HALLSHAPER          = 7077,
    NPC_IRONAYA                     = 7228,
    NPC_VAULT_WALKER                = 10120,
    NPC_ARCHAEDAS                   = 2748,
    NPC_SHADOWFORGE_AMBUSHER        = 7091,
};

enum GameObjects
{
    GO_TEMPLE_DOOR_LOWER            = 141869,
    GO_TEMPLE_DOOR_UPPER            = 124367,
    GO_ALTAR_OF_ARCHAEDAS           = 133234,
    GO_ALTAR_OF_THE_KEEPER_TEMPLE   = 130511,
    GO_ANCIENT_VAULT_DOOR           = 124369,
    GO_IRONAYA_SEAL_DOOR            = 124372,
    GO_KEYSTONE                     = 124371
};

enum Misc
{
    PHASE_ARCHA_1                   = 1,
    PHASE_ARCHA_2                   = 2,
    PHASE_ARCHA_3                   = 3,

    SPELL_SELF_DESTRUCT             = 9874,
    SPELL_STONED                    = 10255,

    EVENT_ID_ALTAR_KEEPER           = 2228,                     // spell 11568
    EVENT_ID_ALTAR_ARCHAEDAS        = 2268                      // spell 10340
};

class MANGOS_DLL_DECL instance_uldaman : public ScriptedInstance
{
    public:
        instance_uldaman(Map* pMap);
        ~instance_uldaman() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnCreatureDeath(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);

        void ActivateStoneKeeper();
        void StartEvent(uint32 uiEventId, Player* pPlayer);
        Creature* GetClosestDwarfNotInCombat(Creature* pSearcher, uint32 uiPhase);
        ObjectGuid GetEventStarterGuid() { return playerGuid;}
        bool ActivateGuardians(uint32 uiEntry);

	protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        uint32 m_uiIronayaSealDoorTimer;

        ObjectGuid playerGuid;

        GUIDList lStoneKeepers;
        GUIDList lWardens;
};

#endif

