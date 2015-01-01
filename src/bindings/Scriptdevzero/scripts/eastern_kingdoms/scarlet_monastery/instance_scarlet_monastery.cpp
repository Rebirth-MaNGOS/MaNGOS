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
SDName: Instance_Scarlet_Monastery
SD%Complete: 90
SDComment: correct Ashbringer event
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"
#include "scarlet_monastery.h"

instance_scarlet_monastery::instance_scarlet_monastery(Map* pMap) : ScriptedInstance(pMap),
	m_bRareBossSpawned(false),
    m_bMograinesWelcomeYell(false),
	m_uiRareBossesCounter(0)
{
	Initialize();
};

void instance_scarlet_monastery::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

	lCathedralMobs.clear();
}

Player* instance_scarlet_monastery::GetAshbringerPlayerInInstance()
{
	Map::PlayerList const& PlayerList = instance->GetPlayers();

	if (!PlayerList.isEmpty())
        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
			    if (pPlayer->HasAura(SPELL_ASHBRINGER))
				    return pPlayer;
	return NULL;
}

void instance_scarlet_monastery::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_MOGRAINE:
			if (m_auiEncounter[1] == DONE)
				pCreature->SetStandState(UNIT_STAND_STATE_DEAD);
			break;
        case NPC_WHITEMANE:
        case NPC_VORREL:
		case NPC_FAIRBANKS:
			break;
        case NPC_FALLEN_CHAMPION:
        case NPC_IRONSPINE:
        case NPC_AZSHIR_THE_SLEEPLESS:
            // We want only one rare spawn
            if (!m_bRareBossSpawned)
            {
                if (SetRareBoss(pCreature, 30))
                    m_bRareBossSpawned = true;
                else
                {
                    if (++m_uiRareBossesCounter == 3)
                        SetRareBoss(pCreature, 100);
                }
            }
            return;
		default:
			return;

		/*case NPC_SCARLET_SORCERER:
		case NPC_SCARLET_MYRMIDON:
		case NPC_SCARLET_DEFENDER:
		case NPC_SCARLET_WIZARD:
		case NPC_SCARLET_CENTURION:
		case NPC_SCARLET_CHAMPION:
		case NPC_SCARLET_ABBOT:
		case NPC_SCARLET_MONK:
		case NPC_SCARLET_CHAPLAIN:
            lCathedralMobs.push_back(pCreature->GetObjectGuid());
            break;*/
    }

	m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_scarlet_monastery::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_HIGH_INQUISITORS_DOOR:
        case GO_CHAPEL_DOOR:
			m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
            /*if (GetAshbringerPlayerInInstance())
            {
                pGo->SetGoState(GO_STATE_ACTIVE);
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
            }*/
            break;
    }
}

void instance_scarlet_monastery::SetData(uint32 uiType, uint32 uiData)
{
	debug_log("SD0: Instance Scarlet Monastery: SetData update (Type: %u Data %u)", uiType, uiData);

	switch(uiType)
	{
		case TYPE_MOGRAINE_AND_WHITE_EVENT:
            m_auiEncounter[0] = uiData;
            switch(uiData)
            {
                case IN_PROGRESS:
                case FAIL:
                    DoUseDoorOrButton(GO_HIGH_INQUISITORS_DOOR);
            }
			break;
		case TYPE_ASHBRINGER_EVENT:
            m_auiEncounter[1] = uiData;
			if (uiData == DONE)
            {
				if (Creature* pFairbanks = GetSingleCreatureFromStorage(NPC_FAIRBANKS))
                {
					pFairbanks->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    //TODO: change model to Human Fairbanks
                }
            }
			break;
        case TYPE_ASH_WELCOME_YELL:
            m_auiEncounter[2] = uiData;
            break;
	}

	if (uiData == DONE)
	{
		OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1];// << " " << m_auiEncounter[2];

        strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
	}
}

uint32 instance_scarlet_monastery::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_scarlet_monastery::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1];// >> m_auiEncounter[2];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_scarlet_monastery(Map* pMap)
{
    return new instance_scarlet_monastery(pMap);
}

void AddSC_instance_scarlet_monastery()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_scarlet_monastery";
    pNewScript->GetInstanceData = &GetInstanceData_instance_scarlet_monastery;
    pNewScript->RegisterSelf();
}
