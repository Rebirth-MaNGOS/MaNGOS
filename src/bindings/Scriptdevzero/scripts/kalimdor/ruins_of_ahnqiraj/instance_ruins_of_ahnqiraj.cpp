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
SDName: Instance_Ruins_of_Ahnqiraj
SD%Complete: 80
SDComment: 
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "ruins_of_ahnqiraj.h"

/* Ruins of Ahn'Qiraj encounters:
0 - Kurinnaxx
1 - General Rajaxx
2 - Moam
3 - Buru the Gorger
4 - Ayamiss the Hunter
5 - Ossirian the Unscarred */

instance_ruins_of_ahnqiraj::instance_ruins_of_ahnqiraj(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

void instance_ruins_of_ahnqiraj::Initialize()
{   
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

bool instance_ruins_of_ahnqiraj::IsEncounterInProgress() const
{
    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    return false;
}

void instance_ruins_of_ahnqiraj::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_OSSIRIAN_CRYSTAL:
            m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            break;
    }
}

void instance_ruins_of_ahnqiraj::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_KURINNAXX:
        case NPC_RAJAXX:
        case NPC_GENERAL_ANDOROV:
        case NPC_CAPTAIN_QEEZ:
        case NPC_CAPTAIN_TUUBID:
        case NPC_CAPTAIN_DRENN:
        case NPC_CAPTAIN_XURREM:
        case NPC_MAJOR_YEGGETH:
        case NPC_MAJOR_PAKKON:
        case NPC_COLONEL_ZERRAN:
        case NPC_MOAM:
        case NPC_BURU:
        case NPC_AYAMISS:
        case NPC_OSSIRIAN:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
		case NPC_BURU_EGG:
			m_lBuruEggs.push_back(pCreature->GetObjectGuid());	
			break;
		case NPC_TORNADO:
			m_lTornadoes.push_back(pCreature->GetObjectGuid());		
			pCreature->setFaction(35);
			pCreature->SetVisibility(VISIBILITY_OFF);
			break;
            
            // For mobs up til Rajaxx
        case NPC_QIRAJI_SWARMGUARD:
        //case NPC_HIVEZARA_WASP:     // some, but not all, both at ayamiss and rajaxx 
        //case NPC_HIVEZARA_STINGER:   // some, but not all, both at ayamiss and rajaxx 
        //case NPC_QIRAJI_GLADIATOR: // first 4 gladiators? rly,
            m_lRajaxxTrash.push_back(pCreature->GetObjectGuid());  
            break;
            // For mobs up til Buru
        case NPC_HIVEZARA_SANDSTALKER:
        case NPC_HIVEZARA_SOLDIER:
        //case NPC_FLESH_HUNTER:  // some, but not all, 
            m_lBuruTrash.push_back(pCreature->GetObjectGuid());  
            break;
            // For mobs up til Ayamiss
        case NPC_HIVEZARA_TAIL_LASHER:
        case NPC_HIVEZARA_COLLECTOR:
        case NPC_HIVEZARA_DRONE:
       // case NPC_HIVEZARA_WASP:       // some, but not all, both at ayamiss and rajaxx 
        //case NPC_HIVEZARA_STINGER:    // some, but not all, both at ayamiss and rajaxx 
            m_lAyamissTrash.push_back(pCreature->GetObjectGuid());  
            break;
        // For mobs up til Moam
        case NPC_SHRIEKER_SCARAB:
        case NPC_SPITTING_SCARAB:
        case NPC_OBSIDIAN_DESTROYER:
        //case NPC_FLESH_HUNTER:  // some, but not all
            m_lMoamTrash.push_back(pCreature->GetObjectGuid());  
            break;
            // For mobs up til Ossirian
        case NPC_ANUBISATH_GUARDIAN:
            m_lOssirianTrash.push_back(pCreature->GetObjectGuid());  
            break;
    }
}

void instance_ruins_of_ahnqiraj::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_KURINNAXX:
            m_auiEncounter[0] = uiData;
            if (uiData == DONE)
            {
                DoOrSimulateScriptTextForThisInstance(YELL_OSSIRIAN_BREACHED, NPC_OSSIRIAN);
            }
            break;
        case TYPE_RAJAXX:           // no trash respawn after Rajaxx is dead
            m_auiEncounter[1] = uiData;
            if (uiData == DONE)
            {
                if (!m_lRajaxxTrash.empty())
                {
                    for(GUIDList::iterator itr = m_lRajaxxTrash.begin(); itr != m_lRajaxxTrash.end(); ++itr)
                    {
                        if (Creature* pTrash = instance->GetCreature(*itr))
                        {
                            pTrash->SetRespawnDelay(604800);
                            if (!pTrash->isAlive())
                            {
                                pTrash->SetRespawnTime(604800);
                                pTrash->SaveRespawnTime();
                            }
                        }
                    }
                }
            }
            break;
        case TYPE_MOAM:           // no trash respawn after Moam is dead
            m_auiEncounter[2] = uiData;
            if (uiData == DONE)
            {
                if (!m_lMoamTrash.empty())
                {
                    for(GUIDList::iterator itr = m_lMoamTrash.begin(); itr != m_lMoamTrash.end(); ++itr)
                    {
                        if (Creature* pTrash = instance->GetCreature(*itr))
                        {
                            pTrash->SetRespawnDelay(604800);
                            if (!pTrash->isAlive())
                            {
                                pTrash->SetRespawnTime(604800);
                                pTrash->SaveRespawnTime();
                            }
                        }
                    }
                }
            }
            break;
        case TYPE_BURU:           // no trash respawn after Buru is dead
            m_auiEncounter[3] = uiData;
			if (uiData == DONE)					// If Buru is dead despawn the eggs that are alive and set the respawn of all eggs to 1 week
			{
				if (!m_lBuruEggs.empty())
				{
					for(GUIDList::iterator itr = m_lBuruEggs.begin(); itr != m_lBuruEggs.end(); ++itr)
					{
						if (Creature* pTarget = instance->GetCreature(*itr))
						{
							if (pTarget->isAlive())
								pTarget->ForcedDespawn();

							pTarget->SetRespawnTime(604800);
							pTarget->SaveRespawnTime();
						}
					}				
                    if (!m_lBuruTrash.empty())
                    {
                        for(GUIDList::iterator itr = m_lBuruTrash.begin(); itr != m_lBuruTrash.end(); ++itr)
                        {
                            if (Creature* pTrash = instance->GetCreature(*itr))
                            {
                                pTrash->SetRespawnDelay(604800);
                                if (!pTrash->isAlive())
                                {
                                    pTrash->SetRespawnTime(604800);
                                    pTrash->SaveRespawnTime();
                                }
                            }
                        }
                    }
				}
			}
            break;
        case TYPE_AYAMISS:           // no trash respawn after Ayamiss is dead
            m_auiEncounter[4] = uiData;
            if (uiData == DONE)
            {
                if (!m_lAyamissTrash.empty())
                {
                    for(GUIDList::iterator itr = m_lAyamissTrash.begin(); itr != m_lAyamissTrash.end(); ++itr)
                    {
                        if (Creature* pTrash = instance->GetCreature(*itr))
                        {
                            pTrash->SetRespawnDelay(604800);
                            if (!pTrash->isAlive())
                            {
                                pTrash->SetRespawnTime(604800);
                                pTrash->SaveRespawnTime();
                            }
                        }
                    }
                }
            }
            break;
        case TYPE_OSSIRIAN:           // no trash respawn after Ossirian is dead
            m_auiEncounter[5] = uiData;
            if (uiData == DONE)
            {
                if (!m_lOssirianTrash.empty())
                {
                    for(GUIDList::iterator itr = m_lOssirianTrash.begin(); itr != m_lOssirianTrash.end(); ++itr)
                    {
                        if (Creature* pTrash = instance->GetCreature(*itr))
                        {
                            pTrash->SetRespawnDelay(604800);
                            if (!pTrash->isAlive())
                            {
                                pTrash->SetRespawnTime(604800);
                                pTrash->SaveRespawnTime();
                            }
                        }
                    }
                }
            }
            break;
    }

    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;
}

uint32 instance_ruins_of_ahnqiraj::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_ruins_of_ahnqiraj::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_ruins_of_ahnqiraj(Map* pMap)
{
    return new instance_ruins_of_ahnqiraj(pMap);
}

void AddSC_instance_ruins_of_ahnqiraj()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_ruins_of_ahnqiraj";
    pNewscript->GetInstanceData = &GetInstanceData_instance_ruins_of_ahnqiraj;
    pNewscript->RegisterSelf();
}
