/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Instance_Wailing_Caverns
SD%Complete: 99
SDComment: Everything seems to work, still need some checking
SDCategory: Wailing Caverns
EndScriptData */

#include "precompiled.h"
#include "wailing_caverns.h"

instance_wailing_caverns::instance_wailing_caverns(Map* pMap) : ScriptedInstance(pMap),
	m_bYelled(false)
{
	Initialize();
}

void instance_wailing_caverns::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_wailing_caverns::OnPlayerEnter(Player* pPlayer)
{
    // Respawn the Mysterious chest if one of the players who enter the instance has the quest in his log
    if (pPlayer->GetQuestStatus(QUEST_FORTUNE_AWAITS) == QUEST_STATUS_COMPLETE &&
        !pPlayer->GetQuestRewardStatus(QUEST_FORTUNE_AWAITS))
    {
        DoRespawnGameObject(GO_MYSTERIOUS_CHEST, HOUR);
    }
}

void instance_wailing_caverns::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_DISCIPLE_OF_NARALEX:
            break;
        case NPC_NARALEX:
            if (m_auiEncounter[7] != DONE)
                pCreature->SetStandState(UNIT_STAND_STATE_SLEEP);
            break;
        case NPC_DEVIATE_FAERIE_DRAGON:
            SetRareBoss(pCreature, 50);
            return;
		default:
			return;
    }
	
	m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_wailing_caverns::OnObjectCreate(GameObject* pGo)
{
    if (pGo->GetEntry() == GO_MYSTERIOUS_CHEST)
    {
        m_mGoEntryGuidStore[GO_MYSTERIOUS_CHEST] = pGo->GetObjectGuid();
    }
}

void instance_wailing_caverns::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_LORD_COBRAHN:
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_LORD_PYTHAS:
            m_auiEncounter[1] = uiData;
            break;
        case TYPE_LADY_ANACONDRA:
            m_auiEncounter[2] = uiData;
            break;
        case TYPE_LORD_SERPENTIS:
            m_auiEncounter[3] = uiData;
            break;
        case TYPE_NARALEX_EVENT:
            m_auiEncounter[4] = uiData;
            break;
        case TYPE_NARALEX_PART1:
            m_auiEncounter[5] = uiData;
            break;
        case TYPE_NARALEX_PART2:
            m_auiEncounter[6] = uiData;
            break;
        case TYPE_NARALEX_PART3:
            m_auiEncounter[7] = uiData;
            break;
        case TYPE_MUTANUS_THE_DEVOURER:
            m_auiEncounter[8] = uiData;
            break;
        case TYPE_NARALEX_YELLED:
            m_bYelled = true;
            break;
    }
        
    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;        
    }
}

uint32 instance_wailing_caverns::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_wailing_caverns::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] != DONE)
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_wailing_caverns(Map* pMap)
{
    return new instance_wailing_caverns(pMap);
}

void AddSC_instance_wailing_caverns()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_wailing_caverns";
    pNewScript->GetInstanceData = &GetInstanceData_instance_wailing_caverns;
    pNewScript->RegisterSelf();
}
