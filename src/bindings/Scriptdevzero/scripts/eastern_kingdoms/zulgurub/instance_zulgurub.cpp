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
SDName: Instance_ZulGurub
SD%Complete: 90
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

instance_zulgurub::instance_zulgurub(Map* pMap) : ScriptedInstance(pMap),
    m_bEnterZulGurubYelled(false),
    m_bProtectAltarYelled(false),
    m_bThekalEvent(false),

    m_uiThekalEventTimer(10000)
{
    Initialize();
}

void instance_zulgurub::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    lJeklikFire.clear();
    lChainedSpirits.clear();
}

void instance_zulgurub::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_LORKHAN:
        case NPC_ZATH:
        case NPC_THEKAL:
            if (m_auiEncounter[4] != DONE && !pCreature->isAlive())
                pCreature->Respawn();
            break;
        case NPC_JINDO:
        case NPC_HAKKAR: 
        case NPC_VENOXIS:
        case NPC_MANDOKIR:
            break;
		case NPC_ATALAI_MISTRESS:
		case NPC_VOODOO_SLAVE:
		case NPC_WITHERED_MISTRESS:
			m_mNpcMultiEntryGuidStore[pCreature->GetEntry()].push_back(pCreature->GetObjectGuid());
			return;
        case NPC_CHAINED_SPIRIT:
            lChainedSpirits.push_back(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_zulgurub::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_JEKLIK:
        case NPC_VENOXIS:
        case NPC_MARLI:
        case NPC_THEKAL:
        case NPC_ARLOKK:
            LowerHakkarHitPoints();
            break;
    }
}

void instance_zulgurub::OnCreatureEnterCombat(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_HAKKAR:
            LowerHakkarHitPoints();
            break;
    }
}

void instance_zulgurub::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_GONG_OF_BETHEKK:
        case GO_FORCE_FIELD:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
        case GO_LIQUID_FIRE:
            lJeklikFire.push_back(pGo->GetObjectGuid());
            break;
    }
}

void instance_zulgurub::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_ARLOKK:
            m_auiEncounter[0] = uiData;
            if (uiData != SPECIAL)
                DoUseDoorOrButton(GO_FORCE_FIELD);
            break;
        case TYPE_JEKLIK:
            m_auiEncounter[1] = uiData;
            if (!lJeklikFire.empty() && (uiData == NOT_STARTED || uiData == DONE))
            {
                for(GUIDList::iterator itr = lJeklikFire.begin(); itr != lJeklikFire.end(); ++itr)
                {
                    if (GameObject* pLiquidFire = instance->GetGameObject(*itr))
                        pLiquidFire->AddObjectToRemoveList();
                }
                lJeklikFire.clear();
            }
            break;
        case TYPE_VENOXIS:
            m_auiEncounter[2] = uiData;
            break;
        case TYPE_MARLI:
            m_auiEncounter[3] = uiData;
            break;
        case TYPE_THEKAL:
            m_auiEncounter[4] = uiData;
            HandleThekalEvent(uiData);
            break;
        case TYPE_LORKHAN:
            m_auiEncounter[5] = uiData;
            HandleThekalEvent(uiData);
            break;
        case TYPE_ZATH:
            m_auiEncounter[6] = uiData;
            HandleThekalEvent(uiData);
            break;
        case TYPE_MANDOKIR:
            m_auiEncounter[7] = uiData;
            switch(uiData)
            {
                case NOT_STARTED:
                case FAIL:
                case DONE:
                    if (!lChainedSpirits.empty())
                        for(GUIDList::iterator itr = lChainedSpirits.begin(); itr != lChainedSpirits.end(); ++itr)
                            if (Creature* pChainedSpirit = instance->GetCreature(*itr))
                                pChainedSpirit->ForcedDespawn();
                    lChainedSpirits.clear();
                    break;
            }
            break;
        case TYPE_OHGAN:
            m_auiEncounter[8] = uiData;
            break;
        case TYPE_HAKKAR:
            m_auiEncounter[9] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " " << m_auiEncounter[9];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_zulgurub::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_zulgurub::Update(uint32 uiDiff)
{
    if (m_bThekalEvent)
    {
        if (m_uiThekalEventTimer <= uiDiff)
        {
            if (GetData(TYPE_THEKAL) == IN_PROGRESS && GetData(TYPE_LORKHAN) == IN_PROGRESS && GetData(TYPE_ZATH) == IN_PROGRESS)
                SetData(TYPE_THEKAL, SPECIAL);
            else
                SetData(TYPE_THEKAL, FAIL);

            m_bThekalEvent = false;
        }
        else
            m_uiThekalEventTimer -= uiDiff;
    }
}

void instance_zulgurub::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8] >> m_auiEncounter[9];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_zulgurub::LowerHakkarHitPoints()
{
    // each time High Priest dies lower Hakkar's HP
    uint32 m_uiPriestDead = 0;
    for(uint8 i = 0; i < 5; ++i)
    {
        if (m_auiEncounter[i] == DONE)
            ++m_uiPriestDead;
    }

    Creature* pHakkar = GetSingleCreatureFromStorage(NPC_HAKKAR);
    if (pHakkar && pHakkar->isAlive())
    {
        pHakkar->SetMaxHealth(pHakkar->GetCreatureInfo()->maxhealth - (m_uiPriestDead*60000));
        pHakkar->SetHealth(pHakkar->GetCreatureInfo()->maxhealth - (m_uiPriestDead*60000));
    }
}

void instance_zulgurub::HakkarYell(uint32 atEntry)
{
    switch(atEntry)
    {
        case AT_ZULGURUB_ENTRANCE_1:
        case AT_ZULGURUB_ENTRANCE_2:
        {
            if (m_bEnterZulGurubYelled)
                return;

            DoOrSimulateScriptTextForThisInstance(SAY_HAKKAR_MINION_DESTROY, NPC_HAKKAR);
            m_bEnterZulGurubYelled = true;
            return;
        }    
        case AT_ALTAR_OF_THE_BLOOD_GOD_DOWN:
        case AT_ALTAR_OF_THE_BLOOD_GOD_UP:
        {
            if (m_bProtectAltarYelled)
                return;

            DoOrSimulateScriptTextForThisInstance(SAY_HAKKAR_PROTECT_ALTAR, NPC_HAKKAR);
            m_bProtectAltarYelled = true;
            return;
        }
        default:
            return;
    }
}

void instance_zulgurub::HandleThekalEvent(uint32 uiData)
{
    switch(uiData)
    {
        case IN_PROGRESS:
        {
            if (!m_bThekalEvent)
            {
                m_uiThekalEventTimer = 10000;
                m_bThekalEvent = true;
            }
            break;
        }
        case FAIL:
        {
            Creature* pThekal = GetSingleCreatureFromStorage(NPC_THEKAL);
            Creature* pLorKhan = GetSingleCreatureFromStorage(NPC_LORKHAN);
            Creature* pZath = GetSingleCreatureFromStorage(NPC_ZATH);
            Unit* pVictim = NULL;

            if (pThekal && pThekal->isAlive() && pThekal->getVictim())
                pVictim = pThekal->getVictim();
            else if (pLorKhan && pLorKhan->isAlive() && pLorKhan->getVictim())
                pVictim = pLorKhan->getVictim();
            else if (pZath && pZath->isAlive() && pZath->getVictim())
                pVictim = pZath->getVictim();

            if (pThekal && pThekal->getStandState() == UNIT_STAND_STATE_DEAD)
            {
                pThekal->AI()->JustRespawned();
                pThekal->SetHealth(pThekal->GetMaxHealth());
                pThekal->SetPower(POWER_MANA, pThekal->GetMaxPower(POWER_MANA));
                pThekal->RemoveAllAuras();
            }

            if (pLorKhan && !pLorKhan->isAlive())
            {
                if (pVictim)
                {
                    pLorKhan->SetDeathState(JUST_ALIVED);
                    pLorKhan->AI()->AttackStart(pVictim);
                }
                else
                    pLorKhan->Respawn();
            }

            if (pZath && !pZath->isAlive())
            {
                if (pVictim)
                {
                    pZath->SetDeathState(JUST_ALIVED);
                    pZath->AI()->AttackStart(pVictim);
                }
                else
                    pZath->Respawn();
            }

            SetData(TYPE_THEKAL, NOT_STARTED);
            SetData(TYPE_LORKHAN, NOT_STARTED);
            SetData(TYPE_ZATH, NOT_STARTED);
            break;
        }
        default:
            break;
    }
}

InstanceData* GetInstanceData_instance_zulgurub(Map* pMap)
{
    return new instance_zulgurub(pMap);
}

void AddSC_instance_zulgurub()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_zulgurub";
    pNewScript->GetInstanceData = &GetInstanceData_instance_zulgurub;
    pNewScript->RegisterSelf();
}
