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
SDName: Instance_Onyxias_Lair
SD%Complete: 80
SDComment: 
SDCategory: Onyxia's Lair
EndScriptData */

#include "precompiled.h"
#include "onyxias_lair.h"
#include "TemporarySummon.h"

/* Onyxia's Lair encounters:
0 - Onyxia */

instance_onyxias_lair::instance_onyxias_lair(Map* pMap) : ScriptedInstance(pMap)
{
	Initialize();
}

void instance_onyxias_lair::Initialize()
{   
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_uiOnyxiaWarderGUID.clear();
	m_lavaFissures.clear();
}

bool instance_onyxias_lair::IsEncounterInProgress() const
{
    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    return false;
}

void instance_onyxias_lair::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_ONYXIA:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_ONYXIA_WARDER:
            m_uiOnyxiaWarderGUID.push_back(pCreature->GetObjectGuid());
            break;
		case NPC_ONYXIA_WHELP:
		case NPC_ONYXIA_WHELP_WEST:
			{
				TemporarySummon * summonedWhelp = dynamic_cast<TemporarySummon*>(pCreature);

				if (summonedWhelp != NULL)
				{
					m_onyxianWhelps.push_back(summonedWhelp);

					Unit* pOnyxia = GetSingleCreatureFromStorage(NPC_ONYXIA);

					if (pOnyxia && summonedWhelp->GetSummonerGuid() == pOnyxia->GetObjectGuid())
					{
						//Hard-summoned whelps- start them pathing & set them up to aggro on LOS
						summonedWhelp->GetMotionMaster()->MoveWaypoint();
					} else
					{
						//Egg-summoned whelps- aggro them now
						summonedWhelp->SetInCombatWithZone();
					}
				}
				
				break;
			}
		case NPC_ONYXIA_TRIGGER:
			this->m_mNpcEntryGuidStore[NPC_ONYXIA_TRIGGER] = pCreature->GetObjectGuid();
			break;
    }
}

void instance_onyxias_lair::OnCreatureDeath(Creature *creature)
{
    switch(creature->GetEntry())
    {
	case NPC_ONYXIA_WHELP:
	case NPC_ONYXIA_WHELP_WEST:
		//Now onyxia doesn't have to despawn it!
		TemporarySummon *summonedWhelp = dynamic_cast<TemporarySummon*>(creature);

		if (summonedWhelp != NULL)
		{
			m_onyxianWhelps.remove(summonedWhelp);
		}
		break;
	}
}

void instance_onyxias_lair::OnObjectCreate(GameObject *gameObject)
{
	switch(gameObject->GetEntry())
	{
	case GO_WHELP_EGG:
		m_eggGuids.push_back(gameObject->GetObjectGuid());
		break;
	default:
		uint32 counter = gameObject->GetObjectGuid().GetCounter();
		if (counter >= 82053 && counter <= 82106)
		{
			m_lavaFissures.push_back(gameObject);
		}
	}
}

void instance_onyxias_lair::ShuffleFissures()
{
	//Fill the live lava guids lists- find a random fissure and just fill them in order
	//Keeps the fissures grouped together and makes it seem less random
	uint32 fissureCount = m_lavaFissures.size();
	int firstLavaFissureIndex = urand(0,fissureCount-1);
	while (firstLavaFissureIndex > 0)
	{
		GameObject *itemToMove = m_lavaFissures.front();
		m_lavaFissures.pop_front();
		m_lavaFissures.push_back(itemToMove);
		firstLavaFissureIndex--;
	}


	for(int liveList = 0; liveList < 3; liveList++)
	{
		m_liveLavaFissures[liveList].clear();
		for(uint32 fissure=0; fissure < ((liveList==0)?(fissureCount/3 + fissureCount%3):(fissureCount/3)); fissure++)
		{
			GameObject *liveItem = m_lavaFissures.front();
			m_lavaFissures.pop_front();
			m_lavaFissures.push_back(liveItem);

            if (fissure < fissureCount/8)
			    m_liveLavaFissures[liveList].push_back(liveItem);
		}
	}
}

bool instance_onyxias_lair::FissureSurge(Creature *onyxia)
{
	if (m_liveLavaFissures[0].empty() && m_liveLavaFissures[1].empty() && m_liveLavaFissures[2].empty())
		return false;

    GameObject *firstFissure = NULL;
    GameObject *secondFissure = NULL;
	for (int liveList=0; liveList < 3; liveList++)
	{
		if (!m_liveLavaFissures[liveList].empty())
		{
			GameObject *fissure = m_liveLavaFissures[liveList].front();
			m_liveLavaFissures[liveList].pop_front();

            if (liveList == 1 && firstFissure != NULL && firstFissure->IsWithinDist2d(fissure->GetPositionX(),fissure->GetPositionY(),10))
                continue;

            if (liveList == 2 && secondFissure != NULL && secondFissure->IsWithinDist2d(fissure->GetPositionX(),fissure->GetPositionY(),10))
                continue;

            if (fissure->GetGoState() == GO_STATE_READY)
            {
				onyxia->CastSpell(fissure->GetPositionX(),fissure->GetPositionY(),fissure->GetPositionZ(),SPELL_ERUPTION,true);

                if (liveList == 0)
                    firstFissure = fissure;
                else if (liveList == 1)
                    secondFissure = fissure;
            } else
                m_liveLavaFissures[liveList].push_back(fissure);
		}
	}

	return true;
}

void instance_onyxias_lair::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_ONYXIA:
            m_auiEncounter[0] = uiData;
            if (uiData == DONE)
            {
                // Despawn Onyxia Warders
                if (!m_uiOnyxiaWarderGUID.empty())
                    for (GUIDList::iterator itr = m_uiOnyxiaWarderGUID.begin(); itr != m_uiOnyxiaWarderGUID.end(); ++itr)
                        if (Creature* pWarder = instance->GetCreature(*itr))
                            pWarder->ForcedDespawn();
            } else if (uiData == FAIL)
			{
				//Depspawn summoned whelps
				if (!m_onyxianWhelps.empty())
					for(std::list<TemporarySummon*>::const_iterator itr = m_onyxianWhelps.begin(); itr != m_onyxianWhelps.end(); ++itr)
						if ( (*itr)->IsInWorld())
							(*itr)->UnSummon();

				m_onyxianWhelps.clear();

				//Respawn eggs
				if (!m_eggGuids.empty())
					for(GUIDList::iterator itr = m_eggGuids.begin(); itr != m_eggGuids.end(); ++itr)
						if (GameObject *egg = instance->GetGameObject(*itr))
							egg->Respawn();
			}
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

uint32 instance_onyxias_lair::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_onyxias_lair::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_onyxias_lair(Map* pMap)
{
    return new instance_onyxias_lair(pMap);
}

void AddSC_instance_onyxias_lair()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_onyxias_lair";
    pNewScript->GetInstanceData = &GetInstanceData_instance_onyxias_lair;
    pNewScript->RegisterSelf();
}
