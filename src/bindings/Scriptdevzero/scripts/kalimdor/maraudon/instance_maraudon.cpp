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
SDName: Instance_Maraudon
SD%Complete:
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"
#include "maraudon.h"

instance_maraudon::instance_maraudon(Map* pMap) : ScriptedInstance(pMap)
{
	m_uiSpawnTimer = 5000;
    Initialize();
}

void instance_maraudon::Initialize()
{
	memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_maraudon::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CELEBRAS_THE_REDEEMED:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            if (m_auiEncounter[0] != DONE)
            {
                pCreature->SetVisibility(VISIBILITY_OFF);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            break;
        /*case NPC_MESHLOK_THE_HARVESTER:	// handled in the pool instead
            SetRareBoss(pCreature, 50);
            break;*/
		//case NPC_PATROL:
		//	for (int i = 0; i < MAX_PATROL; i++)
		//		if (pCreature->GetGUIDLow() == PATROL_GUID_START+i)
		//			PatrolerGuid[i] = pCreature->GetObjectGuid();
		//	break;
	}

	//extra code for patrols, currently unused
	//switch (pCreature->GetGUIDLow())
	//{
	//	case 53990:
	//	case 53991:
	//		PatrolerList[0].push_back(pCreature->GetObjectGuid());
	//		break;

	//	case 54128:
	//	case 54129: 
	//		PatrolerList[1].push_back(pCreature->GetObjectGuid());
	//		break;

	//	case 54326:
	//	case 54327:
	//		PatrolerList[2].push_back(pCreature->GetObjectGuid());
	//		break;

	//	case 54566:
	//	case 54567:
	//		PatrolerList[3].push_back(pCreature->GetObjectGuid());
	//		break;

	//	case 54574:
	//	case 54575:
	//		PatrolerList[4].push_back(pCreature->GetObjectGuid());
	//		break;
	//}
}

void instance_maraudon::OnObjectCreate(GameObject* pGo)
{
	if (pGo->GetEntry() == GO_LARVA_SPEWER)
		m_uiLarvaSpewerGUID = pGo->GetObjectGuid();
}

void instance_maraudon::Update(uint32 uiDiff)
{
	if (m_uiSpawnTimer <= uiDiff)
	{
		m_uiSpawnTimer = 5000;
		GameObject* pGo = instance->GetGameObject(m_uiLarvaSpewerGUID);
		if (!pGo || pGo->GetGoState() == GO_STATE_ACTIVE)
			return;
		Map::PlayerList const& L = instance->GetPlayers();
		if (L.isEmpty())
			return;
		bool spawn = false;
		for (Map::PlayerList::const_iterator i = L.begin(); i != L.end(); ++i)
		{
			Player* P = i->getSource();
			if (P && P->isAlive() && P->GetDistance(pGo) <= 45.0f)
			{
				spawn = true;
				break;
			}
		}
		if (spawn)
		{
			float coord_dif[2];
			for (int i = 0; i < 2; i++)
				coord_dif[i] = (float)urand(-3,4);
			if (Creature* C = pGo->SummonCreature(NPC_LARVA,pGo->GetPositionX()+coord_dif[0],pGo->GetPositionY()+coord_dif[1],pGo->GetPositionZ(),pGo->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN,3000))
				C->SetInCombatWithZone();
			//Unit* target = C->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
			//if (target && C->AI())
			//	C->AI()->AttackStart(target);
		}
	}
	else
	  m_uiSpawnTimer -= uiDiff;
}

void instance_maraudon::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_CELEBRAS_THE_CURSED:
            m_auiEncounter[0] = uiData;
            if (uiData == DONE)
            {
				if (Creature* Celebras = GetSingleCreatureFromStorage(NPC_CELEBRAS_THE_REDEEMED))
                {
                    Celebras->SetVisibility(VISIBILITY_ON);
                    Celebras->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
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

uint32 instance_maraudon::GetData(uint32 uiType)
{
	if (uiType < MAX_ENCOUNTER)
		return m_auiEncounter[uiType];

	return 0;
}

void instance_maraudon::Load(const char* in)
{
    if (!in)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(in);

    std::istringstream loadStream(in);
    loadStream >> m_auiEncounter[0];
        
    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }
        
    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_maraudon(Map* pMap)
{
    return new instance_maraudon(pMap);
}

void AddSC_instance_maraudon()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "instance_maraudon";
    pNewscript->GetInstanceData = &GetInstanceData_instance_maraudon;
    pNewscript->RegisterSelf();
}
