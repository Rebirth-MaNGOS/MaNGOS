/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Sartura
SD%Complete: 85
SDComment: Targeting currently doesn't work as expected
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "precompiled.h"
#include "temple_of_ahnqiraj.h"

enum
{
    SAY_AGGRO                   = -1531008,
    SAY_SLAY                    = -1531009,
    SAY_DEATH                   = -1531010,

    SPELL_WHIRLWIND             = 26083,
    SPELL_ENRAGE                = 28747,                    // Not sure if right ID.
    SPELL_ENRAGEHARD            = 28798,
    SPELL_SUNDERING_CLEAVE = 25174,

    // Guard Spell
    SPELL_WHIRLWIND_ADD         = 26038,
    SPELL_KNOCKBACK             = 26027,
    
    NPC_SARTURAS_ROYAL_GUARD    = 15984,
    NPC_BATTLEGUARD_SARTURA     = 15516
};

struct MANGOS_DLL_DECL boss_sarturaAI : public ScriptedAI
{
    boss_sarturaAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();           
    }
    
    ScriptedInstance* m_pInstance;
    uint32 m_uiWhirlWindTimer;
    uint32 m_uiWhirlWindRandomTimer;
    uint32 m_uiWhirlWindEndTimer;
    uint32 m_uiAggroResetTimer;
    uint32 m_uiAggroResetEndTimer;
    uint32 m_uiEnrageHardTimer;
    uint32 m_targetSwitch;
    uint32 m_uiSunderTimer;

    bool m_bIsEnraged;
    bool m_bIsEnragedHard;
    bool m_bIsWhirlWind;
    bool m_bAggroReset;

    void Reset()
    {
        m_uiSunderTimer = 3000;
        m_uiWhirlWindTimer = 30000;
        m_uiWhirlWindRandomTimer = urand(3000, 7000);
        m_uiWhirlWindEndTimer = 15000;
        m_uiAggroResetTimer = urand(45000, 55000);
        m_uiAggroResetEndTimer = 5000;
        m_uiEnrageHardTimer = 10*60000;
        m_targetSwitch = 5000;

        m_bIsWhirlWind = false;
        m_bAggroReset = false;
        m_bIsEnraged = false;
        m_bIsEnragedHard = false;
        RespawnGuards();
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTURA, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*pVictim*/)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        DoScriptText(SAY_DEATH, m_creature);
        
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTURA, DONE);
    }
    
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTURA, FAIL);
    }
    
    void RespawnGuards()
    {        
        // Respawn Guards 
        // long range so players won't exploit it by killing adds and then pulling boss far away and reset it to do the boss without adds
        std::list<Creature*> m_lRoyalGuards;
        GetCreatureListWithEntryInGrid(m_lRoyalGuards, m_creature, NPC_SARTURAS_ROYAL_GUARD, 250.f); 

        if (!m_lRoyalGuards.empty())
        {
            for(std::list<Creature*>::iterator itr = m_lRoyalGuards.begin(); itr != m_lRoyalGuards.end(); ++itr)
            {
                if ((*itr) && !(*itr)->isAlive())
                    (*itr)->Respawn();
            }
        }        
    }
    // Sätt random,1 target utan att ge threat på 5 sek och sedan till top aggro target
    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_bAggroReset)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;
        }
        
        if(m_creature->HasAura(SPELL_WHIRLWIND))
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);                
            m_bIsWhirlWind = true;
        }
        else
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
            m_bIsWhirlWind = false;
        }
                
        if (m_bIsWhirlWind)
        {
            // While whirlwind, switch to random targets often
            if (m_uiWhirlWindRandomTimer < uiDiff)
            {
                DoResetThreat();
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    m_creature->AddThreat(pTarget, 1000.f);
                m_uiWhirlWindRandomTimer = urand(3000, 7000);
            }
            else
                m_uiWhirlWindRandomTimer -= uiDiff;

            // End Whirlwind Phase
            if (m_uiWhirlWindEndTimer < uiDiff)
                DoResetThreat();
            else
                m_uiWhirlWindEndTimer -= uiDiff;
        }
        else // if (!m_bIsWhirlWind)
        {
            // Enter Whirlwind Phase
            if (m_uiWhirlWindTimer < uiDiff)
            {                
                m_creature->CastSpell(m_creature, SPELL_WHIRLWIND, true);
                
                DoResetThreat();
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    m_creature->AddThreat(pTarget, 1000.f);
                m_uiWhirlWindEndTimer = 15000;
                m_uiWhirlWindTimer = m_uiWhirlWindEndTimer + urand(8000, 12000);
            }
            else
                m_uiWhirlWindTimer -= uiDiff;

            // Aquire a new target sometimes
            if(m_uiAggroResetTimer)
            {
                if (m_uiAggroResetTimer < uiDiff)
                {
                    m_bAggroReset = true;
                    m_uiAggroResetTimer = 0;
                    m_uiAggroResetEndTimer = 10000;
                    m_targetSwitch = 5000;
                }
                else
                    m_uiAggroResetTimer -= uiDiff;
            }
            
            if (m_uiSunderTimer < uiDiff)
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SUNDERING_CLEAVE, false);
                m_uiSunderTimer = 5000;
            }
            else
                m_uiSunderTimer -= uiDiff;

            if (m_bAggroReset)
            {
                if (m_uiAggroResetEndTimer < uiDiff)
                {
                    m_bAggroReset = false;
                    m_uiAggroResetEndTimer = 0;
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        m_creature->AI()->AttackStart(pTarget);
                    m_uiAggroResetTimer = urand(10000, 15000);
                }
                else
                {
                    m_uiAggroResetEndTimer -= uiDiff;

                    if(m_targetSwitch)
                    {
                        if(m_targetSwitch <= uiDiff)
                        {
                            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                                m_creature->AI()->AttackStart(pTarget);
                            m_targetSwitch = 5000;
                        }
                        else
                            m_targetSwitch -= uiDiff;
                    }
                }
            }
        }

        // If she is 20% enrage
        if (!m_bIsEnraged && m_creature->GetHealthPercent() <= 20.0f)
		{
			if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE, m_bIsWhirlWind ? CAST_TRIGGERED : 0) == CAST_OK)
                m_bIsEnraged = true;
        }

        // After 10 minutes hard enrage
        if (!m_bIsEnragedHard)
        {
            if (m_uiEnrageHardTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_ENRAGEHARD, m_bIsWhirlWind ? CAST_TRIGGERED : 0) == CAST_OK)
                    m_bIsEnragedHard = true;
            }
            else
                m_uiEnrageHardTimer -= uiDiff;
        }

        // No melee damage while in whirlwind
        if (!m_bIsWhirlWind)
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_sartura_royal_guardAI : public ScriptedAI
{
    mob_sartura_royal_guardAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiWhirlWindTimer;
    uint32 m_uiWhirlWindRandomTimer;
    uint32 m_uiWhirlWindEndTimer;
    uint32 m_uiAggroResetTimer;
    uint32 m_uiAggroResetEndTimer;
    uint32 m_uiKnockBackTimer;
    uint32 m_targetSwitch;

    bool m_IsWhirlWind;
    bool m_bAggroReset;

    void Reset()
    {
        m_uiWhirlWindTimer = 30000;
        m_uiWhirlWindRandomTimer = urand(3000, 5000);
        m_uiWhirlWindEndTimer = 15000;
        m_uiAggroResetTimer = urand(45000, 55000);
        m_uiAggroResetEndTimer = 5000;
        m_uiKnockBackTimer = 10000;
        m_targetSwitch = 5000;
        
        m_IsWhirlWind = false;
        m_bAggroReset = false;

        Creature* pSartura = m_creature->GetClosestCreatureWithEntry(m_creature, NPC_BATTLEGUARD_SARTURA, 300.0f);

        if(pSartura)
        {
            if(pSartura->isInCombat())
                pSartura->SetEvadeMode(true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_bAggroReset)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;
        }

         if(m_creature->HasAura(SPELL_WHIRLWIND_ADD))
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);                
            m_IsWhirlWind = true;
        }
        else
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, false);
            m_IsWhirlWind = false;
        }
                
        if (m_IsWhirlWind)
        {
            // While whirlwind, switch to random targets often
            if (m_uiWhirlWindRandomTimer < uiDiff)
            {
                DoResetThreat();
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    m_creature->AddThreat(pTarget, 1000.f);
                m_uiWhirlWindRandomTimer = urand(3000, 5000);
            }
            else
                m_uiWhirlWindRandomTimer -= uiDiff;

            // End Whirlwind Phase
            if (m_uiWhirlWindEndTimer < uiDiff)
                DoResetThreat();
            else
                m_uiWhirlWindEndTimer -= uiDiff;                  
        }
        else // if (!m_bIsWhirlWind)
        {
            // Knockback nearby enemies
            if (m_uiKnockBackTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_KNOCKBACK) == CAST_OK)
                    m_uiKnockBackTimer = urand(10000, 20000);
            }
            else
                m_uiKnockBackTimer -= uiDiff;
            
            // Enter Whirlwind Phase
            if (m_uiWhirlWindTimer < uiDiff)
            {
                m_creature->CastSpell(m_creature, SPELL_WHIRLWIND_ADD, true);
                
                DoResetThreat();
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    m_creature->AddThreat(pTarget, 1000.f);
                m_uiWhirlWindEndTimer = 15000;
                m_uiWhirlWindTimer = m_uiWhirlWindEndTimer + urand(8000, 12000);
            }
            else
                m_uiWhirlWindTimer -= uiDiff;

            // Aquire a new target sometimes
            if(m_uiAggroResetTimer)
            {
                if (m_uiAggroResetTimer < uiDiff)
                {
                    m_bAggroReset = true;
                    m_uiAggroResetTimer = 0;
                    m_uiAggroResetEndTimer = 10000;
                    m_targetSwitch = 5000;
                }
                else
                    m_uiAggroResetTimer -= uiDiff;
            }

            // Remove remaining taunts, TODO
            if (m_bAggroReset)
            {
                if (m_uiAggroResetEndTimer < uiDiff)
                {
                    m_bAggroReset = false;
                    m_uiAggroResetEndTimer = 0;
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        m_creature->AI()->AttackStart(pTarget);
                    m_uiAggroResetTimer = urand(10000, 15000);
                }
                else
                {
                    m_uiAggroResetEndTimer -= uiDiff;

                    if(m_targetSwitch)
                    {
                        if(m_targetSwitch <= uiDiff)
                        {
                            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                                m_creature->AI()->AttackStart(pTarget);
                            m_targetSwitch = 5000;
                        }
                        else
                            m_targetSwitch -= uiDiff;
                    }
                }
            }
        }        

        // No melee damage while in whirlwind
        if (!m_IsWhirlWind)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sartura(Creature* pCreature)
{
    return new boss_sarturaAI(pCreature);
}

CreatureAI* GetAI_mob_sartura_royal_guard(Creature* pCreature)
{
    return new mob_sartura_royal_guardAI(pCreature);
}

void AddSC_boss_sartura()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_sartura";
    pNewScript->GetAI = &GetAI_boss_sartura;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_sartura_royal_guard";
    pNewScript->GetAI = &GetAI_mob_sartura_royal_guard;
    pNewScript->RegisterSelf();
}
