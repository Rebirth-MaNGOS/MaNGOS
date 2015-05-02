/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

/**
   Function that uses a door or a button

   @param   guid The ObjectGuid of the Door/ Button that will be used
   @param   uiWithRestoreTime (in seconds) if == 0 autoCloseTime will be used (if not 0 by default in *_template)
   @param   bUseAlternativeState Use to alternative state
 */
void ScriptedInstance::DoUseDoorOrButton(ObjectGuid guid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!guid)
        return;

    if (GameObject* pGo = instance->GetGameObject(guid))
    {
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton(uiWithRestoreTime, bUseAlternativeState);
            else if (pGo->getLootState() == GO_ACTIVATED)
                pGo->ResetDoorOrButton();
        }
        else
            error_log("SD0: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.", pGo->GetEntry(), pGo->GetGoType());
    }
}

/// Function that uses a door or button that is stored in m_mGoEntryGuidStore
void ScriptedInstance::DoUseDoorOrButton(uint32 uiEntry, uint32 uiWithRestoreTime /*= 0*/, bool bUseAlternativeState /*= false*/)
{
    EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(uiEntry);
    if (find != m_mGoEntryGuidStore.end())
        DoUseDoorOrButton(find->second, uiWithRestoreTime, bUseAlternativeState);
    else
        // Output log, possible reason is not added GO to storage, or not yet loaded
        debug_log("SD0: Script call DoUseDoorOrButton(by Entry), but no gameobject of entry %u was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());
}

/**
   Function that respawns a despawned GameObject with given time

   @param   guid The ObjectGuid of the GO that will be respawned
   @param   uiTimeToDespawn (in seconds) Despawn the GO after this time, default is a minute
 */
void ScriptedInstance::DoRespawnGameObject(ObjectGuid guid, uint32 uiTimeToDespawn)
{
    if (!guid)
        return;

    if (GameObject* pGo = instance->GetGameObject(guid))
    {
        //not expect any of these should ever be handled
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE || pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR ||
            pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON || pGo->GetGoType() == GAMEOBJECT_TYPE_TRAP)
            return;

        if (pGo->isSpawned())
            return;

        pGo->SetRespawnTime(uiTimeToDespawn);
        pGo->Refresh();
    }
}

/// Function that respawns a despawned GO that is stored in m_mGoEntryGuidStore
void ScriptedInstance::DoRespawnGameObject(uint32 uiEntry, uint32 uiTimeToDespawn)
{
    EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(uiEntry);
    if (find != m_mGoEntryGuidStore.end())
        DoRespawnGameObject(find->second, uiTimeToDespawn);
    else
        // Output log, possible reason is not added GO to storage, or not yet loaded;
        debug_log("SD0: Script call DoRespawnGameObject(by Entry), but no gameobject of entry %u was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());
}

/**
   Helper function to update a world state for all players in the map

   @param   uiStateId The WorldState that will be set for all players in the map
   @param   uiStateData The Value to which the State will be set to
 */
void ScriptedInstance::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                pPlayer->SendUpdateWorldState(uiStateId, uiStateData);
        }
    }
    else
        debug_log("SD0: DoUpdateWorldState attempt send data but no players in map.");
}

/// Get the first found Player* (with requested properties) in the map. Can return NULL.
Player* ScriptedInstance::GetPlayerInMap(bool bOnlyAlive /*=false*/, bool bCanBeGamemaster /*=true*/)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
    {
        Player* pPlayer = itr->getSource();
        if (pPlayer && (!bOnlyAlive || pPlayer->isAlive()) && (bCanBeGamemaster || !pPlayer->isGameMaster()))
            return pPlayer;
    }

    return NULL;
}

/// Returns a pointer to a loaded GameObject that was stored in m_mGoEntryGuidStore. Can return NULL
GameObject* ScriptedInstance::GetSingleGameObjectFromStorage(uint32 uiEntry)
{
    EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(uiEntry);
    if (find != m_mGoEntryGuidStore.end())
        return instance->GetGameObject(find->second);

    // Output log, possible reason is not added GO to map, or not yet loaded;
    debug_log("SD0: Script requested gameobject with entry %u, but no gameobject of this entry was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());

    return NULL;
}

/// Returns a pointer to a loaded Creature that was stored in m_mGoEntryGuidStore. Can return NULL
Creature* ScriptedInstance::GetSingleCreatureFromStorage(uint32 uiEntry, bool bSkipDebugLog /*=false*/)
{
    EntryGuidMap::iterator find = m_mNpcEntryGuidStore.find(uiEntry);
    if (find != m_mNpcEntryGuidStore.end())
        return instance->GetCreature(find->second);

    // Output log, possible reason is not added GO to map, or not yet loaded;
    if (!bSkipDebugLog)
        debug_log("SD0: Script requested creature with entry %u, but no npc of this entry was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());

    return NULL;
}

// Returns a list of pointers to all creatures in m_mGoEntryGuidStore that match the provided entry. If there are many creatures in storage this might take some time. Use with caution!
std::vector<Creature*> ScriptedInstance::GetCreaturesFromMultiStorage(uint32 uiEntry)
{
	std::vector<Creature*> result;

	for (ObjectGuid current_ref : m_mNpcMultiEntryGuidStore[uiEntry])
	{

		Creature* current_creature = instance->GetCreature(current_ref);
		if (current_creature && current_creature->GetEntry() == uiEntry)
			result.push_back(current_creature);
	}

	return result;
}

void ScriptedInstance::HandleGameObject(ObjectGuid guid, bool open)
{
    if (GameObject* pGo = instance->GetGameObject(guid))
        pGo->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    else
        debug_log("SD0: InstanceData: HandleGameObject failed");
}

/// Function that uses a door or button that is stored in m_mGoEntryGuidStore
void ScriptedInstance::HandleGameObject(uint32 uiEntry, bool open)
{
    EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(uiEntry);
    if (find != m_mGoEntryGuidStore.end())
        HandleGameObject(find->second, open);
    else
        // Output log, possible reason is not added GO to storage, or not yet loaded
        debug_log("SD0: Script call HandleGameObject(by Entry), but no gameobject of entry %u was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());
}

void ScriptedInstance::InteractWithGo(ObjectGuid guid, bool state)
{
    if (GameObject* pGo = instance->GetGameObject(guid))
	{
		if (state)
			pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
		else
			pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
	}
}

void ScriptedInstance::InteractWithGo(uint32 uiEntry, bool state)
{
    EntryGuidMap::iterator find = m_mGoEntryGuidStore.find(uiEntry);
    if (find != m_mGoEntryGuidStore.end())
        InteractWithGo(find->second, state);
    else
        // Output log, possible reason is not added GO to storage, or not yet loaded
        debug_log("SD0: Script call InteractWithGo(by Entry), but no gameobject of entry %u was created yet, or it was not stored by script for map %u.", uiEntry, instance->GetId());
}

bool ScriptedInstance::SetRareBoss(Creature* pCreature, uint32 uiChance)
{
    if (uiChance < urand(0,100))
    {
        pCreature->setFaction(35);
        pCreature->SetVisibility(VISIBILITY_OFF);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        return false;
    }
    else
    {
        pCreature->setFaction(pCreature->GetCreatureInfo()->faction_A);
        pCreature->SetVisibility(VISIBILITY_ON);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        return true;
    }
}

Player* ScriptedInstance::GetRandomLivingPlayerInInstance(){
		Map::PlayerList const& players = instance->GetPlayers();
		std::vector<Player*> playersInMap;
				
		for (auto player = players.begin(); player != players.end(); ++player)
			if ((player->getSource())->isAlive()) 
				playersInMap.push_back(player->getSource());

        if (playersInMap.empty())
            return NULL;

        return playersInMap[urand(0, playersInMap.size() - 1)];

}

/**
   Constructor for DialogueHelper

   @param   pDialogueArray The static const array of DialogueEntry holding the information about the dialogue. This array MUST be terminated by {0,0,0}
 */
DialogueHelper::DialogueHelper(DialogueEntry const* pDialogueArray) :
    m_pInstance(NULL),
    m_pDialogueArray(pDialogueArray),
    m_pCurrentEntry(NULL),
    m_pDialogueTwoSideArray(NULL),
    m_pCurrentEntryTwoSide(NULL),
    m_uiTimer(0),
    m_bIsFirstSide(true),
    m_bCanSimulate(false)
{}

/**
   Constructor for DialogueHelper (Two Sides)

   @param   pDialogueTwoSideArray The static const array of DialogueEntryTwoSide holding the information about the dialogue. This array MUST be terminated by {0,0,0,0,0}
 */
DialogueHelper::DialogueHelper(DialogueEntryTwoSide const* pDialogueTwoSideArray) :
    m_pInstance(NULL),
    m_pDialogueArray(NULL),
    m_pCurrentEntry(NULL),
    m_pDialogueTwoSideArray(pDialogueTwoSideArray),
    m_pCurrentEntryTwoSide(NULL),
    m_uiTimer(0),
    m_bIsFirstSide(true),
    m_bCanSimulate(false)
{}

/**
   Function to start a (part of a) dialogue

   @param   iTextEntry The TextEntry of the dialogue that will be started (must be always the entry of first side)
 */
void DialogueHelper::StartNextDialogueText(int32 iTextEntry)
{
    // Find iTextEntry
    bool bFound = false;

    if (m_pDialogueArray)                                   // One Side
    {
        for (DialogueEntry const* pEntry = m_pDialogueArray; pEntry->iTextEntry; ++pEntry)
        {
            if (pEntry->iTextEntry == iTextEntry)
            {
                m_pCurrentEntry = pEntry;
                bFound = true;
                break;
            }
        }
    }
    else                                                    // Two Sides
    {
        for (DialogueEntryTwoSide const* pEntry = m_pDialogueTwoSideArray; pEntry->iTextEntry; ++pEntry)
        {
            if (pEntry->iTextEntry == iTextEntry)
            {
                m_pCurrentEntryTwoSide = pEntry;
                bFound = true;
                break;
            }
        }
    }

    if (!bFound)
    {
        debug_log("Script call DialogueHelper::StartNextDialogueText, but textEntry %i is not in provided dialogue (on map id %u)", iTextEntry, m_pInstance ? m_pInstance->instance->GetId() : 0);
        return;
    }

    DoNextDialogueStep();
}

/// Internal helper function to do the actual say of a DialogueEntry
void DialogueHelper::DoNextDialogueStep()
{
    // Last Dialogue Entry done?
    if ((m_pCurrentEntry && !m_pCurrentEntry->iTextEntry) || (m_pCurrentEntryTwoSide && !m_pCurrentEntryTwoSide->iTextEntry))
    {
        m_uiTimer = 0;
        return;
    }

    // Get Text, SpeakerEntry and Timer
    int32 iTextEntry = 0;
    uint32 uiSpeakerEntry = 0;

    if (m_pDialogueArray)                               // One Side
    {
        uiSpeakerEntry = m_pCurrentEntry->uiSayerEntry;
        iTextEntry = m_pCurrentEntry->iTextEntry;

        m_uiTimer = m_pCurrentEntry->uiTimer;
    }
    else                                                // Two Sides
    {
        // Second Entries can be 0, if they are the entry from first side will be taken
        uiSpeakerEntry = !m_bIsFirstSide && m_pCurrentEntryTwoSide->uiSayerEntryAlt ? m_pCurrentEntryTwoSide->uiSayerEntryAlt : m_pCurrentEntryTwoSide->uiSayerEntry;
        iTextEntry = !m_bIsFirstSide && m_pCurrentEntryTwoSide->iTextEntryAlt ? m_pCurrentEntryTwoSide->iTextEntryAlt : m_pCurrentEntryTwoSide->iTextEntry;

        m_uiTimer = m_pCurrentEntryTwoSide->uiTimer;
    }

    // Simulate Case
    if (uiSpeakerEntry && iTextEntry < 0)
    {
        // Use Speaker if directly provided
        Creature* pSpeaker = GetSpeakerByEntry(uiSpeakerEntry);
        if (m_pInstance && !pSpeaker)                       // Get Speaker from instance
        {
            if (m_bCanSimulate)                             // Simulate case
            {
                m_pInstance->DoOrSimulateScriptTextForThisInstance(iTextEntry, uiSpeakerEntry);
            }
            else
            {
                pSpeaker = m_pInstance->GetSingleCreatureFromStorage(uiSpeakerEntry);
            }
        }

        if (pSpeaker)
        {
            DoScriptText(iTextEntry, pSpeaker);
        }
    }

    JustDidDialogueStep(m_pDialogueArray ?  m_pCurrentEntry->iTextEntry : m_pCurrentEntryTwoSide->iTextEntry);

    // Increment position
    if (m_pDialogueArray)
    {
        ++m_pCurrentEntry;
    }
    else
    {
        ++m_pCurrentEntryTwoSide;
    }
}

/// Call this function within any DialogueUpdate method. This is required for saying next steps in a dialogue
void DialogueHelper::DialogueUpdate(uint32 uiDiff)
{
    if (m_uiTimer)
    {
        if (m_uiTimer <= uiDiff)
        {
            DoNextDialogueStep();
        }
        else
        {
            m_uiTimer -= uiDiff;
        }
    }
}
