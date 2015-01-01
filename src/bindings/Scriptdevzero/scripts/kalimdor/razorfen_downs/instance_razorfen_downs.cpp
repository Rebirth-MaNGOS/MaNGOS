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
SDName: Instance Razorfen Downs
SD%Complete:
SDComment:
SDCategory: Razorfen Downs
EndScriptData */

#include "precompiled.h"
#include "razorfen_downs.h"

static float Spawns[2][3] =
{
	{2483, 826, 45},
	{2556, 900, 48}
};

instance_razorfen_downs::instance_razorfen_downs(Map* pMap) : ScriptedInstance(pMap),
	EventProgress(0),
	m_uiGongUse(0)
{
    Initialize();
}

void instance_razorfen_downs::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_uiSpectreGUID.clear();
}
    
void instance_razorfen_downs::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_FROZEN_SPECTRE:
			m_uiSpectreGUID.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_RAGGLESNOUT:
            SetRareBoss(pCreature, 50);
            break;
    }
}
    
void instance_razorfen_downs::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_GONG:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            if(m_auiEncounter[0] == DONE)
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
            break;
    }
}

void instance_razorfen_downs::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case DATA_GONG_USE:
			if(uiData == IN_PROGRESS)
			{
				++EventProgress;
				DoEvent(EventProgress);
			}
			m_uiGongUse = uiData;
            break;
        case TYPE_TUTENKASH: 
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_AMNENNAR:
            m_auiEncounter[1] = uiData;
            if (uiData == NOT_STARTED || uiData == DONE)
                DespawnSpectres();
            break;                
    }
        
    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}
    
uint32 instance_razorfen_downs::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case DATA_GONG_USE:
            return m_uiGongUse;
        case TYPE_TUTENKASH:
            return m_auiEncounter[0];
        case TYPE_AMNENNAR:
            return m_auiEncounter[1];
    }
    return 0;
}
    
void instance_razorfen_downs::DespawnSpectres()
{
    for(GUIDList::iterator itr = m_uiSpectreGUID.begin(); itr != m_uiSpectreGUID.end(); ++itr)
    {
        if (Creature* Spectre = instance->GetCreature(*itr))
            Spectre->RemoveFromWorld();
    }
}
    
void instance_razorfen_downs::DoEvent(int EventProgress)
{
	if (GameObject* Gong = GetSingleGameObjectFromStorage(GO_GONG))
    {
		Creature* pTemp;
		int SideSwitchCount;
		int SummonMob;
		int HowMany;
		int FastDespawn;
        switch(EventProgress)
        {
            case 1:
				HowMany = MAX_TOMB_FIEND;
				SideSwitchCount = 4;
				SummonMob = NPC_TOMB_FIEND;
				FastDespawn = 5000;
                break;
            case 2:
				HowMany = MAX_TOMB_REAVER;
				SideSwitchCount = 2;
				SummonMob = NPC_TOMB_REAVER;
				FastDespawn = 5000;
                break;
            case 3:
                SetData(DATA_GONG_USE, DONE);
				Gong->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
				HowMany = 1;
				SideSwitchCount = 1;
				SummonMob = NPC_TUTENKASH;
				FastDespawn = 180000;
                break;
		}

		for(int32 i = 0; i < HowMany; ++i)
		{
			int random = urand(-5,5);
			if(i < SideSwitchCount)
				pTemp = Gong->SummonCreature(SummonMob, Spawns[0][0]+random, Spawns[0][1]+random, Spawns[0][2], 0, TEMPSUMMON_DEAD_DESPAWN, FastDespawn);
			else
				pTemp = Gong->SummonCreature(SummonMob, Spawns[1][0]+random, Spawns[1][1]+random, Spawns[1][2], 0, TEMPSUMMON_DEAD_DESPAWN, FastDespawn);
			pTemp->SetInCombatWithZone();
        }
    }
}

void instance_razorfen_downs::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];
        
    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_razorfen_downs(Map* pMap)
{
    return new instance_razorfen_downs(pMap);
}

void AddSC_instance_razorfen_downs()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_razorfen_downs";
    pNewscript->GetInstanceData = &GetInstanceData_instance_razorfen_downs;
    pNewscript->RegisterSelf();
}
