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
SDName: Instance Zul'Farrak
SD%Complete: 80
SDComment:
SDCategory: Zul'Farrak
EndScriptData */

#include "precompiled.h"
#include "zulfarrak.h"

instance_zulfarrak::instance_zulfarrak(Map* pMap) : ScriptedInstance(pMap),
	m_uiCheckPyramideTrash_Timer(15000),
    m_uiBombExplodationExpire_Timer(0),
    m_uiBlysBandHeartstone_Timer(2*MINUTE*IN_MILLISECONDS),
	m_uiTrollsMovedCount(0),
	m_uiWave(0)
{
    Initialize();
};

void instance_zulfarrak::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

    m_uiPyramideTrash.clear();
	m_uiPyramideTrashTemp.clear();
	for (uint32 i = 0; i < 5; i++)
		m_uiPyramideNPCs[i].clear();
}

void instance_zulfarrak::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_GONG_OF_ZULFARRAK:
            if (m_auiEncounter[0] == DONE)
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
            break;
        case GO_END_DOOR:
            if (m_auiEncounter[2] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            break;
        case GO_DOOR_EXPLOSIVE:
			break;
		case GO_TROLL_CAGE1:
		case GO_TROLL_CAGE2:
		case GO_TROLL_CAGE3:
		case GO_TROLL_CAGE4:
		case GO_TROLL_CAGE5:
			break;

		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_zulfarrak::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
		case NPC_SNAKE:
			m_uiSnakes.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_WEEGLI_BLASTFUSE:
        case NPC_SERGEANT_BLY:
        case NPC_RAVEN:
        case NPC_ORO_EYEGOUGE:
        case NPC_MURTA_GRIMGUT:
        case NPC_NEKRUM_GUTCHEWER:
			m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
			break;
		case NPC_SANDFURY_EXECUTIONER:
			m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
			break;
        case NPC_DUSTWRAITH:
        case NPC_SANDARR_DUNEREAVER:
        case NPC_ZERILIS:
            SetRareBoss(pCreature, 75);
            break;
        case NPC_SANDFURY_SLAVE:
			m_uiPyramideNPCs[0].push_back(pCreature->GetObjectGuid());
			m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_SANDFURY_DRUDGE:
			m_uiPyramideNPCs[1].push_back(pCreature->GetObjectGuid());
			m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_SANDFURY_CRETIN:
			m_uiPyramideNPCs[4].push_back(pCreature->GetObjectGuid());
			m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_SANDFURY_ACOLYTE:
			m_uiPyramideNPCs[2].push_back(pCreature->GetObjectGuid());
			m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_SANDFURY_ZEALOT:
			m_uiPyramideNPCs[3].push_back(pCreature->GetObjectGuid());
			m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
			break;
        case NPC_SANDFURY_SOUL_EATER: //no spawns
        case NPC_SHADOWPRIEST_SEZZZIZZ:
            m_uiPyramideTrash.push_back(pCreature->GetObjectGuid());
            break;

    }
}

void instance_zulfarrak::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_GAHZRILLA:
            if (uiData == DONE)
				if (GameObject* Gong = GetSingleGameObjectFromStorage(GO_GONG_OF_ZULFARRAK))
                    Gong->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
            m_auiEncounter[0] = uiData;
            break;
        case TYPE_PYRAMIDE:
            if (uiData == IN_PROGRESS)
            {
                // Respawn Bly's crew if someone member is dead and set faction to "enemy for trolls, but friend with players"
                if (Creature* pWeegli = GetSingleCreatureFromStorage(NPC_WEEGLI_BLASTFUSE))
                {
                    if (pWeegli->isDead())
                        pWeegli->Respawn();
                    pWeegli->setFaction(FACTION_FFP_EFE);
                }
                if (Creature* pBly = GetSingleCreatureFromStorage(NPC_SERGEANT_BLY))
                {
                    if (pBly->isDead())
                        pBly->Respawn();
                    pBly->setFaction(FACTION_FFP_EFE);
                }
                if (Creature* pRaven = GetSingleCreatureFromStorage(NPC_RAVEN))
                {
                    if (pRaven->isDead())
                        pRaven->Respawn();
                    pRaven->setFaction(FACTION_FFP_EFE);
                }
                if (Creature* pOro = GetSingleCreatureFromStorage(NPC_ORO_EYEGOUGE))
                {
                    if (pOro->isDead())
                        pOro->Respawn();
                    pOro->setFaction(FACTION_FFP_EFE);
                }
                if (Creature* pMurta = GetSingleCreatureFromStorage(NPC_MURTA_GRIMGUT))
                {
                    if (pMurta->isDead())
                        pMurta->Respawn();
                    pMurta->setFaction(FACTION_FFP_EFE);
                }
            }
            m_auiEncounter[1] = uiData;
            break;
        case TYPE_WEEGLI:
            if (uiData == DONE)
                m_uiBombExplodationExpire_Timer = 10000;
            m_auiEncounter[2] = uiData;
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

uint32 instance_zulfarrak::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_zulfarrak::Load(const char* in)
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
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

void instance_zulfarrak::OnPlayerDeath(Player * /*pPlayer*/)
{
	//Wipe during pyramide event: reset!
	if (GetData(TYPE_PYRAMIDE) != IN_PROGRESS && GetData(TYPE_PYRAMIDE) != SPECIAL)
		return;
	Map::PlayerList const& L = instance->GetPlayers();
	uint32 i = 0;
	for (Map::PlayerList::const_iterator itr = L.begin(); itr != L.end(); ++itr) {
		Player* P = itr->getSource();
		if (!P || P->isDead() || !P->isAlive())
			i++;
	}
	if (i < L.getSize()-1)
		return;
    //Reset Event
	SetData(TYPE_PYRAMIDE, NOT_STARTED);
	//despawn summons
    if (!m_uiPyramideTrash.empty())
    {
        for(GUIDList::iterator itr = m_uiPyramideTrash.begin(); itr != m_uiPyramideTrash.end(); ++itr)
        {
			if (Creature* pTroll = instance->GetCreature(*itr))
            {
				pTroll->ForcedDespawn();
				pTroll->RemoveFromWorld();
            }
        }
    }
	m_uiPyramideTrash.clear();
	m_uiPyramideTrashTemp.clear();
	m_uiWave = 0;
	m_uiCheckPyramideTrash_Timer = 15000;
	//Reset troll cages and prisoners so event can be restarted
	for (int i = 0; i < 5; i++)
    {
		m_uiPyramideNPCs[i].clear();
		DoUseDoorOrButton(m_mGoEntryGuidStore[GO_TROLL_CAGE1+i],0,false);
		Creature* C = GetSingleCreatureFromStorage(NPC_SERGEANT_BLY+i);
		C->SetDeathState(JUST_DIED);
		C->Respawn();
		C->setFaction(FACTION_FRIENDLY);
    }
	//Set respawn time of executioner
	Creature* C = GetSingleCreatureFromStorage(NPC_SANDFURY_EXECUTIONER);
	if (C && C->IsInWorld() && !C->isAlive())
    {
        C->RemoveCorpse();
        C->SetRespawnTime(35);
        C->SaveRespawnTime();
        C->SetRespawnDelay(10 * 24 * 3600);
    }
	//Despawn Nekrum aswell
	C = GetSingleCreatureFromStorage(NPC_NEKRUM_GUTCHEWER);
	if (C && C->IsInWorld())
    {
		C->ForcedDespawn();
		C->RemoveFromWorld();
    }
}

void instance_zulfarrak::OnCreatureDeath(Creature * pCreature)
{
	if (GetData(TYPE_PYRAMIDE) != IN_PROGRESS && GetData(TYPE_PYRAMIDE) != SPECIAL)
		return;

	m_uiPyramideTrash.remove(pCreature->GetObjectGuid());
	m_uiPyramideTrashTemp.remove(pCreature->GetObjectGuid());
}

static uint32 spawnNumber[6][5] =
{ {0,0,0,0,2},
  {2,1,1,1,1},
  {2,2,2,1,0},
  {1,2,0,1,2},
  {2,2,1,1,2},

  {2,0,1,1,2}
//9,7,5,5,9
};

void instance_zulfarrak::Update(uint32 uiDiff)
{
    // Pyramide Trash
    if (GetData(TYPE_PYRAMIDE) == IN_PROGRESS || GetData(TYPE_PYRAMIDE) == SPECIAL)
    {
		if (GetData(TYPE_PYRAMIDE) == IN_PROGRESS && m_uiPyramideTrash.size() <= 15 && m_uiPyramideTrashTemp.empty())
			SetData(TYPE_PYRAMIDE, SPECIAL);	//Make Bly's crew walk downstairs

        if (m_uiCheckPyramideTrash_Timer <= uiDiff && m_uiWave < 6)
        {
            debug_log("SD0: Instance Zul'Farrak: Pyramide event: Sending 2 trolls up stairs.");

			for (uint32 i = 0; i < 5; i++) {
            if (!m_uiPyramideNPCs[i].empty())
			{
                for(GUIDList::iterator itr = m_uiPyramideNPCs[i].begin(); itr != m_uiPyramideNPCs[i].end(); ++itr)
				{
                    if (Creature* pTroll = instance->GetCreature(*itr))
					{
                        if (!pTroll->isDead() || pTroll->isAlive())
                        {
                            if (++m_uiTrollsMovedCount > spawnNumber[m_uiWave][i])
                            {
                                m_uiTrollsMovedCount = 0;
                                break;
                            }

                            // Move troll up Stairs
                            if (!pTroll->getVictim())
                            {
                                pTroll->GetMotionMaster()->Clear();
                                pTroll->GetMotionMaster()->MovePoint(1, 1886.31f, 1269.72f, 42.f);
								CreatureCreatePos pos(pTroll->GetMap(), 1886.31f, 1269.72f, 42.f, 0.0f);
								pTroll->SetSummonPoint(pos);
                            }

							m_uiPyramideTrashTemp.push_back(pTroll->GetObjectGuid());
                            itr = m_uiPyramideNPCs[i].erase(itr);
                        }
					}
				}
			}
			}
			m_uiWave++;
			m_uiCheckPyramideTrash_Timer = 40000;
		}
        else m_uiCheckPyramideTrash_Timer -= uiDiff;
	}

    // Nekrum death state checker
    if (GetData(TYPE_PYRAMIDE) != DONE)
	{
	    Creature* pNekrum = GetSingleCreatureFromStorage(NPC_NEKRUM_GUTCHEWER);
        if (pNekrum && pNekrum->isDead())
        {
            SetData(TYPE_PYRAMIDE, DONE);
            m_uiPyramideTrash.clear();
        }
	}

    // Bomb explodation
    if (m_uiBombExplodationExpire_Timer)
    {
        if (m_uiBombExplodationExpire_Timer <= uiDiff)
        {
            // Bomb explodation
            GameObject* DoorExplosive = GetSingleGameObjectFromStorage(GO_DOOR_EXPLOSIVE);
			if (DoorExplosive && DoorExplosive->GetGoState() != GO_STATE_ACTIVE_ALTERNATIVE)
				DoorExplosive->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            // Door opened by exploding bomb
            GameObject* EndDoor = GetSingleGameObjectFromStorage(GO_END_DOOR);
			if (EndDoor && EndDoor->GetGoState() != GO_STATE_ACTIVE_ALTERNATIVE)
				EndDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
        }
        else
			m_uiBombExplodationExpire_Timer -= uiDiff;
    }

    // Bly's crew use Heartstone after defeating trolls
    //if (m_uiBlysBandHeartstone_Timer)
    //    if ( (GetData(TYPE_PYRAMIDE) == DONE) && (m_uiBlysBandHeartstone_Timer <= uiDiff) )
    //    {
    //        Creature* pBly = GetSingleCreatureFromStorage(NPC_SERGEANT_BLY);
    //        if (pBly && pBly->isAlive() && !pBly->getVictim())
    //            pBly->CastSpell(pBly, SPELL_BLYS_BANDS_ESCAPE, false);
    //        Creature* pRaven = GetSingleCreatureFromStorage(NPC_RAVEN);
    //        if (pRaven && pRaven->isAlive() && !pRaven->getVictim())
    //            pRaven->CastSpell(pRaven, SPELL_BLYS_BANDS_ESCAPE, false);
    //        Creature* pOro = GetSingleCreatureFromStorage(NPC_ORO_EYEGOUGE);
    //        if (pOro && pOro->isAlive() && !pOro->getVictim())
    //            pOro->CastSpell(pOro, SPELL_BLYS_BANDS_ESCAPE, false);
    //        Creature* pMurta = GetSingleCreatureFromStorage(NPC_MURTA_GRIMGUT);
    //        if (pMurta && pMurta->isAlive() && !pMurta->getVictim())
    //            pMurta->CastSpell(pMurta, SPELL_BLYS_BANDS_ESCAPE, false);
    //
    //        m_uiBlysBandHeartstone_Timer = 0;
    //    }
    //    else m_uiBlysBandHeartstone_Timer -= uiDiff;
}

InstanceData* GetInstanceData_instance_zulfarrak(Map* pMap)
{
    return new instance_zulfarrak(pMap);
}

void AddSC_instance_zulfarrak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_zulfarrak";
    pNewScript->GetInstanceData = &GetInstanceData_instance_zulfarrak;
    pNewScript->RegisterSelf();
}
