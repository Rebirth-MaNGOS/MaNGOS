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
SDName: Instance_Razorfen_Kraul
SD%Complete:
SDComment: Support for Ward passing
SDCategory: Razorfen Kraul
EndScriptData */

#include "precompiled.h"
#include "razorfen_kraul.h"

instance_razorfen_kraul::instance_razorfen_kraul(Map* pMap) : ScriptedInstance(pMap),
	m_uiWardKeepersRemaining(0)
{
    Initialize();
}

void instance_razorfen_kraul::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_razorfen_kraul::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_EARTHCALLER_HALMGAR:
        case NPC_BLIND_HUNTER:
            SetRareBoss(pCreature, 50);
            break;
		case NPC_WARD_KEEPER:
            ++m_uiWardKeepersRemaining;
            break;
    }
}

void instance_razorfen_kraul::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_WARD_KEEPER:
            --m_uiWardKeepersRemaining;
			
			if (!m_uiWardKeepersRemaining)
				SetData(TYPE_AGATHELOS, DONE);
            break;
    }
}

void instance_razorfen_kraul::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_AGATHELOS_WARD:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            if(m_auiEncounter[0] == DONE) 
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
    }
}

void instance_razorfen_kraul::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_AGATHELOS:
			if (uiData == DONE)
				HandleGameObject(GO_AGATHELOS_WARD, true);
            m_auiEncounter[0] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_razorfen_kraul::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_razorfen_kraul::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0];

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_razorfen_kraul(Map* pMap)
{
    return new instance_razorfen_kraul(pMap);
}

void AddSC_instance_razorfen_kraul()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_razorfen_kraul";
    pNewscript->GetInstanceData = &GetInstanceData_instance_razorfen_kraul;
    pNewscript->RegisterSelf();
}
