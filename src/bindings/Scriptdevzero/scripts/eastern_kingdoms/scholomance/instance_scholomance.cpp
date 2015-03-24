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
SDName: Instance_Scholomance
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

/*
    Encounter 0 = Kirtonos the Herald
    Encounter 1 = Jandice Barow
    Encounter 2 = Great Ossuary Event (Q: paladin, shaman)
    Encounter 3 = Ras Frostwhipser
    Encounter 4 = Kormok
    Encounter 5-10 = Minibosses - Darkmasters' Chambers
    Encounter 11 = Darkmaster Gandling
    Encounter 12 = Viewing Room Door
*/

#include "precompiled.h"
#include "scholomance.h"

instance_scholomance::instance_scholomance(Map* pMap) : ScriptedInstance(pMap),
    m_uiWaveCount(0),
    m_uiViewingRoomTimer(0)
{
	m_bPaladin = true;
    Initialize();
}

void instance_scholomance::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_scholomance::HandleGate(uint32 uiEntry, uint32 uiData, bool bTrySummon)
{
    if (GameObject* pGo = GetSingleGameObjectFromStorage(uiEntry))
    {
        if (uiData == IN_PROGRESS)
            pGo->SetGoState(GO_STATE_READY);
        else
        {
            pGo->SetGoState(GO_STATE_ACTIVE);
                
            if (bTrySummon)
                TrySummonGandling(pGo);
        }
    }
}

void instance_scholomance::OssuaryStartEvent()
{
    if (GetData(TYPE_GREAT_OSSUARY) == FAIL)
        return;

	if(m_bPaladin)
	{
		if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_GREAT_OSSUARY))			// spawn npc that starts the paladin event
		{
			float fX, fY, fZ;
			pGo->GetPosition(fX, fY, fZ);
			pGo->SummonCreature(NPC_PALADIN_EVENT, fX, fY, fZ, 0, TEMPSUMMON_DEAD_DESPAWN, 60000);
			m_bPaladin = false;
		}
	}
}

void instance_scholomance::TrySummonGandling(GameObject* pGo)
{
    if (!pGo || m_auiEncounter[11] == DONE)
        return;

	// No more than one time
	if (GetSingleCreatureFromStorage(NPC_DARKMASTER_GANDLING))
        return;

    for(uint32 i = 5; i <= 10; ++i)
    {
        if (m_auiEncounter[i] != DONE)
            return;
    }

    pGo->SummonCreature(NPC_DARKMASTER_GANDLING, 180.73f, -9.43856f, 75.507f, 1.61399f, TEMPSUMMON_DEAD_DESPAWN, 120000);
}

void instance_scholomance::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_BRAZIER_OF_THE_HERALD:
            if (m_auiEncounter[0] == DONE)
                InteractWithGo(GO_BRAZIER_OF_THE_HERALD, false);
            break;
        case GO_GATE_KIRTONOS:
            HandleGate(GO_GATE_KIRTONOS, m_auiEncounter[0], false);
            break;
        case GO_JOURNAL_OF_JANDICE_BAROV:
        case GO_GREAT_OSSUARY:
            break;
        case GO_GATE_GREAT_OSSUARY:
            HandleGate(GO_GATE_GREAT_OSSUARY, m_auiEncounter[2], false);
            break;
        case GO_GATE_RAS_FROSTWHISPER:
            HandleGate(GO_GATE_RAS_FROSTWHISPER, m_auiEncounter[3], false);
            break;
        case GO_VIEWING_ROOM_DOOR:
            if (m_auiEncounter[12] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        // Darkmasters' Chambers
        case GO_GATE_POLKELT:
            HandleGate(GO_GATE_POLKELT, m_auiEncounter[5], false);
            break;
        case GO_GATE_THEOLEN:
            HandleGate(GO_GATE_THEOLEN, m_auiEncounter[6], false);
            break;
        case GO_GATE_MALICIA:
            HandleGate(GO_GATE_MALICIA, m_auiEncounter[7], false);
            break;
        case GO_GATE_ILLUCIA:
            HandleGate(GO_GATE_ILLUCIA, m_auiEncounter[8], false);
            break;
        case GO_GATE_ALEXEI:
            HandleGate(GO_GATE_ILLUCIA, m_auiEncounter[9], false);
            break;
        case GO_GATE_RAVENIAN:
            HandleGate(GO_GATE_ILLUCIA, m_auiEncounter[10], false);
            break;
        case GO_GATE_GANDLING:
            HandleGate(GO_GATE_GANDLING, m_auiEncounter[11], true);
            break;
        case GO_DAWNS_GAMBIT_1:
            pGo->SetGoState(GO_STATE_ACTIVE);
            m_uiViewingRoomTimer = 2500;
            break;
        case GO_DAWNS_GAMBIT_2:
            break;
		default:
			return;
    }

	m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_scholomance::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_BLOOD_STEWARD_OF_KIRTONOS:
            if (m_auiEncounter[0] == IN_PROGRESS && pCreature->isDead())
                pCreature->Respawn();
            break;
		case NPC_VECTUS_PASSIVE:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
		case NPC_MARDUK_BLACKPOOL:
			m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
			break;
    }
}

void instance_scholomance::OnCreatureEnterCombat(Creature * pCreature)
{
	if (pCreature->GetEntry() != NPC_SCHOLOMANCE_STUDENT || pCreature->getFaction() == FACTION_HOSTILE)
		return;

    std::list<Creature*> m_lStudent;
    GetCreatureListWithEntryInGrid(m_lStudent, pCreature, NPC_SCHOLOMANCE_STUDENT, DEFAULT_VISIBILITY_INSTANCE);
    if (!m_lStudent.empty())
    {
        for(std::list<Creature*>::iterator itr = m_lStudent.begin(); itr != m_lStudent.end(); ++itr)
        {
            if ((*itr) && (*itr)->isAlive())
                (*itr)->setFaction(FACTION_HOSTILE);
        }
    }
}

void instance_scholomance::Update(uint32 uiDiff)
{
    if (m_uiViewingRoomTimer)
    {
        // Vectus Yell
        if (m_uiViewingRoomTimer <= uiDiff)
        {
			if (Creature* pVectus = GetSingleCreatureFromStorage(NPC_VECTUS_PASSIVE))
            {
                pVectus->MonsterYell(SAY_VECTUS_DAWN_GAMBIT, LANG_UNIVERSAL, NULL);
				pVectus->UpdateEntry(NPC_VECTUS);
				pVectus->setFaction(FACTION_HOSTILE);
                //DoScriptText(SAY_VECTUS_DAWN_GAMBIT, pVectus);

				if(Creature* pMarduk = GetSingleCreatureFromStorage(NPC_MARDUK_BLACKPOOL))
					pMarduk->setFaction(FACTION_HOSTILE);

                if (GameObject* GoDawnGambit1 = GetSingleGameObjectFromStorage(GO_DAWNS_GAMBIT_1))
                    GoDawnGambit1->AddObjectToRemoveList();
                if (GameObject* GoDawnGambit2 = GetSingleGameObjectFromStorage(GO_DAWNS_GAMBIT_2))
                    GoDawnGambit2->AddObjectToRemoveList();

				std::list<Creature*> m_lStudent;
				GetCreatureListWithEntryInGrid(m_lStudent, pVectus, NPC_SCHOLOMANCE_STUDENT, DEFAULT_VISIBILITY_INSTANCE);
				if (!m_lStudent.empty())
				{
					for(std::list<Creature*>::iterator itr = m_lStudent.begin(); itr != m_lStudent.end(); ++itr)
					{
						if ((*itr) && (*itr)->isAlive())
						{
							(*itr)->AI()->DoCastSpellIfCan((*itr), 18115, CastFlags::CAST_INTERRUPT_PREVIOUS); // transformation
							(*itr)->setFaction(FACTION_HOSTILE);
							(*itr)->GetMotionMaster()->MoveRandom();
						}
					}
				}
            }
            m_uiViewingRoomTimer = 0;
        }
        else
            m_uiViewingRoomTimer -= uiDiff;
    }
}

void instance_scholomance::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {    
        case TYPE_KIRTONOS_THE_HERALD:
            m_auiEncounter[0] = uiData;
            HandleGate(GO_GATE_KIRTONOS, uiData, false);
            if (uiData == NOT_STARTED)
            {
                HandleGameObject(GO_BRAZIER_OF_THE_HERALD, false);
                InteractWithGo(GO_BRAZIER_OF_THE_HERALD, true);
            }
            break;
        case TYPE_JANDICE_BAROV:
            m_auiEncounter[1] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(GO_JOURNAL_OF_JANDICE_BAROV, HOUR);
            break;
        case TYPE_GREAT_OSSUARY:
            m_auiEncounter[2] = uiData;
            HandleGate(GO_GATE_GREAT_OSSUARY, uiData, false);
            break;
        case TYPE_RAS_FROSTWHISPER:
            m_auiEncounter[3] = uiData;
            HandleGate(GO_GATE_RAS_FROSTWHISPER, uiData, false);
            break;
        case TYPE_KORMOK:
            m_auiEncounter[4] = uiData;
            break;
        case TYPE_LOREKEEPER_POLKELT:
            m_auiEncounter[5] = uiData;
            HandleGate(GO_GATE_POLKELT, uiData, true);
            break;
        case TYPE_DOCTOR_THEOLEN_KRASTINOV:
            m_auiEncounter[6] = uiData;
            HandleGate(GO_GATE_THEOLEN, uiData, true);
            break;
        case TYPE_INSTRUCTOR_MALICIA:
            m_auiEncounter[7] = uiData;
            HandleGate(GO_GATE_MALICIA, uiData, true);
            break;
        case TYPE_LADY_ILLUCIA_BAROV:
            m_auiEncounter[8] = uiData;
            HandleGate(GO_GATE_ILLUCIA, uiData, true);
            break;
        case TYPE_LORD_ALEXEI_BAROV:
            m_auiEncounter[9] = uiData;
            HandleGate(GO_GATE_ALEXEI, uiData, true);
            break;
        case TYPE_THE_RAVENIAN:
            m_auiEncounter[10] = uiData;
            HandleGate(GO_GATE_RAVENIAN, uiData, true);
            break;
        case TYPE_DARKMASTER_GANDLING:
            m_auiEncounter[11] = uiData;
            HandleGate(GO_GATE_GANDLING, uiData, false);
            break;
        case TYPE_VIEWING_ROOM_DOOR:
            m_auiEncounter[12] = uiData;
            break;
    }

    OUT_SAVE_INST_DATA;

    std::ostringstream saveStream;
    saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " " << m_auiEncounter[9] << " " << m_auiEncounter[10] << " " << m_auiEncounter[11] << " " << m_auiEncounter[12];

    strInstData = saveStream.str();

    SaveToDB();
    OUT_SAVE_INST_DATA_COMPLETE;
}

uint32 instance_scholomance::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_scholomance::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7] >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10] >> m_auiEncounter[11] >> m_auiEncounter[12];

    for(uint32 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS && i != 0)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_scholomance(Map* pMap)
{
    return new instance_scholomance(pMap);
}

void AddSC_instance_scholomance()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_scholomance";
    pNewscript->GetInstanceData = &GetInstanceData_instance_scholomance;
    pNewscript->RegisterSelf();
}
