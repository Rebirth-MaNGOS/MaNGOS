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
SDName: Boss_Wushoolay
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eWushoolay
{
    SPELL_CHAIN_LIGHTNING  = 24680,
    //SPELL_FORKED_LIGHTNING = 24682,   // since TBC ?
    //SPELL_LIGHTNING_CLOUD  = 24683,   // since TBC ?

    SPELL_POISON_AURA      = 29865,
    SPELL_HURRICANE        = 27530,
};

struct MANGOS_DLL_DECL boss_wushoolayAI : public ScriptedAI
{
    boss_wushoolayAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiChainLightningTimer;
    uint32 m_uiHurricaneTimer;
    uint32 m_uiPoisonAuraTimer;
    //uint32 m_uiForkedLightningTimer;
    //uint32 m_uiLightningCloudTimer;
    
    void Reset()
    {
        m_uiHurricaneTimer = urand(12000,16000);
        m_uiPoisonAuraTimer = urand(5000, 10000);
        m_uiChainLightningTimer = urand(8000, 16000);
        //m_uiForkedLightningTimer = urand(12000,16000);
        //m_uiLightningCloudTimer = urand(5000, 10000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Hurricane
        if (m_uiHurricaneTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_HURRICANE);
            m_uiHurricaneTimer = urand(20000, 25000);
        }
        else
            m_uiHurricaneTimer -= uiDiff;

        // Poison Aura
        if (m_uiPoisonAuraTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_POISON_AURA);
            m_uiPoisonAuraTimer = urand(15000, 20000);
        }
        else
            m_uiPoisonAuraTimer -= uiDiff;

        // Chain Lightning
        if (m_uiChainLightningTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(12000, 16000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        // Forked Lightning
        /*if (m_uiForkedLightningTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FORKED_LIGHTNING);
            m_uiForkedLightningTimer = urand(10000, 15000);
        }
        else
            m_uiForkedLightningTimer -= uiDiff;*/

        // Lightning Cloud
        /*if (m_uiLightningCloudTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LIGHTNING_CLOUD);
            m_uiLightningCloudTimer = urand(15000, 20000);
        }
        else
            m_uiLightningCloudTimer -= uiDiff;*/

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_wushoolay(Creature* pCreature)
{
    return new boss_wushoolayAI(pCreature);
}

void AddSC_boss_wushoolay()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_wushoolay";
    pNewScript->GetAI = &GetAI_boss_wushoolay;
    pNewScript->RegisterSelf();
}
