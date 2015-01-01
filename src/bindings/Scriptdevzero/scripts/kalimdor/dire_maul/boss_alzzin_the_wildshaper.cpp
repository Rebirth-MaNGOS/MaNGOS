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
SDName: Boss Alzzin The WildShaper
SD%Complete: 95%
SDComment: (TODO: check timers, aggro all mobs in Conservatory on Wave summon)
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_DIRE_WOLF_FORM          = 22660,
    SPELL_DISARM                  = 22691,
    SPELL_ENERVATE                = 22661,
    SPELL_KNOCK_AWAY              = 10101,
    SPELL_MANGLE                  = 22689,
    SPELL_THORNS                  = 22128,
    SPELL_TREE_FORM               = 22688,
    SPELL_VICIOUS_BITE            = 19319,
    SPELL_WILD_REGENERATION       = 7948,
    SPELL_WITHER                  = 22662,
};

enum Phases
{
    PHASE_DEMON                   = 0,
    PHASE_WOLF                    = 1,
    PHASE_TREE                    = 2,
};

struct MANGOS_DLL_DECL boss_alzzin_the_wildshaperAI : public ScriptedAI
{
    boss_alzzin_the_wildshaperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    bool m_bWallCrumbled;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiDisarmTimer;
    uint32 m_uiEnervateTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiMangleTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiThornsTimer;
    uint32 m_uiViciousBiteTimer;
    uint32 m_uiWildRegenerationTimer;
    uint32 m_uiWitherTimer;

    void Reset()
    {
        m_bWallCrumbled = false;

        m_uiSummonTimer = 15000;
        m_uiPhase = 0;
        m_uiPhaseTimer = 10000;
        m_uiDisarmTimer = urand(3000,4000);
        m_uiEnervateTimer = urand(7000,9000);
        m_uiKnockAwayTimer = urand(5000,7000);
        m_uiMangleTimer = urand(5000,7000);
        m_uiViciousBiteTimer = urand(2000,3000);
        m_uiWildRegenerationTimer = urand(5000,6000);
        m_uiWitherTimer = urand(2000,4000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALZZIN_THE_WILDSHAPER, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALZZIN_THE_WILDSHAPER, DONE);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
        if (!m_creature->getVictim() && !pTarget)
            return;

        pSummoned->AI()->AttackStart(pTarget ? pTarget : m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Change Phase
        if (m_uiPhaseTimer <= uiDiff)
        {
            uint32 currPhase = m_uiPhase;
            while(currPhase == m_uiPhase)
            {
                switch(urand(0,2))
                {
                    case PHASE_DEMON:
                        m_uiPhase = PHASE_DEMON;
                        m_creature->RemoveAurasDueToSpell(SPELL_DIRE_WOLF_FORM);
                        m_creature->RemoveAurasDueToSpell(SPELL_TREE_FORM);
                        break;
                    case PHASE_WOLF:
                        m_uiPhase = PHASE_WOLF;
                        DoCastSpellIfCan(m_creature, SPELL_DIRE_WOLF_FORM, CAST_INTERRUPT_PREVIOUS);
                        break;
                    case PHASE_TREE:
                        m_uiPhase = PHASE_TREE;
                        DoCastSpellIfCan(m_creature, SPELL_TREE_FORM, CAST_INTERRUPT_PREVIOUS);
                        break;
                }
            }
            m_uiPhaseTimer = 30000;
        }
        else
            m_uiPhaseTimer -= uiDiff;

        if (m_uiPhase == PHASE_WOLF)
        {
            // Mangle
            if (m_uiMangleTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANGLE);
                m_uiMangleTimer = urand(6000,8000);
            }
            else
                m_uiMangleTimer -= uiDiff;

            // Vicious Bite
            if (m_uiViciousBiteTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_VICIOUS_BITE);
                m_uiViciousBiteTimer = urand(3000,5000);
            }
            else
                m_uiViciousBiteTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_TREE)
        {
            // Wild Regeneration
            if (m_uiWildRegenerationTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, SPELL_VICIOUS_BITE);
                m_uiWildRegenerationTimer = urand(6000,8000);
            }
            else
                m_uiWildRegenerationTimer -= uiDiff;

            // Wither
            if (m_uiWitherTimer <= uiDiff)
            {
                Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_WITHER);
                m_uiWitherTimer = urand(4000,6000);
            }
            else
                m_uiWitherTimer -= uiDiff;
        }

        // Disarm
        if (m_uiDisarmTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DISARM);
            m_uiDisarmTimer = urand(6000,8000);
        }
        else
            m_uiDisarmTimer -= uiDiff;

        // Knock Away
        if (m_uiKnockAwayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(7000,9000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Enervate Volley
        if (m_uiEnervateTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_ENERVATE);
            m_uiEnervateTimer = urand(6000,7000);
        }
        else
            m_uiEnervateTimer -= uiDiff;

        // Summon Minions
        if (HealthBelowPct(50))
        {
            if (!m_bWallCrumbled)
            {
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_ALZZIN_THE_WILDSHAPER, SPECIAL);

                m_creature->CallForHelp(DEFAULT_VISIBILITY_INSTANCE);
                m_bWallCrumbled = true;
            }

            if (m_uiSummonTimer <= uiDiff)
            {
                m_creature->SummonCreature(NPC_ALZZINS_MINION, 254.67f, -406.95f, -119.96f, 4.74f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                m_uiSummonTimer = urand(5000,6000);
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        // Thorns
        if (HealthBelowPct(30))
            DoCastSpellIfCan(m_creature, SPELL_THORNS, CAST_AURA_NOT_PRESENT);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_alzzin_the_wildshaper(Creature* pCreature)
{
    return new boss_alzzin_the_wildshaperAI(pCreature);
}

void AddSC_boss_alzzin_the_wildshaper()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_alzzin_the_wildshaper";
    pNewscript->GetAI = &GetAI_boss_alzzin_the_wildshaper;
    pNewscript->RegisterSelf();
}
