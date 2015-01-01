/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_INSTANCE_H
#define SC_INSTANCE_H

#include "InstanceData.h"
#include "Map.h"

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4
};

#define LAVA_DAMAGE_PERIOD  1750

#define OUT_SAVE_INST_DATA             debug_log("SD0: Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    debug_log("SD0: Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          debug_log("SD0: Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    debug_log("SD0: Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        error_log("SD0: Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

typedef std::map<uint32, std::vector<ObjectGuid>> MultiObjectMap;

class MANGOS_DLL_DECL ScriptedInstance : public InstanceData
{
    public:
        ScriptedInstance(Map* pMap) : InstanceData(pMap) {}
        ~ScriptedInstance() {}

        // Default accessor functions
        GameObject* GetSingleGameObjectFromStorage(uint32 uiEntry);
        Creature* GetSingleCreatureFromStorage(uint32 uiEntry, bool bSkipDebugLog = false);
		std::vector<Creature*> GetCreaturesFromMultiStorage(uint32 uiEntry);

        // Change active state of doors or buttons
        void DoUseDoorOrButton(ObjectGuid guid, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);
        void DoUseDoorOrButton(uint32 uiEntry, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);

        // Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(ObjectGuid guid, uint32 uiTimeToDespawn = MINUTE);
        void DoRespawnGameObject(uint32 uiEntry, uint32 uiTimeToDespawn = MINUTE);

        // Sends world state update to all players in instance
        void DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData);

        // Get a Player from map
        Player* GetPlayerInMap(bool bOnlyAlive = false, bool bCanBeGamemaster = true);

        /// Wrapper for simulating map-wide text in this instance. It is expected that the Creature is stored in m_mNpcEntryGuidStore if loaded.
        void DoOrSimulateScriptTextForThisInstance(int32 iTextEntry, uint32 uiCreatureEntry)
        {
            // Prevent debug output in GetSingleCreatureFromStorage
            DoOrSimulateScriptTextForMap(iTextEntry, uiCreatureEntry, instance, GetSingleCreatureFromStorage(uiCreatureEntry, true));
        }

        //use HandleGameObject(GUID,boolen,NULL); in any other script
        void HandleGameObject(ObjectGuid guid, bool open);
        void HandleGameObject(uint32 uiEntry, bool open);

        // set interaction with GameObject
		void InteractWithGo(ObjectGuid guid, bool state);
		void InteractWithGo(uint32 uiEntry, bool state);

        // set rare boss chance to be in instance
        bool SetRareBoss(Creature* pCreature, uint32 uiChance);

        // Lava damage
        void SetLavaState(Player* pPlayer, bool enter);
        void DoLavaDamage(const uint32 uiDiff);

		Player* GetRandomLivingPlayerInInstance();

    protected:
        // Storage for GO-Guids and NPC-Guids
        EntryGuidMap m_mGoEntryGuidStore;                   ///< Store unique GO-Guids by entry
        EntryGuidMap m_mNpcEntryGuidStore;                  ///< Store unique NPC-Guids by entry
		MultiObjectMap m_mNpcMultiEntryGuidStore;			// Store multiple creatures with the same entry.
};

#endif
