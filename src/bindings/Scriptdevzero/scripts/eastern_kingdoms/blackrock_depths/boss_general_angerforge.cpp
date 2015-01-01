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
SDName: Boss_General_Angerforge
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "blackrock_depths.h"

/*######
## mob_anvilrage_reservist
######*/

enum eAnvilrageReservist
{
	SPELL_BATTLE_SHOUT  = 9128,
	SPELL_HAMSTRING     = 9080,
	SPELL_SHOOT         = 6660,
	SPELL_STRIKE        = 11976,
	SPELL_RUN_AWAY      = 8225,
};

struct MANGOS_DLL_DECL mob_anvilrage_reservistAI : public npc_escortAI
{
    mob_anvilrage_reservistAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

	uint32 m_uiHamstringTimer;
	uint32 m_uiShootTimer;
	uint32 m_uiStrikeTimer;
	uint32 m_uiMovementStartTimer;

	bool bCanWalk;
	bool bWasFeared;

	void Reset()
	{
		m_uiHamstringTimer = urand(4000, 6000);
		m_uiShootTimer = urand(2500, 3200);
		m_uiStrikeTimer = urand(8000, 11000);
        m_uiMovementStartTimer = 0;

		bCanWalk = true;
		bWasFeared = false;
	}

    void Aggro(Unit* pWho)
	{
        if (m_creature->IsWithinDistInMap(pWho, ATTACK_DISTANCE))
		{
            // Melee
            DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
        }
	}

	void WaypointReached(uint32 uiPointId)
	{
		if (uiPointId == 8)
		{
			bCanWalk = false;
		}
	}

    void DoStartMovementToGeneral()
    {
        m_uiMovementStartTimer = urand(0, 1500);
    }

	void UpdateAI(const uint32 uiDiff)
	{
		if (m_uiMovementStartTimer)
        {
		    if (m_uiMovementStartTimer <= uiDiff)
		    {
                m_uiMovementStartTimer = 0;     // do not start again
			    Start(true);
		    }
		    else
                m_uiMovementStartTimer -= uiDiff;
        }

        if (bCanWalk)
			npc_escortAI::UpdateAI(uiDiff);

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

        if (!bWasFeared && HealthBelowPct(16))
		{
            bWasFeared = true;
            DoCastSpellIfCan(m_creature, SPELL_RUN_AWAY, CAST_INTERRUPT_PREVIOUS);
			return;
		}

        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
        {
            // Melee
            m_creature->clearUnitState(UNIT_STAT_ROOT);
            m_creature->SetSheath(SHEATH_STATE_MELEE);

            // Hamstring
			if (m_uiHamstringTimer <= uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMSTRING);
				m_uiHamstringTimer = urand(12000, 15000);
			}
			else
                m_uiHamstringTimer -= uiDiff;

			// Strike
			if (m_uiStrikeTimer <= uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
				m_uiStrikeTimer = urand(6000, 9000);
			}
			else
                m_uiStrikeTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30.0f) && m_creature->IsWithinLOSInMap(m_creature->getVictim()))
        {
            // Ranged
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_ROOT);
            m_creature->SetSheath(SHEATH_STATE_RANGED);

            // Shoot
			if (m_uiShootTimer <= uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
				m_uiShootTimer = urand(2200, 3800);
			}
			else
                m_uiShootTimer -= uiDiff;
        }
        else
        {
            // Out of range, must come closer..
            m_creature->clearUnitState(UNIT_STAT_ROOT);
        }
	}
};

CreatureAI* GetAI_mob_anvilrage_reservist(Creature* pCreature)
{
	return new mob_anvilrage_reservistAI (pCreature);
}

/*######
## mob_anvilrage_medic
######*/

enum eAnvilrageMedic
{
	SPELL_HEAL                  = 15586,
	SPELL_MIND_BLAST            = 15587,
	SPELL_POWER_WORD_FORTITUDE  = 13864,
	SPELL_PRAYER_OF_HEALING	    = 15585,
};

struct MANGOS_DLL_DECL mob_anvilrage_medicAI : public npc_escortAI
{
	mob_anvilrage_medicAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

	uint32 m_uiHealTimer;
	uint32 m_uiMindBlastTimer;
	uint32 m_uiPrayerOfHealingTimer;
	uint32 m_uiMovementStartTimer;
	
    bool bCanWalk;
	bool bWasFeared;

	void Reset()
	{
		m_uiHealTimer = 10000;             // these times are probably wrong
		m_uiMindBlastTimer = 10000;
		m_uiPrayerOfHealingTimer = 15000;
        m_uiMovementStartTimer = 0;
		
		bCanWalk = true;
		bWasFeared = false;
	}

    void Aggro(Unit* pWho)
    {
        if (m_creature->IsWithinDistInMap(pWho, 30.0f))
        {
            m_creature->addUnitState(UNIT_STAT_ROOT);
        }
        DoCastSpellIfCan(m_creature, SPELL_POWER_WORD_FORTITUDE, CAST_INTERRUPT_PREVIOUS);
    }

	void WaypointReached(uint32 uiPointId)
	{
		if (uiPointId == 8)
		{
			bCanWalk = false;
		}
	}

    void DoStartMovementToGeneral()
    {
        m_uiMovementStartTimer = urand(0, 1500);
    }

	void UpdateAI(const uint32 uiDiff)
	{
        if (m_uiMovementStartTimer)
        {
		    if (m_uiMovementStartTimer <= uiDiff)
		    {
                m_uiMovementStartTimer = 0;     // do not start again
			    Start(true);
		    }
		    else
                m_uiMovementStartTimer -= uiDiff;
        }

        if (!bWasFeared && HealthBelowPct(16))
		{
            bWasFeared = true;
			DoCastSpellIfCan(m_creature, SPELL_RUN_AWAY, CAST_INTERRUPT_PREVIOUS);
            return;
		}

        if (bCanWalk)
			npc_escortAI::UpdateAI(uiDiff);

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30.0f) && m_creature->IsWithinLOSInMap(m_creature->getVictim()))
        {
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_ROOT);
        }
        else
            // Out of range, must come closer..
            m_creature->clearUnitState(UNIT_STAT_ROOT);

		// Heal
		if (m_uiHealTimer <= uiDiff)
		{
			if (Unit* pAlliedUnit = DoSelectLowestHpFriendly(30.0f, 1300))
				DoCastSpellIfCan(pAlliedUnit, SPELL_HEAL);

			m_uiHealTimer = 10000;
		}
		else
            m_uiHealTimer -= uiDiff;

		// Mind Blast
		if (m_uiMindBlastTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
			m_uiMindBlastTimer = urand(5000, 10000);
		}
		else
            m_uiMindBlastTimer -= uiDiff;

		// Prayer of Healing
		if (m_uiPrayerOfHealingTimer <= uiDiff)
		{
            if (DoSelectLowestHpFriendly(30.0f, 1000))
			    DoCastSpellIfCan(m_creature, SPELL_PRAYER_OF_HEALING);

			m_uiPrayerOfHealingTimer = 15000;
		}
		else
            m_uiPrayerOfHealingTimer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_anvilrage_medic(Creature* pCreature)
{
	return new mob_anvilrage_medicAI (pCreature);
}

/*######
## boss_general_angerforge
######*/

enum eGeneralAngerforge
{
	SPELL_MIGHTYBLOW		= 14099,
	//SPELL_HAMSTRING		= 9080,
	SPELL_CLEAVE			= 20691,
	SPELL_SUNDER_ARMOR		= 15572,
};

struct MANGOS_DLL_DECL boss_general_angerforgeAI : public ScriptedAI
{
    boss_general_angerforgeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiSunderArmorTimer;

    void Reset()
    {
		m_uiSunderArmorTimer = 2000;
    }

	void JustSummoned(Creature* pSummoned)
	{
		switch(pSummoned->GetEntry())
		{
			case NPC_ANVILRAGE_RESERVIST:
            {
                CreatureCreatePos pos(pSummoned->GetMap(), 663.45f, 35.55f, -60.03f, 0.0f);
                pSummoned->SetSummonPoint(pos);
                
				if (mob_anvilrage_reservistAI* pEscortAI = dynamic_cast<mob_anvilrage_reservistAI*>(pSummoned->AI()))
					pEscortAI->DoStartMovementToGeneral();
                break;
            }
			case NPC_ANVILRAGE_MEDIC:
            {
                CreatureCreatePos pos(pSummoned->GetMap(), 663.45f, 35.55f, -60.03f, 0.0f);
                pSummoned->SetSummonPoint(pos);
				if (mob_anvilrage_medicAI* pEscortAI = dynamic_cast<mob_anvilrage_medicAI*>(pSummoned->AI()))
					pEscortAI->DoStartMovementToGeneral();
                break;
            }
		}
	}

	void JustDied(Unit *)
	{
		m_creature->SummonCreature(NPC_ANVILRAGE_RESERVIST, 724.34f, 21.86f, -45.41f, 3.10f, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_ANVILRAGE_RESERVIST, 724.34f, 21.86f, -45.41f, 3.10f, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_ANVILRAGE_RESERVIST, 724.34f, 21.86f, -45.41f, 3.10f, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_ANVILRAGE_MEDIC,     724.34f, 21.86f, -45.41f, 3.10f, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->SummonCreature(NPC_ANVILRAGE_MEDIC,     724.34f, 21.86f, -45.41f, 3.10f, TEMPSUMMON_DEAD_DESPAWN, 0);
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		// Sunder Armor
		if (m_uiSunderArmorTimer <= uiDiff)
		{
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
			m_uiSunderArmorTimer = urand(5000, 8000);
		}
		else
            m_uiSunderArmorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_general_angerforge(Creature* pCreature)
{
    return new boss_general_angerforgeAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_boss_general_angerforge()
{
    Script* pNewScript;

    pNewScript = new Script;
	pNewScript->Name = "mob_anvilrage_reservist";
    pNewScript->GetAI = &GetAI_mob_anvilrage_reservist;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "mob_anvilrage_medic";
    pNewScript->GetAI = &GetAI_mob_anvilrage_medic;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "boss_general_angerforge";
    pNewScript->GetAI = &GetAI_boss_general_angerforge;
    pNewScript->RegisterSelf();
}
