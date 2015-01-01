/*
 * Copyright (C) 2006-2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2010-2011 ScriptDev0 <http://github.com/mangos-zero/scriptdev0>
 *
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
SDName: Instance_Blackwing_Lair
SD%Complete: 0
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"
#include "TemporarySummon.h"

enum RazorgorePhase1
{
    MOB_DEATH_TALON_DRAGONSPAWN = 12422,
    MOB_BLACKWING_LEGIONNAIRE	= 12416,
    MOB_BLACKWING_MAGE			= 12420,

    SPELL_FIREBALL_VOLLEY		= 22425
};

enum {
    EMOTE_TROOPS_FLEE           = -1469033,                 // emote by Nefarian's Troops npc
    SAY_TROOPS_FLEE				= -1469032					// Message saying the troops are fleeing.
};
void instance_blackwing_lair::ClearRazorgoreSpawns()
{
    for (Creature* current_creature : m_vecSummonList)
        ((TemporarySummon*) current_creature)->UnSummon();

    m_vecSummonList.clear();
}



instance_blackwing_lair::instance_blackwing_lair(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
}

instance_blackwing_lair::~instance_blackwing_lair()
{
    if (m_pSharedGoblinThreatList)
        delete m_pSharedGoblinThreatList;
}

void instance_blackwing_lair::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    m_uiCurrentPhase = 0;
    m_uiSpawnTimer = 45000;
    m_uiDragonCount = 0;
    m_uiUnusedEggCounter = 30;
    m_uiRazorgoreRespawnTimer = 0;
    m_bHasRazorUsedAggro = false;

    // Reserve space for the summons and the used eggs to gain performance.
    m_vecSummonList.reserve(52);
    m_vecUsedEggList.reserve(30);

    m_pSharedGoblinThreatList = nullptr;
    
    m_uiLashlayerDoorTimer = 3000;
    m_bLashlayerDoorOpened = false;
}

void instance_blackwing_lair::SetRazorgorePhase(uint32 phase)
{
    m_uiCurrentPhase = phase;

    if (phase == 0)
    {
        m_uiDragonCount = 0;
        m_uiSpawnTimer = 45000;
        m_uiUnusedEggCounter = 30;
        m_bHasRazorUsedAggro = false;

        ClearRazorgoreSpawns();

        DoUseDoorOrButton(GO_DOOR_RAZORGORE_ENTER);

        for (GameObject* current_egg : m_vecEggList)
        {
            current_egg->SetGoState(GO_STATE_READY);
            current_egg->Respawn();
        }

        Creature* razorgore = GetSingleCreatureFromStorage(NPC_RAZORGORE);
        if (razorgore && razorgore->isDead())
            m_uiRazorgoreRespawnTimer = 3000;

        Creature* grethok = GetSingleCreatureFromStorage(NPC_GRETHOK);
        if (grethok && grethok->isDead())
            grethok->Respawn();

        for (Creature* current_guard : m_vecGrethokGuardList)
            if (current_guard && current_guard->isDead())
                current_guard->Respawn();

        m_vecUsedEggList.clear();

    }
}

void instance_blackwing_lair::SuspendOrResumeSuppressionRooms(bool suspend)
{
    for (std::pair<std::pair<uint32, uint32>, GameObject*> &current_pair : m_vecTrapList)
        current_pair.first.first = suspend ? 10 * DAY : 210;

    for (Creature* current_creature : m_vecWhelpList)
    {
        uint32 resume_time;

        if (current_creature->GetEntry() == NPC_DEATH_TALON_HATCHER || current_creature->GetEntry() == NPC_BLACKWING_TASKMASTER)
            resume_time = 600;
        else
            resume_time = urand(25, 240);

        current_creature->SetRespawnTime(suspend ? 10 * DAY : resume_time);
        current_creature->SetRespawnDelay(suspend ? 10 * DAY : resume_time);
    }

}


void instance_blackwing_lair::RemoveRazorgoreEgg(GameObject* egg)
{
    // Don't let Razorgore destroy the same egg twice.
    if (std::find(m_vecUsedEggList.begin(), m_vecUsedEggList.end(), egg->GetObjectGuid()) != m_vecUsedEggList.end())
        return;

    m_uiUnusedEggCounter--;
    m_vecUsedEggList.push_back(egg->GetObjectGuid());

    Creature* razorgore = GetSingleCreatureFromStorage(NPC_RAZORGORE);
    if (!razorgore)
        return;

    // When destroying the four last eggs Razorgore will use his fireball volly once to attract aggro.
    if (!m_bHasRazorUsedAggro && m_uiUnusedEggCounter < 4)
    {
        int roll = urand(0, 99);
        bool can_cast = false;

        switch(m_uiUnusedEggCounter)
        {
        case 3:
            if (roll < 20)
                can_cast = true;
            break;
        case 2:
            if (roll < 60)
                can_cast = true;
            break;
        case 1:
            can_cast = true;
            break;
        }

        if (can_cast)
        {
            razorgore->CastSpell(razorgore, SPELL_FIREBALL_VOLLEY, true);
            m_bHasRazorUsedAggro = true;
        }
    }

}


bool instance_blackwing_lair::IsEncounterInProgress() const
{
    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            return true;
    }
    return false;
}

void instance_blackwing_lair::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
    case NPC_BLACKWING_TECHNICIAN:
        // Sort creatures so we can get only the ones near Vaelastrasz
        if (pCreature->IsWithinDist2d(aNefariusSpawnLoc[0], aNefariusSpawnLoc[1], 50.0f))
            m_lTechnicianGuids.push_back(pCreature->GetObjectGuid());
        return;
    case NPC_GRETHOK_GUARD:
        m_vecGrethokGuardList.push_back(pCreature);
        return;
    case NPC_VAELASTRASZ:
        break;
    case NPC_RAZORGORE:
        break;
    case NPC_GRETHOK:
        break;
    case NPC_ORB_OF_DOMINATION_TRIGGER:
        break;
    case NPC_RED_WHELP:
    case NPC_GREEN_WHELP:
    case NPC_BLUE_WHELP:
    case NPC_BRONZE_WHELP:
    case NPC_DEATH_TALON_HATCHER:
    case NPC_BLACKWING_TASKMASTER:
        if (m_auiEncounter[TYPE_LASHLAYER] == DONE)
        {
            pCreature->ForcedDespawn(0);

            pCreature->SetRespawnTime(10 * DAY);
            pCreature->SaveRespawnTime();

            pCreature->SetRespawnDelay(10 * DAY);
        }
        else
        {
            uint32 respawn_time;

            if (pCreature->GetEntry() == NPC_DEATH_TALON_HATCHER ||
                    pCreature->GetEntry() == NPC_BLACKWING_TASKMASTER)
                respawn_time = 600;
            else
                respawn_time = urand(25, 240);

            pCreature->SetRespawnDelay(respawn_time);

            pCreature->SetRespawnTime(0);
            pCreature->SaveRespawnTime();
        }

        m_vecWhelpList.push_back(pCreature);
        return;
    case NPC_TECHNICIAN_THREAT_HOLDER:
        m_pSharedGoblinThreatList = new ThreatManager(pCreature);						// A custom shared threat list for all goblins after Lashlayer.
        break;
    default:
        return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_blackwing_lair::OnObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
    case GO_DOOR_RAZORGORE_ENTER:
        break;
    case GO_DOOR_RAZORGORE_EXIT:
        if (m_auiEncounter[TYPE_RAZORGORE] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_DOOR_NEFARIAN:
    case GO_DOOR_CHROMAGGUS_ENTER:
    case GO_DOOR_CHROMAGGUS_SIDE:
        break;
    case GO_DOOR_CHROMAGGUS_EXIT:
        if (m_auiEncounter[TYPE_CHROMAGGUS] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_DOOR_VAELASTRASZ:
        if (m_auiEncounter[TYPE_VAELASTRASZ] == DONE)
            pGo->SetGoState(GO_STATE_ACTIVE);
        break;
    case GO_DOOR_LASHLAYER:
        if (m_auiEncounter[TYPE_LASHLAYER] == DONE)
		{
            pGo->SetGoState(GO_STATE_ACTIVE);
			m_bLashlayerDoorOpened = true;
		}
        break;
    case GO_BLACK_DRAGON_EGG:
        pGo->SetGoState(GO_STATE_READY);
        m_vecEggList.push_back(pGo);
        return;
    case GO_SUPPRESSION_DEVICE:
    {
        uint32 spawn_time;

        if (m_auiEncounter[TYPE_LASHLAYER] == DONE)
        {
            pGo->DisarmTrap();
            spawn_time = 10 * DAY * IN_MILLISECONDS;
        }
        else
            spawn_time = 210 * IN_MILLISECONDS;

        m_vecTrapList.push_back(std::make_pair(std::make_pair(spawn_time, urand(0, 10000)), pGo));
        return;
    }
    default:
        return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_blackwing_lair::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
    case TYPE_RAZORGORE:
        m_auiEncounter[uiType] = uiData;
        DoUseDoorOrButton(GO_DOOR_RAZORGORE_ENTER);
        if (uiData == DONE)
            DoUseDoorOrButton(GO_DOOR_RAZORGORE_EXIT);
        break;
    case TYPE_VAELASTRASZ:
        m_auiEncounter[uiType] = uiData;
        // Prevent the players from running back to the first room; use if the encounter is not special
        if (uiData != SPECIAL)
            DoUseDoorOrButton(GO_DOOR_RAZORGORE_EXIT);
        if (uiData == DONE)
            DoUseDoorOrButton(GO_DOOR_VAELASTRASZ);
        break;
    case TYPE_LASHLAYER:
        m_auiEncounter[uiType] = uiData;
        break;
    case TYPE_FIREMAW:
    case TYPE_EBONROC:
    case TYPE_FLAMEGOR:
        m_auiEncounter[uiType] = uiData;
        break;
    case TYPE_CHROMAGGUS:
        m_auiEncounter[uiType] = uiData;
        if (uiData == DONE)
            DoUseDoorOrButton(GO_DOOR_CHROMAGGUS_EXIT);
        break;
    case TYPE_NEFARIAN:
        m_auiEncounter[uiType] = uiData;
        DoUseDoorOrButton(GO_DOOR_NEFARIAN);
        break;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3]
                   << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " " << m_auiEncounter[6] << " " << m_auiEncounter[7];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_blackwing_lair::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4]
               >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

uint32 instance_blackwing_lair::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_blackwing_lair::Update(uint32 uiDiff)
{
  
    // Delay the opening of Lashlayer's door.
    if (GetData(TYPE_LASHLAYER) == DONE && !m_bLashlayerDoorOpened)
    {
		if (m_uiLashlayerDoorTimer <= uiDiff)
		{
			DoUseDoorOrButton(GO_DOOR_LASHLAYER);
			m_bLashlayerDoorOpened = true;
		}
		else
		  m_uiLashlayerDoorTimer -= uiDiff;
    }
  
  
    /***********************************************
    ** Handling for the Razorgore fight has to be
    ** done here since the MC disables his AI.
    ***********************************************/

    if (m_uiRazorgoreRespawnTimer)					// Respawn Razorgore if he fails and kills everyone.
    {
        if (m_uiRazorgoreRespawnTimer <= uiDiff)
        {
            Creature* razorgore = GetSingleCreatureFromStorage(NPC_RAZORGORE);
            if (razorgore)
                razorgore->Respawn();
            m_uiRazorgoreRespawnTimer = 0;
        }
        else
            m_uiRazorgoreRespawnTimer -= uiDiff;

    }


    if (m_auiEncounter[TYPE_RAZORGORE] == IN_PROGRESS)
    {
        float spawn_points[8][3] = {{ -7659.4536f, -1043.3068f, 407.1996f }, { -7643.3994f, -1065.4550f, 407.2058f }, { -7623.2504f, -1094.2072f, 407.2057f }, { -7607.6133f, -1116.1356f, 407.1994f },
            { -7533.2510f, -1063.8618f, 407.1996f }, { -7548.0371f, -1041.3660f, 407.2053f }, { -7568.1523f, -1012.1231f, 407.2048f }, { -7583.7607f, -990.5577f, 407.1994f }
        };

        Creature* razorgore = GetSingleCreatureFromStorage(NPC_RAZORGORE);

        if (!razorgore)
            return;

        switch(GetRazorgorePhase())
        {
        case 1:
        {
            if (razorgore->HasAuraType(AuraType::SPELL_AURA_MOD_DECREASE_SPEED))	// Remove the MC spell's slow effect.
                razorgore->RemoveAura(19832, SpellEffectIndex::EFFECT_INDEX_2);

            if (m_uiUnusedEggCounter == 0)
            {
                for (Creature* current_add :  m_vecSummonList)
                {
                    DoScriptText(EMOTE_TROOPS_FLEE, current_add);
                    current_add->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    current_add->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    current_add->getThreatManager().clearReferences();
                    current_add->CombatStop();
                    current_add->GetMotionMaster()->Clear();
                    current_add->RemoveSplineFlag(SplineFlags::SPLINEFLAG_WALKMODE);
                    current_add->GetMotionMaster()->MovePoint(2, -7553.8491f, -1024.1167f, 408.4908f);
                }

                m_vecSummonList.clear();

                razorgore->RemoveAllAuras();

                razorgore->SetHealthPercent(100.f);

                razorgore->GenericTextEmote("Nefarian's Troops flee as the controlling power of the orb is drained.", NULL, true);

                SetRazorgorePhase(2);
                break;
            }


            if (m_uiSpawnTimer <= uiDiff)
            {
                if (!m_vecSummonList.empty()) // Clear out dead adds.
                {
                    std::vector<Creature*>::iterator itr = m_vecSummonList.begin();
                    while (itr != m_vecSummonList.end())
                    {
                        TemporarySummon* spawned_creature = (TemporarySummon*)(*itr);

                        if (spawned_creature->isDead())
                        {
                            if (spawned_creature->GetEntry() == MOB_DEATH_TALON_DRAGONSPAWN)
                                m_uiDragonCount -= m_uiDragonCount > 0 ? 1 : 0;

                            spawned_creature->UnSummon();
                            itr = m_vecSummonList.erase(itr);
                        }
                        else
                            ++itr;
                    }
                }

                if (m_vecSummonList.size() < 52)
                    for (int i = 0; i < 8; i++)
                    {
                        uint32 creature_id = 0;
                        uint32 spawn_roll = urand(0, 99);

                        if (spawn_roll < 10) // 10 % to spawn dragons.
                        {
                            if (m_uiDragonCount < 12)  // Spawn a maximum of 12 dragons.
                            {
                                creature_id = MOB_DEATH_TALON_DRAGONSPAWN;
                                m_uiDragonCount++;
                            }
                        }
                        else if (m_vecSummonList.size() - m_uiDragonCount < 40) // Spawn a maximum of 40 orcs.
                        {
                            if (spawn_roll < 35)  // 25 % for a Legionnaire.
                                creature_id = MOB_BLACKWING_LEGIONNAIRE;
                            else if(spawn_roll < 60)  // 25 % for a mage.
                                creature_id = MOB_BLACKWING_MAGE;
                        }

                        if (creature_id)  // If we have a spawn id then spawn. There's a 40 % chance not to spawn any mob.
                        {
                            Creature* summoned_creature = razorgore->SummonCreature(creature_id, spawn_points[i][0], spawn_points[i][1], spawn_points[i][2], 0, TempSummonType::TEMPSUMMON_MANUAL_DESPAWN, 0);

                            if (!summoned_creature)
                                return;

                            summoned_creature->setFaction(40);
                            //summoned_creature->SetSpeedRate(MOVE_WALK, 2.f, true);
                            summoned_creature->SetSplineFlags(SPLINEFLAG_NONE);

                            if (summoned_creature->GetEntry() == MOB_BLACKWING_MAGE)
                            {
                                summoned_creature->SetInCombatWithZone();
                                summoned_creature->Attack(razorgore->GetCharmerOrOwner(), false);
                            }
                            else
                            {
                                summoned_creature->GetMotionMaster()->MovePoint(1, -7620.7876f, -1016.8992f, 413.3818f);
                            }

                            m_vecSummonList.push_back(summoned_creature);
                        }
                    }

                m_uiSpawnTimer = 15000;
            }
            else
                m_uiSpawnTimer -= uiDiff;
        }
        }
    }

    // Handle random animation of the traps in the suppresion room.rnd
    for (std::pair<std::pair<uint32, uint32>, GameObject*> &current_pair : m_vecTrapList)
    {
        if (current_pair.second->GetGoState() == GO_STATE_ACTIVE)
        {
            if (m_auiEncounter[TYPE_LASHLAYER] != DONE)
            {
                // current_pair.first.first is the trap CD timer.
                // current_pair.first.second is the animation CD timer.

                // Manually handle the respawning of the traps in the suppression rooms.
                if (current_pair.first.first <= uiDiff)
                {
                    current_pair.second->SetGoState(GO_STATE_READY);

                    current_pair.first.first = 210 * IN_MILLISECONDS;
                    current_pair.second->SetCooldownTime(0);
                }
                else
                    current_pair.first.first -= uiDiff;
            }
        }
        else
        {
            // Handle the trap animations.
            if (current_pair.first.second <= uiDiff)
            {
                current_pair.second->SendGameObjectCustomAnim(current_pair.second->GetGUID());

                current_pair.first.second = 10000;
            }
            else
                current_pair.first.second -= uiDiff;
        }

    }
}


InstanceData* GetInstanceData_instance_blackwing_lair(Map* pMap)
{
    return new instance_blackwing_lair(pMap);
}

void AddSC_instance_blackwing_lair()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_blackwing_lair";
    pNewScript->GetInstanceData = &GetInstanceData_instance_blackwing_lair;
    pNewScript->RegisterSelf();
}
