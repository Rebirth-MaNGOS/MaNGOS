/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: instance_uldaman
SD%Complete: 99%
SDComment:
SDCategory: Uldaman
EndScriptData */

#include "precompiled.h"
#include "uldaman.h"

instance_uldaman::instance_uldaman(Map* pMap) : ScriptedInstance(pMap),
	m_uiIronayaSealDoorTimer(26000)
{
	Initialize();
};

void instance_uldaman::Initialize()
{
	memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
	playerGuid.Clear();

    lWardens.clear();
    lStoneKeepers.clear();
}

void instance_uldaman::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_EARTHEN_CUSTODIAN:
        case NPC_EARTHEN_GUARDIAN:
        case NPC_EARTHEN_HALLSHAPER:
        case NPC_VAULT_WALKER:
            lWardens.push_back(pCreature->GetObjectGuid());
            pCreature->CastSpell(pCreature, SPELL_STONED, true);
            pCreature->SetNoCallAssistance(true);
            break;
        case NPC_STONE_KEEPER:
            lStoneKeepers.push_back(pCreature->GetObjectGuid());
            pCreature->CastSpell(pCreature, SPELL_STONED, true);
            pCreature->SetNoCallAssistance(true);
            break;
        case NPC_IRONAYA:
        case NPC_ARCHAEDAS:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
    }
}

void instance_uldaman::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_STONE_KEEPER)
    {
        pCreature->CastSpell(pCreature, SPELL_SELF_DESTRUCT, true);
        ActivateStoneKeeper();
    }
}
    
void instance_uldaman::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_TEMPLE_DOOR_LOWER:
        case GO_TEMPLE_DOOR_UPPER:
            if (m_auiEncounter[0] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ANCIENT_VAULT_DOOR:
            pGo->SetUInt32Value(GAMEOBJECT_STATE,1);
            pGo->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
            if (m_auiEncounter[1] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_IRONAYA_SEAL_DOOR:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_KEYSTONE:
            if (m_auiEncounter[2] == DONE)
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
            break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_uldaman::ActivateStoneKeeper()
{
    Player* pPlayer = instance->GetPlayer(playerGuid);

    for(GUIDList::iterator itr = lStoneKeepers.begin(); itr != lStoneKeepers.end(); ++itr)
    {
        Creature* pKeeper = instance->GetCreature(*itr);
        if (!pKeeper || !pKeeper->isAlive() || !pKeeper->HasAura(SPELL_STONED, EFFECT_INDEX_0))
            continue;

        pKeeper->RemoveAurasDueToSpell(SPELL_STONED);
        if (pPlayer)
        {
            pKeeper->SetInCombatWith(pPlayer);
            pKeeper->AddThreat(pPlayer);
        }
        else
            pKeeper->SetInCombatWithZone();

        return;
    }

    SetData(TYPE_ALTAR_DOORS, DONE);
}

Creature* instance_uldaman::GetClosestDwarfNotInCombat(Creature* pSearcher, uint32 uiPhase)
{
    std::list<Creature*> lTemp;

    for (GUIDList::const_iterator itr = lWardens.begin(); itr != lWardens.end(); ++itr)
    {
        Creature* pTemp = instance->GetCreature(*itr);

        if (pTemp && pTemp->isAlive() && !pTemp->getVictim())
        {
            switch(uiPhase)
            {
                case PHASE_ARCHA_1:
                    if (pTemp->GetEntry() != NPC_EARTHEN_CUSTODIAN && pTemp->GetEntry() != NPC_EARTHEN_HALLSHAPER)
                        continue;
                    break;
                case PHASE_ARCHA_2:
                    if (pTemp->GetEntry() != NPC_EARTHEN_GUARDIAN)
                        continue;
                    break;
                case PHASE_ARCHA_3:
                    if (pTemp->GetEntry() != NPC_VAULT_WALKER)
                        continue;
                    break;
            }

            lTemp.push_back(pTemp);
        }
    }

    if (lTemp.empty())
        return NULL;

    lTemp.sort(ObjectDistanceOrder(pSearcher));
    return lTemp.front();
}

bool instance_uldaman::ActivateGuardians(uint32 uiEntry)
{
    Creature* pArchaedas = GetSingleCreatureFromStorage(NPC_ARCHAEDAS);

    if (!pArchaedas)
        return false;

    for (GUIDList::const_iterator itr = lWardens.begin(); itr != lWardens.end(); ++itr)
    {
        Creature* pTemp = instance->GetCreature(*itr);

        if (!pTemp || !pTemp->isAlive() || pTemp->GetEntry() != uiEntry)
            continue;

        pTemp->RemoveAurasDueToSpell(SPELL_STONED);

        if (Unit* pUnit = pArchaedas->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            pTemp->SetInCombatWith(pUnit);
            pTemp->AddThreat(pUnit);
        }
    }

    return true;
}

void instance_uldaman::StartEvent(uint32 uiEventId, Player* pPlayer)
{
    playerGuid = pPlayer->GetObjectGuid();

    if (uiEventId == EVENT_ID_ALTAR_KEEPER && GetData(TYPE_ARCHAEDAS) != DONE)
        ActivateStoneKeeper();
    else if (uiEventId == EVENT_ID_ALTAR_ARCHAEDAS)
    {
        if (GetData(TYPE_ARCHAEDAS) == NOT_STARTED || GetData(TYPE_ARCHAEDAS) == FAIL)
            SetData(TYPE_ARCHAEDAS, SPECIAL);
    }
}
    
void instance_uldaman::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_ALTAR_DOORS:
            m_auiEncounter[0] = uiData;
            if (uiData == DONE)
            {
                HandleGameObject(GO_TEMPLE_DOOR_LOWER, true);
                HandleGameObject(GO_TEMPLE_DOOR_UPPER, true);
            }
            break;
        case TYPE_ARCHAEDAS:
            m_auiEncounter[1] = uiData;
            switch(uiData)
            {
                case FAIL:
                {
                    for (GUIDList::const_iterator itr = lWardens.begin(); itr != lWardens.end(); ++itr)
                    {
                        Creature* pWarden = instance->GetCreature(*itr);
                        if (pWarden && !pWarden->isAlive())
                        {
                            pWarden->Respawn();
                            pWarden->CastSpell(pWarden, SPELL_STONED, true);
                            pWarden->SetNoCallAssistance(true);
                        }
                    }

                    HandleGameObject(GO_TEMPLE_DOOR_LOWER, true);
                    break;
                }
                case DONE:
                {
                    for (GUIDList::const_iterator itr = lWardens.begin(); itr != lWardens.end(); ++itr)
                    {
                        Creature* pWarden = instance->GetCreature(*itr);
                        if (pWarden && pWarden->isAlive())
                            pWarden->ForcedDespawn();
                    }

                    HandleGameObject(GO_TEMPLE_DOOR_LOWER, true);
                    HandleGameObject(GO_ANCIENT_VAULT_DOOR, true);
                    break;
                }
                case IN_PROGRESS:
                case SPECIAL:
                {
                    HandleGameObject(GO_TEMPLE_DOOR_LOWER, false);
                    break;
                }
            }
            break;
        case TYPE_IRONAYA:
            m_auiEncounter[2] = uiData;
            if (uiData == DONE)
            {
                if (Creature* pIronaya = GetSingleCreatureFromStorage(NPC_IRONAYA))
                    pIronaya->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                HandleGameObject(GO_IRONAYA_SEAL_DOOR, true);
                InteractWithGo(GO_KEYSTONE, false);
            }
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_uldaman::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_uldaman::Update(uint32 uiDiff)
{
    if (GetData(TYPE_IRONAYA) == IN_PROGRESS)
    {
        if (m_uiIronayaSealDoorTimer < uiDiff)
            SetData(TYPE_IRONAYA, DONE);
        else
            m_uiIronayaSealDoorTimer -= uiDiff;
    }
}

void instance_uldaman::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_uldaman(Map* map)
{
    return new instance_uldaman(map);
}

bool ProcessEventId_event_spell_altar_boss_aggro(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool bIsStart)
{
    if (bIsStart && pSource->GetTypeId() == TYPEID_PLAYER)
    {
        if (instance_uldaman* pInstance = (instance_uldaman*)((Player*)pSource)->GetInstanceData())
        {
            pInstance->StartEvent(uiEventId, (Player*)pSource);
            return true;
        }
    }
    return false;
}

void AddSC_instance_uldaman()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_uldaman";
    pNewScript->GetInstanceData = &GetInstanceData_instance_uldaman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_spell_altar_boss_aggro";
    pNewScript->pProcessEventId = &ProcessEventId_event_spell_altar_boss_aggro;
    pNewScript->RegisterSelf();
}
