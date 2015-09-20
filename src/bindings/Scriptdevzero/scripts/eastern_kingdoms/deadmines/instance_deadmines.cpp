/*
* Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
* Copyright (C) 2010-2011 ScriptDev0 <http://github.com/scriptdev/scriptdevzero>
*
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
SDName: Instance_Deadmines
SD%Complete: 0
SDComment: Placeholder
SDCategory: Deadmines
EndScriptData */

#include "precompiled.h"
#include "deadmines.h"

instance_deadmines::instance_deadmines(Map* pMap) : ScriptedInstance(pMap),
    m_uiIronDoorTimer(0),
    m_uiDoorStep(0)
{
    Initialize();
}

void instance_deadmines::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_deadmines::OnPlayerEnter(Player* pPlayer)
{
    // Respawn the Mysterious chest if one of the players who enter the instance has the quest in his log
    if (pPlayer->GetQuestStatus(QUEST_FORTUNE_AWAITS) == QUEST_STATUS_COMPLETE &&
        !pPlayer->GetQuestRewardStatus(QUEST_FORTUNE_AWAITS))
    {
        DoRespawnGameObject(GO_MYSTERIOUS_CHEST, HOUR);
    }
}

void instance_deadmines::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
	{
		case NPC_MR_SMITE:
			m_mNpcEntryGuidStore[NPC_MR_SMITE] = pCreature->GetObjectGuid();
			break;
		case NPC_DEFIAS_BLACKGUARD:
            //Smite's guards should be visible/attackable
            switch (pCreature->GetObjectGuid().GetCounter())
            {
            case NPC_GUID_GUARD1:
            case NPC_GUID_GUARD2:
            case NPC_GUID_KEG_ATTACKER:
                break;
            default:
                {
			        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
			        pCreature->SetVisibility(VISIBILITY_OFF);
                }
            }
			break;
	}

    uint32 counter = pCreature->GetObjectGuid().GetCounter();

    //HACK - I'm too sleepy to think of a better way of doing this, but here are all the creatures that should be inactive at start and spawn after
    //boss death
    switch (counter)
    {
    case 79373:
    case 79374:
    case 79244:
    case 79177:
    case 79245:
    case 79283:
    case 79284:
    case 79285:
    case 3000648:
        pCreature->ForcedDespawn(250);
    default:
        break;
    }
}

void instance_deadmines::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_FACTORY_DOOR:
            if (GetData(TYPE_RHAHKZOR) == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);

            break;
        case GO_MAST_ROOM_DOOR:
            if (GetData(TYPE_SNEED) == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);

            break;
        case GO_FOUNDRY_DOOR:
            if (GetData(TYPE_GILNID) == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);

            break;
        case GO_IRON_CLAD_DOOR:
        case GO_DEFIAS_CANNON:
        case GO_SMITE_CHEST:
		case GO_MYSTERIOUS_CHEST:
            break;

        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_deadmines::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_RHAHKZOR:
            SetData(TYPE_RHAHKZOR, DONE);
            break;
		case NPC_SNEEDS_SHREDDER:
			pCreature->SummonCreature(NPC_SNEED, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), 0.0f, TEMPSUMMON_DEAD_DESPAWN, 1200000);
			break;
        case NPC_SNEED:
            SetData(TYPE_SNEED, DONE);
            break;
        case NPC_GILNID:
            SetData(TYPE_GILNID, DONE);
            break;
    }
}

void instance_deadmines::RespawnPatrolMember(uint32 entry, uint32 guidCounter)
{
    if (Creature *patrolCreature = instance->GetCreature(ObjectGuid(HIGHGUID_UNIT, entry, guidCounter)))
        patrolCreature->Respawn();
}

void instance_deadmines::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_RHAHKZOR:
        {
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_FACTORY_DOOR);

                //Respawn patrol
                RespawnPatrolMember(NPC_OVERSEER, 79373);
                RespawnPatrolMember(NPC_EVOKER, 79374);
            }
            break;
        }
        case TYPE_SNEED:
        {
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_MAST_ROOM_DOOR);

                //Respawn patrol
                RespawnPatrolMember(NPC_OVERSEER, 79244);
                RespawnPatrolMember(NPC_EVOKER, 79177);
                RespawnPatrolMember(NPC_EVOKER, 79245);
            }
            break;
        }
        case TYPE_GILNID:
        {
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_FOUNDRY_DOOR);

                //Respawn patrol
                RespawnPatrolMember(NPC_TASKMASTER, 79283);
                RespawnPatrolMember(NPC_WIZARD, 79284);
                RespawnPatrolMember(NPC_TASKMASTER, 79285);
            }
            break;
        }
        case TYPE_DEFIAS_ENDDOOR:
        {
            if (uiData == IN_PROGRESS)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_IRON_CLAD_DOOR))
                {
                    pGo->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    m_uiIronDoorTimer = 1000;
                }
            }
            break;
        }
        case TYPE_POWDER:
        {
            if (uiData == DONE)
            {
                RespawnPatrolMember(NPC_DEFIAS_BLACKGUARD, 3000648);
                if (Creature *blackguard = instance->GetCreature(ObjectGuid(HIGHGUID_UNIT, NPC_DEFIAS_BLACKGUARD, uint32(3000648))))
                {
                    blackguard->SetInCombatWithZone();
                }
            }
            break;
        }
    }

    m_auiEncounter[uiType-1] = uiData;
}

uint32 instance_deadmines::GetData(uint32 uiType)
{
    switch(uiType)
    {
        case TYPE_DEFIAS_ENDDOOR:
            return m_auiEncounter[0];
        case TYPE_RHAHKZOR:
            return m_auiEncounter[1];
        case TYPE_SNEED:
            return m_auiEncounter[2];
        case TYPE_GILNID:
            return m_auiEncounter[3];
    }

    return 0;
}

void instance_deadmines::CreatureInvestigateDoor(uint32 entry, uint32 creatureGuidCounter, float xTarget, float yTarget, float zTarget)
{
    if (Creature *investigator = instance->GetCreature(ObjectGuid(HIGHGUID_UNIT,entry,creatureGuidCounter)))
    {
        investigator->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        investigator->GetMotionMaster()->Clear(true,true);
        investigator->GetMotionMaster()->MovePoint(0, xTarget, yTarget, zTarget);
    }
}

void instance_deadmines::Update(uint32 uiDiff)
{
    if (m_uiIronDoorTimer)
    {
        if (m_uiIronDoorTimer <= uiDiff)
        {
            switch(m_uiDoorStep)
            {
                case 0:
                    DoOrSimulateScriptTextForThisInstance(INST_SAY_ALARM1,NPC_MR_SMITE);
                    m_uiIronDoorTimer = 100;
                    ++m_uiDoorStep;
                    break;
                case 1:
                {
                    // Screw it- exactly 4 creatures should run over toward the door to see what happened.. their guid counters:
                    // - 79289
                    // - 79292
                    // - 79290
                    // - 79294
                    //The positions and guids are hard coded because at some point I'd like to get
                    //the cove set up with creature pooling 50/50 random spawns, which won't allow
                    //searching by entry- the positions are hard coded because their guard positions
                    //need to look good if the players back off the door after using the cannon.
                    CreatureInvestigateDoor(NPC_PIRATE, 79290, -100.502f, -668.771f, 7.4105f);
                    CreatureInvestigateDoor(NPC_SQUALLSHAPER, 79289, -95.721f, -672.609f, 7.411f);
                    CreatureInvestigateDoor(NPC_SQUALLSHAPER, 79292, -99.448f, -672.244f, 7.4238f);
                    CreatureInvestigateDoor(NPC_SQUALLSHAPER, 79294, -102.226f, -672.279f, 7.4153f);

                    ++m_uiDoorStep;
                    m_uiIronDoorTimer = 10000;
                    break;
                }
                case 2:
                    DoOrSimulateScriptTextForThisInstance(INST_SAY_ALARM2,NPC_MR_SMITE);
                    m_uiDoorStep = 0;
                    m_uiIronDoorTimer = 0;
                    debug_log("SD2: Instance Deadmines: Iron door event reached end.");
                    break;
            }
        }
        else
            m_uiIronDoorTimer -= uiDiff;
    }
}

InstanceData* GetInstanceData_instance_deadmines(Map* pMap)
{
    return new instance_deadmines(pMap);
}

bool GoUse_factory_lever(Player *,GameObject *gobj)
{
    ScriptedInstance *instance = dynamic_cast<ScriptedInstance*>(gobj->GetInstanceData());

    if (instance != NULL)
        instance->DoUseDoorOrButton(GO_FACTORY_DOOR);

    return true;
}

bool GoUse_mastroom_lever(Player *,GameObject *gobj)
{
    ScriptedInstance *instance = dynamic_cast<ScriptedInstance*>(gobj->GetInstanceData());

    if (instance != NULL)
        instance->DoUseDoorOrButton(GO_MAST_ROOM_DOOR);

    return true;
}

bool GoUse_foundry_lever(Player *,GameObject *gobj)
{
    ScriptedInstance *instance = dynamic_cast<ScriptedInstance*>(gobj->GetInstanceData());

    if (instance != NULL)
        instance->DoUseDoorOrButton(GO_FOUNDRY_DOOR);

    return true;
}

void AddSC_instance_deadmines()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_deadmines";
    pNewScript->GetInstanceData = &GetInstanceData_instance_deadmines;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mastroom_lever";
    pNewScript->pGOUse = &GoUse_mastroom_lever;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_factory_lever";
    pNewScript->pGOUse = &GoUse_factory_lever;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_foundry_lever";
    pNewScript->pGOUse = &GoUse_foundry_lever;
    pNewScript->RegisterSelf();
}