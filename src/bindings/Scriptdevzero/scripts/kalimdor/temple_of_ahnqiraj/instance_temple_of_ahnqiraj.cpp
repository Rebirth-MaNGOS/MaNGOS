/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Instance_Temple_of_Ahnqiraj
SD%Complete: 80
SDComment:
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

instance_temple_of_ahnqiraj::instance_temple_of_ahnqiraj(Map* pMap) : ScriptedInstance(pMap),
    m_uiBugTrioDeathCount(0),
    m_bOpenTwinsDoor(false)
{
    Initialize();
};

void instance_temple_of_ahnqiraj::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_lAnbDefenderList.clear();
}

void instance_temple_of_ahnqiraj::OnCreatureCreate (Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_VEM:
        case NPC_KRI:
        case NPC_YAUJ:
        case NPC_FANKRISS:
        case NPC_VISCIDUS:
        case NPC_VEKLOR:
        case NPC_VEKNILASH:
        case NPC_CTHUN:
        case NPC_VISCIDUS_DUMMY:
        case NPC_OURO_GROUND:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ANUBISATH_DEFENDER:
            m_lAnbDefenderList.push_back(pCreature->GetObjectGuid());
            break;
    }
}

void instance_temple_of_ahnqiraj::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_HUHURAN:
        {
            if(!m_bOpenTwinsDoor)
                m_bOpenTwinsDoor = true;
            break;
        }
        case NPC_ANUBISATH_DEFENDER:
            m_lAnbDefenderList.remove(pCreature->GetObjectGuid());
            break;
        default:
            break;
    }
    
    // Only open door if Hururan and all defenders are dead
    if(m_bOpenTwinsDoor && m_lAnbDefenderList.empty())
    {
        if (GameObject* pEntryDoor = GetSingleGameObjectFromStorage(GO_TWINS_ENTER_DOOR))
            pEntryDoor->SetGoState(GO_STATE_ACTIVE);
    }
}
       

void instance_temple_of_ahnqiraj::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_SKERAM_GATE:
            if (m_auiEncounter[TYPE_SKERAM] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;            
        case GO_TWINS_ENTER_DOOR:
            pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_TWINS_EXIT_DOOR:
            if (m_auiEncounter[TYPE_TWINS] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;

        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

bool instance_temple_of_ahnqiraj::IsEncounterInProgress() const
{
    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    return false;
}

void instance_temple_of_ahnqiraj::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_SKERAM:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_SKERAM_GATE);
            break;
        case TYPE_VEM:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_SARTURA:
        case TYPE_FANKRISS:
        case TYPE_VISCIDUS:
        case TYPE_HUHURAN:
        case TYPE_OURO:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_TWINS:
            // Either of the twins can set data, so return to avoid double changing
            if (m_auiEncounter[uiType] ==  uiData)
                return;

            m_auiEncounter[uiType] = uiData;

            if (GameObject* pEntryDoor = GetSingleGameObjectFromStorage(GO_TWINS_ENTER_DOOR))
                pEntryDoor->SetGoState(uiData != IN_PROGRESS ? GO_STATE_ACTIVE : GO_STATE_READY);

            if (GameObject* pExitDoor = GetSingleGameObjectFromStorage(GO_TWINS_EXIT_DOOR))
                pExitDoor->SetGoState(uiData == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
            break;
        case TYPE_CTHUN_PHASE:
            m_auiEncounter[uiType] = uiData;
            break;

        // The following temporarily datas are not to be saved
        case DATA_BUG_TRIO_DEATH:
            ++m_uiBugTrioDeathCount;
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " "
                   << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7] << " "
                   << m_auiEncounter[8];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_temple_of_ahnqiraj::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_temple_of_ahnqiraj::OnPlayerDeath(Player* pPlayer)
{
    // Remove the Digestive Acid.
    pPlayer->RemoveAurasDueToSpell(26476, nullptr, AURA_REMOVE_BY_DEFAULT);
}

uint32 instance_temple_of_ahnqiraj::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case TYPE_SKERAM:
            return m_auiEncounter[0];
        case TYPE_VEM:
            return m_auiEncounter[1];
        case TYPE_TWINS:
            return m_auiEncounter[2];
        case TYPE_CTHUN_PHASE:
            return m_auiEncounter[3];
        case TYPE_SARTURA:
            return m_auiEncounter[4];
        case TYPE_FANKRISS:
            return m_auiEncounter[5];
        case TYPE_VISCIDUS:
            return m_auiEncounter[6];
        case TYPE_HUHURAN:
            return m_auiEncounter[7];
        case TYPE_OURO:
            return m_auiEncounter[8];

        case DATA_BUG_TRIO_DEATH:
            return m_uiBugTrioDeathCount;
        default:
            return 0;
    }
}

InstanceData* GetInstanceData_instance_temple_of_ahnqiraj(Map* pMap)
{
    return new instance_temple_of_ahnqiraj(pMap);
}

void AddSC_instance_temple_of_ahnqiraj()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_temple_of_ahnqiraj";
    pNewScript->GetInstanceData = &GetInstanceData_instance_temple_of_ahnqiraj;
    pNewScript->RegisterSelf();
}
