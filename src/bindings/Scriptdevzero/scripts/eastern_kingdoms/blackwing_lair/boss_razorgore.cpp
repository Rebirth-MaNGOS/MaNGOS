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
SDName: Boss_Razorgore
SD%Complete: 100
SDComment: 
SDCategory: Blackwing Lair
EndScriptData */

#include "precompiled.h"
#include "blackwing_lair.h"
#include "../../src/game/TemporarySummon.h"

// Razorgore Phase 2 Script
enum
{

    SAY_EGGS_BROKEN_1           = -1469022,
    SAY_EGGS_BROKEN_2           = -1469023,
    SAY_EGGS_BROKEN_3           = -1469024,
    SAY_DEATH                   = -1469025,

    SPELL_CLEAVE                = 19632,
    SPELL_WARSTOMP              = 24375,
    SPELL_FIREBALL_VOLLEY       = 22425,
    SPELL_CONFLAGRATION         = 23023,

	SPELL_EXPLODE_ORB			= 20037,
	SPELL_POSSESS				= 23014,
	SPELL_ORB_MIND_CONTROL		= 19869,
	SPELL_ORB_USAGE				= 23018,
	SPELL_ORB_SHRINK_EFFECT		= 23021,
	SPELL_MIND_EXHAUSTION		= 23958
};

struct MANGOS_DLL_DECL boss_razorgoreAI : public ScriptedAI
{
    boss_razorgoreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackwing_lair*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackwing_lair* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiWarStompTimer;
    uint32 m_uiFireballVolleyTimer;
    uint32 m_uiConflagrationTimer;

    void Reset()
    {
        m_uiCleaveTimer         = 15000;                       // These times are probably wrong
        m_uiWarStompTimer       = 35000;
        m_uiConflagrationTimer  = 12000;
        m_uiFireballVolleyTimer = 7000;

		if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) && !m_creature->GetCharmerOrOwner())
			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_RAZORGORE, IN_PROGRESS);
			if (m_pInstance->GetRazorgorePhase() == 0)
				m_pInstance->SetRazorgorePhase(1);

			Creature* grethok = m_pInstance->GetSingleCreatureFromStorage(NPC_GRETHOK);
			if (grethok)
			{
				grethok->SetInCombatWithZone();
				if (grethok->getVictim())
					grethok->getThreatManager().addThreat(grethok->getVictim(), 1.f);
			}

		}

		m_creature->SetInCombatWithZone();

    }


    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
		{
			if (m_pInstance->GetRazorgorePhase() == 1)
			{

				DoScriptText(SAY_DEATH, m_creature);
				m_creature->CastSpell(m_creature, SPELL_EXPLODE_ORB, true);			

				Player* current_victim = GetRandomPlayerInCurrentMap();

				if (current_victim)
				{
					Group* victim_group = current_victim->GetGroup();
					
					if (victim_group)
					{
						Group::MemberSlotList const& members = victim_group->GetMemberSlots();

						for (Group::MemberSlot const current_player_ref : members)
						{
							Player* current_player = m_creature->GetMap()->GetPlayer(current_player_ref.guid);

							if (current_player)
								m_creature->DealDamage(current_player, current_player->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, true);
						}

					}
				}	

				m_pInstance->SetRazorgorePhase(0);
				m_pInstance->SetData(TYPE_RAZORGORE, FAIL);
			}
			else
				m_pInstance->SetData(TYPE_RAZORGORE, DONE);
    
		}
	}

    void JustReachedHome()
    {
        if (m_pInstance)
		{
            m_pInstance->SetData(TYPE_RAZORGORE, FAIL);
			m_pInstance->ClearRazorgoreSpawns();
			m_pInstance->SetRazorgorePhase(0);
			Reset();
		}		
    }

	void JustRespawned()
	{
		if (m_pInstance)
		{
			Creature* grethok = m_pInstance->GetSingleCreatureFromStorage(NPC_GRETHOK);

			if (grethok && !grethok->HasAura(SPELL_ORB_USAGE))
				grethok->CastSpell(grethok, SPELL_ORB_USAGE, true);

		}

		
	}


    void UpdateAI(const uint32 uiDiff)
    {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		switch(m_pInstance->GetRazorgorePhase())
		{
		case 2:
			{
				// Cleave
				if (m_uiCleaveTimer < uiDiff)
				{
					if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
						m_uiCleaveTimer = urand(7000, 10000);
				}
				else
					m_uiCleaveTimer -= uiDiff;

				// War Stomp
				if (m_uiWarStompTimer < uiDiff)
				{
					if (DoCastSpellIfCan(m_creature, SPELL_WARSTOMP) == CAST_OK)
						m_uiWarStompTimer = urand(15000, 25000);
				}
				else
					m_uiWarStompTimer -= uiDiff;

				// Fireball Volley
				if (m_uiFireballVolleyTimer < uiDiff)
				{
					if (DoCastSpellIfCan(m_creature, SPELL_FIREBALL_VOLLEY) == CAST_OK)
						m_uiFireballVolleyTimer = urand(12000, 15000);
				}
				else
					m_uiFireballVolleyTimer -= uiDiff;

				// Conflagration
				if (m_uiConflagrationTimer < uiDiff)
				{
					if (DoCastSpellIfCan(m_creature, SPELL_CONFLAGRATION) == CAST_OK)
						m_uiConflagrationTimer = 12000;
				}
				else
					m_uiConflagrationTimer -= uiDiff;

				break;
			}
		default:
			break;
		}

        /* This is obsolete code, not working anymore, keep as reference, should be handled in core though
        * // Aura Check. If the gamer is affected by confliguration we attack a random gamer.
        * if (m_creature->getVictim()->HasAura(SPELL_CONFLAGRATION, EFFECT_INDEX_0))
        * {
        *     if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
        *         m_creature->TauntApply(pTarget);
        * }
        */

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_razorgore(Creature* pCreature)
{
    return new boss_razorgoreAI(pCreature);
}

/*############################
## mob_orb_of_domination_trigger
############################*/

struct MANGOS_DLL_DECL npc_orb_of_domination_triggerAI : public ScriptedAI
{
    npc_orb_of_domination_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustDied(Unit* /*pKiller*/)
    {
    }

    void JustReachedHome()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {

		Creature* razorgore = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE, true);
		if (razorgore)
		{
			if (!razorgore->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) && razorgore->isInCombat())
			{
				if (m_creature->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))		// If Razorgore isn't MC'ed the trigger shouldn't channel anymore.
					m_creature->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, false);

				if (razorgore->HasAura(SPELL_ORB_SHRINK_EFFECT))									// Remove the shrink effect when the MC is broken.
					for (int i = 0; i < 2; i++)
						razorgore->RemoveAura(SPELL_ORB_SHRINK_EFFECT, (SpellEffectIndex)(EFFECT_INDEX_0 + i));
			}
			else if (!m_creature->GetCurrentSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL))		// If Razorgore is MC'ed, make sure we're channeling.
			{
				m_creature->CastSpell(m_creature, SPELL_POSSESS, true);
			}
		}

	}
};

CreatureAI* GetAI_npc_orb_of_domination_trigger(Creature* pCreature)
{
    return new npc_orb_of_domination_triggerAI(pCreature);
}


enum RazorgoreAdd
{
	SPELL_RAZORGORE_STRIKE		= 15580,
	SPELL_RAZORGORE_CLEAVE		= 15284,
	SPELL_RAZORGORE_DRAGONBANE	= 23967
};

/*############################
## mob_death_talon_dragonspawn
############################*/

struct MANGOS_DLL_DECL mob_death_talon_dragonspawnAI : public ScriptedAI
{

	Creature* razorgore;
	instance_blackwing_lair* pInstance;

	uint32 m_uiStrikeTimer;
	uint32 m_uiCleaveTimer;
	uint32 m_uiDragonBaneTimer;

	uint32 m_uiForcedDespawnTimer;

    mob_death_talon_dragonspawnAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }


    void Reset()
    {
		pInstance = (instance_blackwing_lair*)m_creature->GetInstanceData();

		if (pInstance)
			razorgore = pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE);

		m_uiStrikeTimer = urand(5000, 7000);
		m_uiCleaveTimer = urand(9000, 13000);
		m_uiDragonBaneTimer = urand(11000, 15000);

		m_uiForcedDespawnTimer = 10000;
    }

	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
	
		switch(uiPointId)
		{
		case 1:
			{
				m_creature->SetInCombatWithZone();

				if (razorgore && razorgore->GetCharmerOrOwner())
					m_creature->Attack(razorgore->GetCharmerOrOwner(), false);
				else
				{
					Player* attack_player = GetRandomPlayerInCurrentMap();

					if (attack_player)
						m_creature->Attack(attack_player, false);
				}
				break;
			}
		case 2:
			((TemporarySummon*) m_creature)->UnSummon();
			break;
		}
	}


    void UpdateAI(const uint32 uiDiff)
    {
		
		if (pInstance && pInstance->GetRazorgorePhase() == 2)
		{
			if (m_uiForcedDespawnTimer <= uiDiff)
				((TemporarySummon*) m_creature)->UnSummon();
			else
				m_uiForcedDespawnTimer -= uiDiff;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiStrikeTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_STRIKE);

			m_uiStrikeTimer = urand(5000, 7000);
		}

		else
			m_uiStrikeTimer -= uiDiff;

		if (m_uiCleaveTimer <= uiDiff)
		{
			if (m_creature->GetDistance(m_creature->getVictim()) <= 8)
			{
				DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_CLEAVE);

				m_uiCleaveTimer = urand(11000, 14000);
			}
		}
		else
			m_uiCleaveTimer -= uiDiff;

		if (m_uiDragonBaneTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_DRAGONBANE);

			m_uiDragonBaneTimer = urand(12000, 16000);
		}
		else
			m_uiDragonBaneTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_death_talon_dragonspawn(Creature* pCreature)
{
    return new mob_death_talon_dragonspawnAI(pCreature);
}

/*############################
## mob_blackwing_legionnaire
############################*/

struct MANGOS_DLL_DECL mob_blackwing_legionnaireAI : public ScriptedAI
{

	Creature* razorgore;
	instance_blackwing_lair* pInstance;

	uint32 m_uiStrikeTimer;
	uint32 m_uiCleaveTimer;
	uint32 m_uiDragonBaneTimer;

	uint32 m_uiForcedDespawnTimer;

    mob_blackwing_legionnaireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }


   void Reset()
    {
		pInstance = (instance_blackwing_lair*)m_creature->GetInstanceData();

		if (pInstance)
			razorgore = pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE);

		m_uiStrikeTimer = urand(5000, 7000);
		m_uiCleaveTimer = urand(9000, 13000);
		m_uiDragonBaneTimer = urand(11000,15000);

		m_uiForcedDespawnTimer = 10000;
    }

	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
		switch(uiPointId)
		{
		case 1:
			{
				m_creature->SetInCombatWithZone();

				if (razorgore && razorgore->GetCharmerOrOwner())
					m_creature->Attack(razorgore->GetCharmerOrOwner(), false);
				else
				{
					Player* attack_player = GetRandomPlayerInCurrentMap();

					if (attack_player)
						m_creature->Attack(attack_player, false);
				}
				break;
			}
		case 2:
			((TemporarySummon*) m_creature)->UnSummon();

		}

	}


    void UpdateAI(const uint32 uiDiff)
    {

		if (pInstance && pInstance->GetRazorgorePhase() == 2)
		{
			if (m_uiForcedDespawnTimer <= uiDiff)
				((TemporarySummon*) m_creature)->UnSummon();
			else
				m_uiForcedDespawnTimer -= uiDiff;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiStrikeTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_STRIKE);

			m_uiStrikeTimer = urand(5000, 7000);
		}

		else
			m_uiStrikeTimer -= uiDiff;

		if (m_uiCleaveTimer <= uiDiff)
		{
			if (m_creature->GetDistance(m_creature->getVictim()) <= 8)
			{
				DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_CLEAVE);

				m_uiCleaveTimer = urand(11000, 14000);
			}
		}
		else
			m_uiCleaveTimer -= uiDiff;

		if (m_uiDragonBaneTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), RazorgoreAdd::SPELL_RAZORGORE_DRAGONBANE);

			m_uiDragonBaneTimer = urand(12000, 16000);
		}
		else
			m_uiDragonBaneTimer -= uiDiff;


		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_blackwing_legionnaire(Creature* pCreature)
{
    return new mob_blackwing_legionnaireAI(pCreature);
}

/*############################
## mob_blackwing_mage
############################*/

enum BlackwingMage
{
	SPELL_FIREBALL				= 17290,
	SPELL_ARCANE_EXPLOSION		= 22271
};


struct MANGOS_DLL_DECL mob_blackwing_mageAI : public ScriptedAI
{

	instance_blackwing_lair* pInstance;

	uint32 m_uiFireballTimer;
	uint32 m_uiArcaneExplosionTimer;

	uint32 m_uiForcedDespawnTimer;

    mob_blackwing_mageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }


   void Reset()
    {
		pInstance = (instance_blackwing_lair*)m_creature->GetInstanceData();

		m_uiFireballTimer = urand(3400, 4800);
		m_uiArcaneExplosionTimer = urand(7000, 12000);

		m_uiForcedDespawnTimer = 10000;
    }

	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
		if (uiPointId == 2)
			((TemporarySummon*) m_creature)->UnSummon();

	}


    void UpdateAI(const uint32 uiDiff)
    {
		
		if (pInstance && pInstance->GetRazorgorePhase() == 2)
		{
			if (m_uiForcedDespawnTimer <= uiDiff)
				((TemporarySummon*) m_creature)->UnSummon();
			else
				m_uiForcedDespawnTimer -= uiDiff;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiFireballTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), BlackwingMage::SPELL_FIREBALL);

			m_uiFireballTimer = urand(3400, 4800);
		}
		else
			m_uiFireballTimer -= uiDiff;

		if (m_uiArcaneExplosionTimer <= uiDiff)
		{
			if (m_creature->GetDistance(m_creature->getVictim()) <= 8)
			{
				DoCastSpellIfCan(m_creature->getVictim(), BlackwingMage::SPELL_ARCANE_EXPLOSION);

				m_uiArcaneExplosionTimer = urand(12000, 16000);
			}
		}
		else
			m_uiArcaneExplosionTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_blackwing_mage(Creature* pCreature)
{
    return new mob_blackwing_mageAI(pCreature);
}

/*############################
## mob_grethok_the_controller
############################*/

enum GrethokTheController
{
	SPELL_ARCANE_MISSILES		= 22273,
	SPELL_GREATER_POLYMORPH		= 22274,
	SPELL_DOMINATE_MIND			= 14515,
	SPELL_SLOW					= 13747
};


struct MANGOS_DLL_DECL mob_grethok_the_controllerAI : public ScriptedAI
{
	instance_blackwing_lair* pInstance;

	uint32 m_uiArcaneMissilesTimer;
	uint32 m_uiGreaterPolymorphTimer;
	uint32 m_uiDominateMindTimer;
	uint32 m_uiSlowTimer;
	bool m_bDummyMcTriggered;

    mob_grethok_the_controllerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }


   void Reset()
    {
		pInstance = (instance_blackwing_lair*)m_creature->GetInstanceData();

		m_uiArcaneMissilesTimer = urand(5000, 8000);
		m_uiGreaterPolymorphTimer = urand(8000, 11000);
		m_uiDominateMindTimer = urand(11000, 14000);
		m_uiSlowTimer = urand(15000, 21000);

		m_bDummyMcTriggered = false;

		if (pInstance)
		{
			Creature* orb_dummy = pInstance->GetSingleCreatureFromStorage(NPC_ORB_OF_DOMINATION_TRIGGER, true);
			Creature* razorgore = pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE, true);
			if (orb_dummy && razorgore && !razorgore->HasAura(SPELL_POSSESS))
			{
				orb_dummy->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, false);
				orb_dummy->CastSpell(orb_dummy, SPELL_POSSESS, true);
			}

			m_creature->CastSpell(m_creature, SPELL_ORB_USAGE, true);
		}

    }

	void Aggro(Unit* pWho)
	{
		if (pInstance && pInstance->GetRazorgorePhase() == 0 && pWho)
		{
			Creature* razorgore = pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE);
			razorgore->SetInCombatWith(pWho);
			razorgore->getThreatManager().addThreat(pWho, 1.f);
			razorgore->Attack(pWho, false);

			pInstance->SetRazorgorePhase(1);

			if (pInstance->GetData(TYPE_RAZORGORE) != IN_PROGRESS)
				pInstance->SetData(TYPE_RAZORGORE, IN_PROGRESS);

			Creature* orb_dummy = pInstance->GetSingleCreatureFromStorage(NPC_ORB_OF_DOMINATION_TRIGGER, true);
			if (orb_dummy && !razorgore->GetCharmerOrOwner())
				orb_dummy->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, false);
		}
	}



    void UpdateAI(const uint32 uiDiff)
    {
		
		if (!m_bDummyMcTriggered && pInstance && !m_creature->isInCombat() && !m_creature->HasAura(SPELL_ORB_USAGE))	
		{
			Creature* razorgore = pInstance->GetSingleCreatureFromStorage(NPC_RAZORGORE, true);
			Creature* orb_dummy = pInstance->GetSingleCreatureFromStorage(NPC_ORB_OF_DOMINATION_TRIGGER, true);
			if (orb_dummy && razorgore && !razorgore->HasAura(SPELL_POSSESS))
			{
				orb_dummy->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL, false);
				orb_dummy->CastSpell(orb_dummy, SPELL_POSSESS, true);
			}

			m_creature->CastSpell(m_creature, SPELL_ORB_USAGE, true);

			m_bDummyMcTriggered = true;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiArcaneMissilesTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), GrethokTheController::SPELL_ARCANE_MISSILES);

			m_uiArcaneMissilesTimer = urand(5000, 8000);
		}
		else
			m_uiArcaneMissilesTimer -= uiDiff;

		if (m_uiGreaterPolymorphTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), GrethokTheController::SPELL_GREATER_POLYMORPH);

			m_uiGreaterPolymorphTimer = urand(20000, 24000);
		}
		else
			m_uiGreaterPolymorphTimer -= uiDiff;

		if (m_uiDominateMindTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), GrethokTheController::SPELL_DOMINATE_MIND);

			m_uiDominateMindTimer = urand(16000, 20000);
		}
		else
			m_uiDominateMindTimer -= uiDiff;

		if (m_uiSlowTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), GrethokTheController::SPELL_SLOW);

			m_uiSlowTimer = urand(10000, 17000);
		}
		else
			m_uiSlowTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_grethok_the_controller(Creature* pCreature)
{
    return new mob_grethok_the_controllerAI(pCreature);
}

/*#####################
## go_orb_of_domination
#####################*/

bool GOUse_go_orb_of_domination(Player* pPlayer, GameObject* pGo)
{

	if (pPlayer && pGo)
	{
		instance_blackwing_lair* player_instance = (instance_blackwing_lair*) pPlayer->GetInstanceData();
		Creature* razorgore = player_instance->GetSingleCreatureFromStorage(NPC_RAZORGORE, true);
		Creature* orb_dummy = player_instance->GetSingleCreatureFromStorage(NPC_ORB_OF_DOMINATION_TRIGGER, true);

		if (player_instance->GetRazorgorePhase() == 2)								// Disable the orb during phase 2.
			return true;

		if (player_instance->GetRazorgorePhase() == 0)
		{
			player_instance->SetRazorgorePhase(1);
			player_instance->SetData(TYPE_RAZORGORE, IN_PROGRESS);
		}

		if (razorgore && orb_dummy)
		{

			if (!razorgore->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)) // Do not allow another player to "steal" the MC.
			{
				if (pPlayer->GetPet())														// If the player has a pet he cannot use the orb.
				{
					pPlayer->CastSpell(razorgore, SPELL_ORB_MIND_CONTROL, true);
					pPlayer->CastSpell(pPlayer, SPELL_MIND_EXHAUSTION, true);
					return true;
				}

				if (!pPlayer->HasAura(SPELL_MIND_EXHAUSTION))						// A player with the Mind Exhaustion debuff should not be able to use the orb.
				{
					ThreatList const& threat_list = razorgore->getThreatManager().getThreatList();
					if (!threat_list.empty())
					{
						HostileReference* top_aggro_reference = threat_list.front();
						if (top_aggro_reference)
							{
								Unit* top_aggro = top_aggro_reference->getTarget();

								if (top_aggro && razorgore->getThreatManager().getThreat(top_aggro) >= 99999999.f)
									razorgore->getThreatManager().addThreat(top_aggro, -99999999.f);
							}
					}

					razorgore->SetInCombatWith(pPlayer);
					razorgore->getThreatManager().addThreatDirectly(pPlayer, 99999999.f);
					razorgore->Attack(pPlayer, false);

					Creature* grethok = player_instance->GetSingleCreatureFromStorage(NPC_GRETHOK);
					if (grethok && !grethok->isInCombat() && grethok->isAlive())
					{
						grethok->SetInCombatWithZone();
						grethok->getThreatManager().addThreat(pPlayer, 1.f);
						grethok->Attack(pPlayer, false);
					}


					razorgore->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);	// Is cleared in core.

					pPlayer->CastSpell(pPlayer, SPELL_ORB_USAGE, true);
					pPlayer->CastSpell(pPlayer, SPELL_MIND_EXHAUSTION, true);
					pPlayer->CastSpell(razorgore, SPELL_ORB_MIND_CONTROL, false);
					razorgore->CastSpell(razorgore,  SPELL_ORB_SHRINK_EFFECT, true);

					if (!razorgore->HasAura(SPELL_POSSESS))
						orb_dummy->CastSpell(orb_dummy, SPELL_POSSESS, true);
				}
			}
			else
			{
				if (pPlayer->HasAura(SPELL_MIND_EXHAUSTION)) // If Razorgore is MC'ed and a player with the Mind Exhaustion spell uses the orb the MC should be broken.
					for (int i = 0; i < 3; i++)
						razorgore->RemoveAura(19832, (SpellEffectIndex) i);
			}
		}

	}

	return true;
}

bool GOUse_go_black_dragon_egg(Creature* pMob, GameObject* pGo)
{
	instance_blackwing_lair* pInstance = (instance_blackwing_lair*) pMob->GetInstanceData();

	if (pInstance)
		pInstance->RemoveRazorgoreEgg(pGo);

	int egg_say[3] = { SAY_EGGS_BROKEN_1, SAY_EGGS_BROKEN_2, SAY_EGGS_BROKEN_3 };
	DoScriptText(egg_say[urand(0, 2)], pMob);

	return false; // This function must return false for the egg handling to be done correctly.
}

void AddSC_boss_razorgore()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_razorgore";
    pNewScript->GetAI = &GetAI_boss_razorgore;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "go_orb_of_domination";
	pNewScript->pGOUse = &GOUse_go_orb_of_domination;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "go_black_dragon_egg";
	pNewScript->pGOMobUse = &GOUse_go_black_dragon_egg;
	pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "npc_orb_of_domination_trigger";
    pNewScript->GetAI = &GetAI_npc_orb_of_domination_trigger;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_death_talon_dragonspawn";
    pNewScript->GetAI = &GetAI_mob_death_talon_dragonspawn;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_blackwing_legionnaire";
    pNewScript->GetAI = &GetAI_mob_blackwing_legionnaire;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_blackwing_mage";
    pNewScript->GetAI = &GetAI_mob_blackwing_mage;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_grethok_the_controller";
    pNewScript->GetAI = &GetAI_mob_grethok_the_controller;
    pNewScript->RegisterSelf();
}
