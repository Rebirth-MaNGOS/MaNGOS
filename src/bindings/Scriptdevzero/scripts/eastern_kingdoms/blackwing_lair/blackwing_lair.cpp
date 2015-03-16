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
SDName: Blackwing_Lair
SD%Complete: 10
SDComment:
SDCategory: Blackwing Lair
EndScriptData */

/* ContentData
mob_demon_portal
EndContentData */

#include "precompiled.h"
#include "blackwing_lair.h"
#include "TemporarySummon.h"
#include "MovementGenerator.h"
#include "Unit.h"

#include <unordered_map>

/*######
## mob_demon_portal
######*/

enum eDemonPortal
{
    NPC_DEMONIC_MINION              = 14101,        // The Enraged Felguard summoned by the Warlocks.
    SPELL_SUMMON_DEMONIC_MINION     = 19828,        // probably wrong spell entry
};

struct MANGOS_DLL_DECL mob_demon_portalAI : public ScriptedAI
{
    mob_demon_portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->addUnitState(UNIT_STAT_ROOT);
        m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

        m_uiDespawnSelfTimer = 31000;
		m_uiSpawnDemonTimer = 30000;

		m_creature->SetActiveObjectState(true);
	
        Reset();
    }

    uint32 m_uiSpawnDemonTimer;
    uint32 m_uiDespawnSelfTimer;

    void Reset()
    {
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pOwner = m_creature->GetMap()->GetUnit(m_creature->GetOwnerGuid()))
        {
            pSummoned->SetOwnerGuid(pOwner->GetObjectGuid());
            pSummoned->SetCharmerGuid(pOwner->GetObjectGuid());
            pSummoned->setFaction(pOwner->getFaction());
            pSummoned->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pSummoned->GetMotionMaster()->MoveFollow(pOwner, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }
        else
            pSummoned->setFaction(m_creature->getFaction());

		pSummoned->SetRespawnDelay(10 * DAY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Close portal (despawn) after 35 seconds
        if (m_uiDespawnSelfTimer <= uiDiff)
        {
            m_uiDespawnSelfTimer = 0;
            m_creature->ForcedDespawn();
			return;
        }
		else
		  m_uiDespawnSelfTimer -= uiDiff;
	
		if (!m_creature->GetOwner())
		{
			m_creature->ForcedDespawn();
			return;
		}
		else if (!m_creature->GetOwner()->isInCombat())
		{
			m_creature->ForcedDespawn();
			return;
		}
	
        // Demon Spawn
        if (m_uiSpawnDemonTimer <= uiDiff)
        {
            /*if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_DEMONIC_MINION) == CAST_OK)
                m_uiSpawnDemonTimer = 11000;*/
            Position pos;
            m_creature->GetClosePoint(pos.x, pos.y, pos.z, 0);
            m_creature->SummonCreature(NPC_DEMONIC_MINION, pos.x, pos.y, pos.z, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
	    
			m_uiSpawnDemonTimer = 10000000;
        }
        else
            m_uiSpawnDemonTimer -= uiDiff;

        // no meele, it is a portal..
    }
};

CreatureAI* GetAI_mob_demon_portal(Creature* pCreature)
{
    return new mob_demon_portalAI(pCreature);
}


/*###########################
## npc_death_talon_trigger ##
###########################*/

enum DeathTalonTrigger
{
    NPC_DEATH_TALON_CAPTAIN		= 12467,
    NPC_DEATH_TALON_SEETHER		= 12464,
    NPC_DEATH_TALON_WYRMKIN		= 12465,
    NPC_DEATH_TALON_FLAMESCALE	= 12463
};

struct MANGOS_DLL_DECL npc_death_talon_triggerAI : public ScriptedAI
{
    npc_death_talon_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetActiveObjectState(true);

        SetCombatMovement(false);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        m_pInstance = dynamic_cast<instance_blackwing_lair*>(m_creature->GetInstanceData());

        m_isActive = true;
        m_prevCombatFlag = false;

        m_uiDisableTimer = 900000;

        m_packSpawned = false;
    }

    instance_blackwing_lair* m_pInstance;

    std::vector<Creature*> m_vecDragonList;
    std::unordered_map<uint32, uint32> m_sleepTimerMap;

    bool m_isActive, m_prevCombatFlag, m_packSpawned;
    uint32 m_uiDisableTimer;

    uint32 GetSleepBreakTimer()
    {
        uint32 group_roll = urand(0, 100), time_roll;
        if (group_roll < 70)
            time_roll = urand(10000, 15000);
        else if (group_roll < 85)
            time_roll = urand(5000, 10000);
        else
            time_roll = urand(15000, 20000);

        return time_roll;
    }

    void Reset()
    {
    }

    void DisableDragons()
    {
        if (!m_vecDragonList.empty())
        {
            for (Creature* current_creature : m_vecDragonList)
            {
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                current_creature->Respawn();

                current_creature->SetVisibility(UnitVisibility::VISIBILITY_OFF);

                current_creature->SetRespawnTime(10 * DAY);
                current_creature->SaveRespawnTime();
                current_creature->SetRespawnDelay(10 * DAY);
            }

            m_vecDragonList.clear();
        }
    }

    void SpawnDragons()
    {
        uint32 dragons[] = { NPC_DEATH_TALON_CAPTAIN, NPC_DEATH_TALON_WYRMKIN, NPC_DEATH_TALON_SEETHER, NPC_DEATH_TALON_FLAMESCALE };
        std::vector<Creature*> pick_list;

        // Get all dragons.
        for (uint8 i = 0; i < 4; i++)
        {
            std::list<Creature*> tmp_dragon_list;
            GetCreatureListWithEntryInGrid(tmp_dragon_list, m_creature, dragons[i], 10.f);

            for (Creature* current_creature : tmp_dragon_list)
            {
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                current_creature->SetVisibility(UnitVisibility::VISIBILITY_OFF);
                pick_list.push_back(current_creature);
            }
        }

        // If we don't find all dragons we assume that the packs have been killed and the dragons haven't respawned.
        if (pick_list.size() < 7)
        {
            m_vecDragonList.insert(m_vecDragonList.end(), pick_list.begin(), pick_list.end());

            DisableDragons();
            m_isActive = false;

            return;
        }


        // Start by selecting one of each dragon.
        // Search through the list of dragons to pick from for each dragon type and move it to the list of chosen dragons.
        for (uint8 i = 0; i < 4; i++)
        {
            auto dragon_itr = std::find_if(pick_list.begin(), pick_list.end(), creature_compare(dragons[i]));

            if (dragon_itr != pick_list.end())
            {
                m_vecDragonList.push_back(*dragon_itr);

                // Set inital timers for breaking sleep.
                if ((*dragon_itr)->GetEntry() == DeathTalonTrigger::NPC_DEATH_TALON_WYRMKIN)
                    m_sleepTimerMap[(*dragon_itr)->GetGUIDLow()] = GetSleepBreakTimer();

                pick_list.erase(dragon_itr);
            }
        }

        if (pick_list.size() > 2)
        {
            // Then select two random dragons.
            for (uint8 i = 0; i < 2; i++)
            {
                uint32 rnd = urand(0, pick_list.size() - 1);

                m_vecDragonList.push_back(pick_list[rnd]);
                pick_list.erase(pick_list.begin() + rnd);
            }

            // Shuffle the dragons.
            std::random_shuffle(m_vecDragonList.begin(), m_vecDragonList.end());

            PlaceDragons();
        }

        // Disable the not chosen dragons.
        for (Creature* current_dragon : pick_list)
        {
            current_dragon->SetRespawnDelay(10 * DAY);
            current_dragon->SetRespawnTime(10 * DAY);
            current_dragon->SaveRespawnTime();
        }

    }

    void PlaceDragons()
    {
        // Place the dragons in a circle around the trigger.
        for (uint8 i = 0; i < m_vecDragonList.size(); i++)
        {

            m_vecDragonList[i]->RelocateCreature(m_creature->GetPositionX() + 10.f * cosf((float)(PI) * 2 / m_vecDragonList.size() * i), m_creature->GetPositionY() +
                                                 10.f * sinf((float)(PI) * 2 / m_vecDragonList.size() * i), m_creature->GetPositionZ(), 0);
            m_vecDragonList[i]->SetFacingToObject(m_creature);

            m_vecDragonList[i]->GetMotionMaster()->Clear(false, true);
            m_vecDragonList[i]->GetMotionMaster()->MoveIdle();

            m_vecDragonList[i]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            m_vecDragonList[i]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_vecDragonList[i]->SetVisibility(UnitVisibility::VISIBILITY_ON);
        }
    }


    void UpdateAI(const uint32 uiDiff)
    {

		if(!m_packSpawned)
		{
			SpawnDragons();
			m_packSpawned = true;
		}

        if (m_isActive)
        {
            Unit* target = nullptr;

            // Flags to keep track of the general status of the pack since we have no custom AIs with callbacks.
            bool combat_flag = false, evade_flag = false;
            bool death_flag = false, alive_flag = false;
            for (Creature* current_creature : m_vecDragonList)
            {
                if (current_creature->isAlive())
                {
                    alive_flag = true;

                    if (current_creature->isInCombat())
                    {
                        combat_flag = true;
                        target = current_creature->getVictim();
                    }

                    if (current_creature->IsInEvadeMode())
                        evade_flag = true;

                    if (current_creature->GetEntry() == DeathTalonTrigger::NPC_DEATH_TALON_WYRMKIN)
                    {
                        if (current_creature->HasAura(2637) || current_creature->HasAura(18657) ||
                                current_creature->HasAura(18658))
                        {
                            if (m_sleepTimerMap[current_creature->GetGUIDLow()] <= uiDiff)
                            {
                                current_creature->RemoveAurasDueToSpell(2637);
                                current_creature->RemoveAurasDueToSpell(18657);
                                current_creature->RemoveAurasDueToSpell(18658);

                                m_sleepTimerMap[current_creature->GetGUIDLow()] = GetSleepBreakTimer();
                            }
                            else
                                m_sleepTimerMap[current_creature->GetGUIDLow()] -= uiDiff;
                        }

                    }

                }
                else
                    death_flag = true;
            }

            // If all dragons are out of combat and any dragon happens to be dead the entire pack should respawn.
            if (!combat_flag)
            {
                if (death_flag && alive_flag)
                {
                    for (Creature* current_creature : m_vecDragonList)
                    {
                        if (!current_creature->isAlive())
                            current_creature->Respawn();
                    }
                }

                // Make sure the trigger exits combat if the dragons aren't in combat.
                if (m_creature->isInCombat())
                    m_creature->CombatStop(true);
            }
            else if (target) // If any dragon is in combat and we have a target all dragons should enter combat.
            {
                for (Creature* current_creature : m_vecDragonList)
                {
                    if (current_creature->isAlive() && !current_creature->isInCombat())
						current_creature->Attack(target, true);
                }	
            }

            // If no dragons are alive the pack should stop respawning.
            if (!alive_flag)
            {
                if (m_uiDisableTimer <= uiDiff)
                {
                    DisableDragons();
                    m_isActive = false;
                }
                else
                    m_uiDisableTimer -= uiDiff;
            }

            // If creatrues are still evading we prolong the reset to avoid dragons standing in the wrong places.
            if (!evade_flag)
            {
                // If we weren't in combat this update, but we were in combat the previous update it is time
                // to reset the dragons' positions if there are dragons still alive.
                if (!combat_flag && m_prevCombatFlag && alive_flag)
                    PlaceDragons();

                //Save the current combat flag to determine next update if we've exited combat.
                m_prevCombatFlag = combat_flag;
            }



        }
    }
};

CreatureAI* GetAI_npc_death_talon_trigger(Creature* pCreature)
{
    return new npc_death_talon_triggerAI(pCreature);
}

/*##########################
## npc_technician_trigger ##
##########################*/

enum TechnicianPack
{
    NPC_BLACKWING_WARLOCK	= 12459,
    NPC_BLACKWING_SPELLBINDER	= 12457,
    NPC_DEATH_TALON_OVERSEER	= 12461,
	NPC_DEATH_TALON_WYRMGUARD = 12460,
};

struct DragonPoint
{
    float x;
    float y;
    float z;

	 DragonPoint(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct MANGOS_DLL_DECL npc_technician_pack_triggerAI : public ScriptedAI
{
    npc_technician_pack_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetActiveObjectState(true);

        SetCombatMovement(false);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

        m_pInstance = dynamic_cast<instance_blackwing_lair*>(m_creature->GetInstanceData());

        m_isActive = true;
        m_prevCombatFlag = false;

        m_uiDisableTimer = 900000;
        m_uiDelayedPickUp = 1000;
		m_packSpawned = false;
    }

    instance_blackwing_lair* m_pInstance;

    std::vector<Creature*> m_vecCreatureList;
	std::vector<DragonPoint> m_vecPointlist;

    bool m_isActive, m_prevCombatFlag, m_packSpawned;
    uint32 m_uiDisableTimer;
    uint32 m_uiDelayedPickUp;


    void Reset()
    {
    }

    void DisablePack()
    {
        if (!m_vecCreatureList.empty())
        {
            for (Creature* current_creature : m_vecCreatureList)
            {
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                current_creature->Respawn();

                current_creature->SetVisibility(UnitVisibility::VISIBILITY_OFF);

                current_creature->SetRespawnTime(10 * DAY);
                current_creature->SaveRespawnTime();
                current_creature->SetRespawnDelay(10 * DAY);
            }

            m_vecCreatureList.clear();
        }
    }

    void SpawnPack()
    {
        uint32 creatures[] = { NPC_BLACKWING_WARLOCK, NPC_BLACKWING_TECHNICIAN, NPC_BLACKWING_SPELLBINDER, NPC_DEATH_TALON_OVERSEER, NPC_DEATH_TALON_WYRMGUARD};

        // Get all creatures.
        for (uint8 i = 0; i < 5; i++)
        {
            std::list<Creature*> tmp_creature_list;
            GetCreatureListWithEntryInGrid(tmp_creature_list, m_creature, creatures[i], 30.f);

            for (Creature* current_creature : tmp_creature_list)
            {
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                current_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                current_creature->SetVisibility(UnitVisibility::VISIBILITY_OFF);
                m_vecCreatureList.push_back(current_creature);
            }
        }

        // If we don't find at least one of each creature we assume the pack is disabled.
        if (m_vecCreatureList.size() < 3)
        {
            DisablePack();
            m_isActive = false;

            return;
        }

		 // The group straight ahead when entering the room.
        if (m_vecCreatureList.size() == 4)
		{
			for (uint8 i = 0; i < 4; i++)
			{
				m_vecPointlist.push_back(DragonPoint(m_vecCreatureList[i]->GetPositionX(), m_vecCreatureList[i]->GetPositionY(), m_vecCreatureList[i]->GetPositionZ()));
			}
          
			std::random_shuffle(m_vecPointlist.begin(), m_vecPointlist.end());
     
		}

        PlaceCreatures();

    }

	void PlaceCreatures()
    {
		 if (m_vecCreatureList.size() > 4 || m_vecCreatureList.size() < 4)
		 {
			 // Place the creatures.
			 for (Creature* current_creature : m_vecCreatureList)
			 {
				  current_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
				  current_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				  current_creature->SetVisibility(UnitVisibility::VISIBILITY_ON);
			 }
		 }
		 else if (m_vecCreatureList.size() == 4)
		 {
			 for (uint8 i = 0; i < 4; i++)
			 {
				  m_vecCreatureList[i]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);	
				  m_vecCreatureList[i]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				  m_vecCreatureList[i]->SetVisibility(UnitVisibility::VISIBILITY_ON);
				  m_vecCreatureList[i]->RelocateCreature(m_vecPointlist[i].x, m_vecPointlist[i].y, m_vecPointlist[i].z, 3.656f);
				  m_vecCreatureList[i]->GetMotionMaster()->Clear(false, true);
				  m_vecCreatureList[i]->GetMotionMaster()->MoveIdle();
			 }
		 }

    }


    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_packSpawned)
		{
			if (m_uiDelayedPickUp <= uiDiff)
            {
                SpawnPack();
			    m_packSpawned = true;
            }
            else 
                m_uiDelayedPickUp -= uiDiff;
		}

        if (m_isActive)
        {
            Unit* target = nullptr;

            // Flags to keep track of the general status of the pack since we have no custom AIs with callbacks.
            bool combat_flag = false, evade_flag = false;
            bool death_flag = false, alive_flag = false;
            for (Creature* current_creature : m_vecCreatureList)
            {
                if (current_creature->isAlive())
                {
                    alive_flag = true;

                    if (current_creature->isInCombat())
                    {
                        combat_flag = true;
                        target = current_creature->getVictim();
                    }

                    if (current_creature->IsInEvadeMode())
                        evade_flag = true;

                }
                else
                    death_flag = true;
            }

            // If all dragons are out of combat and any dragon happens to be dead the entire pack should respawn.
            if (!combat_flag)
            {
                if (death_flag && alive_flag)
                {
                    for (Creature* current_creature : m_vecCreatureList)
                    {
                        if (!current_creature->isAlive())
                            current_creature->Respawn();
                    }
                }

                // Make sure the trigger exits combat if the dragons aren't in combat.
                if (m_creature->isInCombat())
                    m_creature->CombatStop(true);
            }
            else if (target) // If any dragon is in combat and we have a target all dragons should enter combat.
            {
                for (Creature* current_creature : m_vecCreatureList)
                {
                    if (current_creature->isAlive() && !current_creature->isInCombat())
					{
                        current_creature->Attack(target, true);
					}
                }
            }

            // If no dragons are alive the pack should stop respawning.
            if (!alive_flag)
            {
                if (m_uiDisableTimer <= uiDiff)
                {
                    DisablePack();
                    m_isActive = false;
                }
                else
                    m_uiDisableTimer -= uiDiff;

				if (m_pInstance && !combat_flag && m_prevCombatFlag)
                {
                    Unit* threat_holder = m_pInstance->GetSingleCreatureFromStorage(NPC_TECHNICIAN_THREAT_HOLDER);

                    m_pInstance->m_pSharedGoblinThreatList->clearReferences();

                    if (threat_holder)
                    {
                        threat_holder->CombatStop(true);
                    }

					m_creature->CombatStop(true);
                }
            }

            // If creatrues are still evading we prolong the reset to avoid dragons standing in the wrong places.
            if (!evade_flag)
            {
                // If we weren't in combat this update, but we were in combat the previous update it is time
                // to reset the dragons' positions if there are dragons still alive.
                if (!combat_flag && m_prevCombatFlag && alive_flag)
                {
                    PlaceCreatures();

                    if (m_pInstance)
                    {
                        Unit* threat_holder = m_pInstance->GetSingleCreatureFromStorage(NPC_TECHNICIAN_THREAT_HOLDER);

                        m_pInstance->m_pSharedGoblinThreatList->clearReferences();

                        if (threat_holder)
                        {
                            threat_holder->CombatStop(true);
                        }

						m_creature->CombatStop(true);
                    }
                }

                //Save the current combat flag to determine next update if we've exited combat.
                m_prevCombatFlag = combat_flag;
            }
        }
    }
};

CreatureAI* GetAI_npc_technician_pack_trigger(Creature* pCreature)
{
    return new npc_technician_pack_triggerAI(pCreature);
}

/*################################
## npc_technician_threat_holder ##
################################*/

struct MANGOS_DLL_DECL npc_technician_threat_holderAI : public ScriptedAI
{
    npc_technician_threat_holderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_creature->SetActiveObjectState(true);

		m_pInstance = dynamic_cast<instance_blackwing_lair*>(m_creature->GetInstanceData());

        SetCombatMovement(false);
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
    }

	instance_blackwing_lair* m_pInstance;

    void Reset() {}

    void UpdateAI(const uint32 /*uiDiff*/) 
	{
		if (m_pInstance && m_pInstance->m_pSharedGoblinThreatList)
		{
			if (!m_pInstance->m_pSharedGoblinThreatList->isThreatListEmpty())
			{
				Unit* topThreat = m_pInstance->m_pSharedGoblinThreatList->getThreatList().front()->getTarget();
				if (topThreat && topThreat->IsInMap(m_creature))
				{
					m_creature->RelocateCreature(topThreat->GetPositionX(), topThreat->GetPositionY(), topThreat->GetPositionZ(), 0.f);
				}

			}
		}

	}
};

CreatureAI* GetAI_npc_technician_threat_holder(Creature* pCreature)
{
    return new npc_technician_threat_holderAI(pCreature);
}


/*############################
## mob_blackwing_technician ##
############################*/
enum BlackwingTechnician
{
    SPELL_THROW_DYNAMITE	= 22334,
    SPELL_THROW_POISON		= 22335,
	CAST_ANIMATION = 2657,
};


struct MANGOS_DLL_DECL mob_blackwing_technicianAI : public ScriptedAI
{

    instance_blackwing_lair* pInstance;

    uint32 m_uiPoisonTimer;
    uint32 m_uiDynamiteTimer;
	uint32 m_uiSpellAnimationTimer;

    bool m_bHasDied;
    bool m_bIsSummoned;
	bool m_bHasInterrupted;

    // Keep track of the previous threat value for synchronisation.
    std::unordered_map<uint64, float> m_PrevThreatValues;

    mob_blackwing_technicianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (instance_blackwing_lair*) m_creature->GetInstanceData();

        m_bHasDied = false;

        m_bIsSummoned = dynamic_cast<TemporarySummon*>(m_creature) ? true : false;

        Reset();
    }

    void Reset()
    {
        m_uiPoisonTimer = urand(7000, 11000);
        m_uiDynamiteTimer = urand(2300, 3900);
		m_uiSpellAnimationTimer = 1400;
		m_bHasInterrupted = false;
        // Clear the map of previous threat values on reset.
        m_PrevThreatValues.clear();
    }

    void Aggro(Unit* pUnit)
    {
        if (m_bIsSummoned)
        {
            if (pInstance)
            {
                std::vector<Creature*>* goblin_group1 = pInstance->GetGoblinGroup1();
                std::vector<Creature*>* goblin_group2 = pInstance->GetGoblinGroup2();

                if (std::find(goblin_group1->begin(), goblin_group1->end(), m_creature) != goblin_group1->end())
                {
                    for (Creature* current_goblin : *goblin_group1)
                        GoblinAttack(current_goblin, pUnit);
                }
                else
                {
                    for (Creature* current_goblin : *goblin_group2)
                        GoblinAttack(current_goblin, pUnit);
                }


            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        m_bHasDied = true;
    }

    void JustRespawned()
    {
        if (m_bIsSummoned && m_bHasDied)
        {
            if (pInstance)
            {
                std::vector<Creature*>* goblin_group1 = pInstance->GetGoblinGroup1();
                std::vector<Creature*>* goblin_group2 = pInstance->GetGoblinGroup2();

                auto result1 = std::find(goblin_group1->begin(), goblin_group1->end(), m_creature);
                if (result1 != goblin_group1->end())
                    goblin_group1->erase(result1);
                else
                {
                    auto result2 = std::find(goblin_group2->begin(), goblin_group2->end(), m_creature);
                    if (result2 != goblin_group2->end())
                        goblin_group2->erase(result2);
                }
            }

            TemporarySummon* temporary_goblin = dynamic_cast<TemporarySummon*>(m_creature);
            if (temporary_goblin)
                temporary_goblin->UnSummon();
        }
    }



    void GoblinAttack(Creature* current_goblin, Unit* target)
    {
        current_goblin->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        current_goblin->GetMotionMaster()->MoveChase(target);
        current_goblin->Attack(target, false);
    }



    void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
    {
        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

        switch(uiPointId)
        {
        case 0:
            m_creature->GetMotionMaster()->MovePoint(2, -7498.f, -1055.f, 424.f, false);
            break;
        case 1:
            m_creature->GetMotionMaster()->MovePoint(2, -7441.f, -1015.f, 424.f, false);
            break;
        case 2:
            m_creature->GetMotionMaster()->MovePoint(3, -7530.f, -959.f, 428.f);
            break;
        case 3:
        {
            if (pInstance)
            {
                std::vector<Creature*>* goblin_group1 = pInstance->GetGoblinGroup1();
                std::vector<Creature*>* goblin_group2 = pInstance->GetGoblinGroup2();

                auto result1 = std::find(goblin_group1->begin(), goblin_group1->end(), m_creature);
                if (result1 != goblin_group1->end())
                    goblin_group1->erase(result1);
                else
                {
                    auto result2 = std::find(goblin_group2->begin(), goblin_group2->end(), m_creature);
                    if (result2 != goblin_group2->end())
                        goblin_group2->erase(result2);
                }
            }

            TemporarySummon* temporary_goblin = dynamic_cast<TemporarySummon*>(m_creature);
            if (temporary_goblin)
                temporary_goblin->UnSummon();
        }
        break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

		if(!m_bHasInterrupted)
		{
			if(m_uiSpellAnimationTimer <= uiDiff)
			{
				m_creature->InterruptNonMeleeSpells(true);

				if(!m_creature->isInCombat())
					DoCast(m_creature, CAST_ANIMATION);
				else
					m_bHasInterrupted = true;

				m_uiSpellAnimationTimer = 1400;
			} 
			else
			{
				m_uiSpellAnimationTimer -= uiDiff;
			}
		}

        // If the goblin is neither summoned nor in combat we use normal non-shared targeting.
        if (m_bIsSummoned || !m_creature->isInCombat())
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;
        }
        else // Goblins that are part of a pack should have shared threat lists.
        {
            SynchroniseThreatLists();

            if (!pInstance || !SelectHostileTargetWithCustomThreatList(pInstance->m_pSharedGoblinThreatList) || !m_creature->getVictim())
                return;
        }

        if (m_uiDynamiteTimer <= uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim()->GetPositionX(), m_creature->getVictim()->GetPositionY(), m_creature->getVictim()->GetPositionZ(), SPELL_THROW_DYNAMITE, false);
            m_uiDynamiteTimer = urand(2300, 3900);
        }
        else
            m_uiDynamiteTimer -= uiDiff;

        if (m_uiPoisonTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THROW_POISON);
            m_uiPoisonTimer = urand(60000, 65000);
        }
        else
            m_uiPoisonTimer -= uiDiff;

		DoMeleeAttackIfReady();

    }

    // Function for synchronising the threat lists for all the goblins when they aren't summoned.
    void SynchroniseThreatLists()
    {
        if (!pInstance)
            return;

        ThreatManager* shared_list = pInstance->m_pSharedGoblinThreatList;

        if (!shared_list || m_creature->getThreatManager().isThreatListEmpty())
            return;

        ThreatList const& current_threatlist = m_creature->getThreatManager().getThreatList();
        for (HostileReference* current_reference : current_threatlist)
        {
            Unit* target_unit = current_reference->getTarget();

            if (!target_unit)
                continue;

            float own_threat = m_creature->getThreatManager().getThreat(target_unit);
            float previous_threat = m_PrevThreatValues[target_unit->GetObjectGuid()];

            // Synchronise the threat level between the creature's list and the shared list.
            shared_list->addThreatDirectly(target_unit, own_threat - previous_threat);

            // Save the current threat value for future comparison.
            m_PrevThreatValues[target_unit->GetObjectGuid()] = own_threat;
        }
    }

    // The goblins in the packs after Lashlayer should share threat threat lists so we're using
    // a custom targeting function and will have to sync the threat lists in the AI updating function.
    bool SelectHostileTargetWithCustomThreatList(ThreatManager* pThreatManager)
    {
        if (!pThreatManager)
            return false;

        if (!m_creature->isAlive())
            return false;

        Unit* target = NULL;

        // No taunt aura or taunt aura caster is dead, standard target selection
        if (!target && !pThreatManager->isThreatListEmpty())
            target = pThreatManager->getHostileTarget();

        if (target)
        {
            if (!m_creature->hasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_DIED)
                    && !m_creature->HasAuraType(SPELL_AURA_MOD_FEAR) && !m_creature->HasAuraType(SPELL_AURA_MOD_CONFUSE)) //Prevent unit from attacking while feared/confused
            {
                // Don't let the mob turn to face the player if it's fleeing.
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != ASSISTANCE_MOTION_TYPE && !m_creature->hasUnitState(UNIT_STAT_FLEEING_MOVE))
                    m_creature->SetInFront(target);

                AttackStart(target);

                // check if currently selected target is reachable
                // NOTE: path alrteady generated from AttackStart()
                bool inEvadeMode = m_creature->IsInEvadeMode();
                if(!m_creature->GetMotionMaster()->operator->()->IsReachable())
                {
                    if (!inEvadeMode)
                    {
                        m_creature->SetEvadeMode(true);
                    }

                    return false;
                } else if (inEvadeMode)
                {
                    m_creature->SetEvadeMode(false);
                }
            }
            return true;
        }

        // no target but something prevent go to evade mode
        if (!m_creature->isInCombat() || m_creature->HasAuraType(SPELL_AURA_MOD_TAUNT))
            return false;

        // last case when creature don't must go to evade mode:
        // it in combat but attacker not make any damage and not enter to aggro radius to have record in threat list
        // for example at owner command to pet attack some far away creature
        // Note: creature not have targeted movement generator but have attacker in this case
        if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
        {
            for(Unit::AttackerSet::const_iterator itr = m_creature->getAttackers().begin(); itr != m_creature->getAttackers().end(); ++itr)
            {
                if ((*itr)->IsInMap(m_creature) && (*itr)->isTargetableForAttack() && (*itr)->isInAccessablePlaceFor(m_creature))
                    return false;
            }
        }

        // enter in evade mode in other case
        if (pThreatManager->isThreatListEmpty())
        {
            ResetToHome();

            if (InstanceData* mapInstance = m_creature->GetInstanceData())
                mapInstance->OnCreatureEvade(m_creature);
        } else
            m_creature->SetEvadeMode(true);

        return false;
    }


};


CreatureAI* GetAI_mob_blackwing_technician(Creature* pCreature)
{
    return new mob_blackwing_technicianAI(pCreature);
}


/*############################
## mob_death_talon_overseer ##
############################*/

enum DeathTalonOverseer
{
    SPELL_FIRE_BLAST		= 20623,
    SPELL_CLEAVE		= 15284,
    SPELL_ELEMENTAL_SHIELD 	= 22276
};

struct MANGOS_DLL_DECL mob_death_talon_overseerAI : public ScriptedAI
{
    mob_death_talon_overseerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiFireBlastTimer;
    uint32 m_uiCleaveTimer;

    void Reset()
    {
        m_uiFireBlastTimer = urand(4000, 7000);
        m_uiCleaveTimer = urand(6000, 9000);
	
		// Set the magic vulnerability.
		DoCast(m_creature, SPELL_ELEMENTAL_SHIELD, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFireBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), DeathTalonOverseer::SPELL_FIRE_BLAST);

            m_uiFireBlastTimer = urand(4000, 7000);
        }
        else
            m_uiFireBlastTimer -= uiDiff;

        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), DeathTalonOverseer::SPELL_CLEAVE);

            m_uiCleaveTimer = urand(6000, 9000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_death_talon_oversser(Creature* pCreature)
{
    return new mob_death_talon_overseerAI(pCreature);
}

/*#############################
## mob_death_talon_wyrmguard ##
#############################*/

enum DeathTalonWyrmguard
{
    SPELL_WAR_STOMP	= 24375,
};

struct MANGOS_DLL_DECL mob_death_talon_wyrmguardAI : public ScriptedAI
{
    mob_death_talon_wyrmguardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
	//                         Red   Bronze  Blue  Black  Green
        uint32 brood_powers[] = { 22284, 22291, 22290, 22312, 22289 };
	
	m_uiSelectedBroodPower = brood_powers[urand(0, 4)]; // Select a random Brood Power for the dragon.

        Reset();
    }

    uint32 m_uiWarStompTimer;
    uint32 m_uiBroodPowerTimer;
    
    uint32 m_uiSelectedBroodPower;

    void Reset()
    {
		m_uiWarStompTimer = urand(10000, 35000);
	
		// Set magic vulnerability.
		DoCast(m_creature, SPELL_ELEMENTAL_SHIELD, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), DeathTalonWyrmguard::SPELL_WAR_STOMP);

			m_uiWarStompTimer = urand(10000, 35000);
        }
        else
            m_uiWarStompTimer -= uiDiff;
	
	

        if (DoMeleeAttackIfReady())
	{
	    // A 40 % chance to use the Brood Power on each melee attack.
	    if (urand(0, 99) < 40)
		DoCast(m_creature->getVictim(), m_uiSelectedBroodPower);
	}
		
    }
};

CreatureAI* GetAI_mob_death_talon_wyrmguard(Creature* pCreature)
{
    return new mob_death_talon_wyrmguardAI(pCreature);
}

/* AddSC */

void AddSC_blackwing_lair()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_demon_portal";
    pNewScript->GetAI = &GetAI_mob_demon_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_talon_trigger";
    pNewScript->GetAI = &GetAI_npc_death_talon_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_technician_pack_trigger";
    pNewScript->GetAI = &GetAI_npc_technician_pack_trigger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_technician_threat_holder";
    pNewScript->GetAI = &GetAI_npc_technician_threat_holder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blackwing_technician";
    pNewScript->GetAI = &GetAI_mob_blackwing_technician;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_death_talon_overseer";
    pNewScript->GetAI = &GetAI_mob_death_talon_oversser;
    pNewScript->RegisterSelf();

    pNewScript = new Script();
    pNewScript->Name = "mob_death_talon_wyrmguard";
    pNewScript->GetAI = &GetAI_mob_death_talon_wyrmguard;
    pNewScript->RegisterSelf();
}
