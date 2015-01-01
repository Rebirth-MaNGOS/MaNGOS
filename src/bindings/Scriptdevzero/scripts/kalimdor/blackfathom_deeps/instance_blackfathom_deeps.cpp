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
SDName: Instance_Blackfathom_Deeps
SD%Complete: 95%
SDComment:
SDCategory: Blackfathom Deeps
EndScriptData */

#include "precompiled.h"
#include "blackfathom_deeps.h"

/* Encounter 0 = Gelihast
   Encounter 1 = Twilight Lord Kelris
   Encounter 2 = Shrine event
   Encounter 3 = Aku'Mai
*/

instance_blackfathom_deeps::instance_blackfathom_deeps(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_blackfathom_deeps::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_uiFireGUID.clear();

	OpenDoorCounter = 0;
}

void instance_blackfathom_deeps::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_SHRINE_OF_GELIHAST:
            if (m_auiEncounter[0] != DONE)
                InteractWithGo(GO_SHRINE_OF_GELIHAST, false);
            break;
        case GO_FIRE_OF_AKU_MAI_1:
        case GO_FIRE_OF_AKU_MAI_2:
        case GO_FIRE_OF_AKU_MAI_3:
        case GO_FIRE_OF_AKU_MAI_4:
			m_uiFireGUID.push_back(pGo->GetObjectGuid());
            InteractWithGo(pGo->GetEntry(), m_auiEncounter[1] == DONE ? true : false);
            return;
        case GO_AKU_MAI_DOOR:
            if (m_auiEncounter[2] == DONE) 
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ALTAR_OF_THE_DEEPS:
            if (m_auiEncounter[3] != DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_blackfathom_deeps::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_GELIHAST:
            if (uiData == DONE)
                InteractWithGo(GO_SHRINE_OF_GELIHAST, true);
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_KELRIS:
            if (uiData == DONE)
                SetFires(true);
            else
                SetFires(false);
            m_auiEncounter[1] = uiData;
            break;
        case TYPE_SHRINE:
            if (uiData == DONE)
                HandleGameObject(GO_AKU_MAI_DOOR, true);
            m_auiEncounter[2] = uiData;
            break;
        case TYPE_AKU_MAI:
            if (uiData == DONE)
                InteractWithGo(GO_ALTAR_OF_THE_DEEPS, true);
            m_auiEncounter[3] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_blackfathom_deeps::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_blackfathom_deeps::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
		case FIRE_OF_AKU_MAI_MOB_CRAB:
		case FIRE_OF_AKU_MAI_MOB_TURTLE:
		case FIRE_OF_AKU_MAI_MOB_ELEMENTAL:
		case FIRE_OF_AKU_MAI_MOB_MAKURA:
			++OpenDoorCounter;
			if (OpenDoorCounter >= 17)
				SetData(TYPE_SHRINE, DONE);
    }
}

void instance_blackfathom_deeps::SetFires(bool bState)
{
    for(GUIDList::iterator itr = m_uiFireGUID.begin(); itr != m_uiFireGUID.end(); ++itr)
		InteractWithGo(*itr, bState);
}

void instance_blackfathom_deeps::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_blackfathom_deeps(Map* pMap)
{
    return new instance_blackfathom_deeps(pMap);
}

void AddSC_instance_blackfathom_deeps()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_blackfathom_deeps";
    pNewscript->GetInstanceData = &GetInstanceData_instance_blackfathom_deeps;
    pNewscript->RegisterSelf();
}
