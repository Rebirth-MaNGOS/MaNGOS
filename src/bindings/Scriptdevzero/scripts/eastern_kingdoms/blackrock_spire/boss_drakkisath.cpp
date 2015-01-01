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
SDName: Boss_Drakkisath
SD%Complete: 90
SDComment: TODO: Make drakkisath aggro instead of respawning his adds when he returns after being kited
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eDrakkisath
{
    SPELL_CLEAVE          = 15284,
    SPELL_CONFLAGRATION   = 16805,
    SPELL_FLAMESTRIKE     = 16419,
    SPELL_PIERCE_ARMOR    = 12097,
    SPELL_RAGE            = 16789,
    SPELL_THUNDERCLAP     = 15588
};

struct MANGOS_DLL_DECL boss_drakkisathAI : public ScriptedAI
{
    boss_drakkisathAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiFlamestrikeTimer;
    uint32 m_uiPierceArmorTimer;
    uint32 m_uiThunderclapTimer;
	uint32 m_uiHelpTimer;
	std::list<Creature*> adds;

    void Reset()
    {
		m_uiHelpTimer = 1000;
        m_uiCleaveTimer = 5000;
        m_uiConflagrationTimer = 12000;
        m_uiFlamestrikeTimer = 8000;
        m_uiPierceArmorTimer = 2000;
        m_uiThunderclapTimer = 5000;
    }

    void Aggro(Unit* pWho)
    {
        //m_creature->CallForHelp(20.0f);
		if (adds.empty())
			GetCreatureListWithEntryInGrid(adds,m_creature,10814,20.0f);
		for (std::list<Creature*>::iterator itr = adds.begin(); itr != adds.end(); itr++)
		{
			Creature* pGuard = *itr;
			if (pGuard)
			{
				if (!pGuard->isAlive() && pGuard->isDead())
				{
				    pGuard->RemoveCorpse();
                    pGuard->Respawn();
				}

				pGuard->SetInCombatWithZone();
				pGuard->AddThreat(pWho,15.0f);
			}
		}

		m_creature->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKKISATH, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKKISATH, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKKISATH, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		// Call for help
		if (!adds.empty())
		{
			if (m_uiHelpTimer <= uiDiff)
			{
				m_uiHelpTimer = 1000;
				bool bossInCombat = m_creature->getVictim();
				bool addInCombat = false;
				for (std::list<Creature*>::iterator itr = adds.begin(); itr != adds.end(); itr++)
				{
					Creature* pGuard = *itr;
					if (pGuard && pGuard->isAlive())
					{
						addInCombat = pGuard->getVictim();
						if (addInCombat != bossInCombat)
						{
							if (bossInCombat)
							{
								pGuard->SetInCombatWithZone();
								if (pGuard->AI())
									pGuard->AI()->AttackStart(m_creature->getVictim());
							}
							else
							{
								m_creature->SetInCombatWithZone();
								m_creature->AI()->AttackStart(pGuard->getVictim());
							}
						}
					}
				}	
			}
			else
				m_uiHelpTimer -= uiDiff;
		}

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(5000,8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Conflagration
        if (m_uiConflagrationTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONFLAGRATION);
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1);
            m_creature->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
            m_uiConflagrationTimer = urand(17000,20000);
        }
        else
            m_uiConflagrationTimer -= uiDiff;

        // Flamestrike
        if (m_uiFlamestrikeTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan((pTarget ? pTarget : m_creature->getVictim()), SPELL_FLAMESTRIKE);
            m_uiFlamestrikeTimer = urand(10000,14000);
        }
        else
            m_uiFlamestrikeTimer -= uiDiff;

        // Pierce Armor
        if (m_uiPierceArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCE_ARMOR);
            m_uiPierceArmorTimer = urand(15000,20000);
        }
        else
            m_uiPierceArmorTimer -= uiDiff;

        // Thunderclap
        if (m_uiThunderclapTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_THUNDERCLAP);
            m_uiThunderclapTimer = urand(13000,17000);
        }
        else
            m_uiThunderclapTimer -= uiDiff;

        // Enrage at 20% hp
        if (HealthBelowPct(20))
            DoCastSpellIfCan(m_creature, SPELL_RAGE, CAST_AURA_NOT_PRESENT);

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_drakkisath(Creature* pCreature)
{
    return new boss_drakkisathAI(pCreature);
}

/*######
## mob_chromatic_elite_guard
######*/

enum eDrakkisathGuard
{
    SPELL_KNOCKDOWN       = 16790,
    SPELL_MORTAL_STRIKE   = 15708,
    SPELL_STRIKE          = 15580
};

struct MANGOS_DLL_DECL mob_chromatic_elite_guardAI : public ScriptedAI
{
    mob_chromatic_elite_guardAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiKnockdownTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiStrikeTimer;

    void Reset()
    {
        m_uiKnockdownTimer = urand(1000,3000);
		    m_uiMortalStrikeTimer = urand(5000,8000);
		    m_uiStrikeTimer = urand(10000,12000);
    }

    void Aggro(Unit* /*pVictim*/)
    {
        m_creature->CallForHelp(20.0f);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
		    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knockdown
        if (m_uiKnockdownTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKDOWN);
            m_uiKnockdownTimer = urand(8000,10000);
        }
        else
            m_uiKnockdownTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(5000,9000);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Strike
        if (m_uiStrikeTimer <= uiDiff)
        {
            // 80% chance to cast Strike
            if (urand(0, 100) <= 80)
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(6000,9000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_chromatic_elite_guard(Creature* pCreature)
{
    return new mob_chromatic_elite_guardAI(pCreature);
}

void AddSC_boss_drakkisath()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_drakkisath";
    pNewscript->GetAI = &GetAI_boss_drakkisath;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_chromatic_elite_guard";
    pNewscript->GetAI = &GetAI_mob_chromatic_elite_guard;
    pNewscript->RegisterSelf();
}
