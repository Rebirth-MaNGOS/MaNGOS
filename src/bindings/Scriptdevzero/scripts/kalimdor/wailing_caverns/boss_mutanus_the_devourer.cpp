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
SDName: Boss Mutanus the Devourer
SD%Complete: 100
SDComment:
SDCategory: Wailing Caverns
EndScriptData */

#include "precompiled.h"
#include "wailing_caverns.h"

enum eMutanusTheDevourer
{
    SPELL_NARALEXS_NIGHTMARE    = 7967,
    SPELL_THUNDERCRACK          = 8150,
    SPELL_TERRIFY               = 7399
};


struct MANGOS_DLL_DECL boss_mutanus_the_devourerAI : public ScriptedAI
{
    boss_mutanus_the_devourerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_wailing_caverns*)pCreature->GetInstanceData(); 
        Reset();
    }
    
    instance_wailing_caverns* m_pInstance;

    uint32 m_uiNaralexsNightmareTimer;
    uint32 m_uiThundercrackTimer;
    uint32 m_uiTerrifyTimer;

    void Reset()
    {
        m_uiNaralexsNightmareTimer = urand(6000, 11000);
        m_uiThundercrackTimer = urand(10000, 14000);
        m_uiTerrifyTimer = urand(15000, 21000);
    }

    //void Aggro(Unit* /*pAttacker*/) {}

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MUTANUS_THE_DEVOURER, DONE);
    }
    
    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MUTANUS_THE_DEVOURER, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Naralex's Nightmare
        if (m_uiNaralexsNightmareTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (pTarget->GetEntry() != NPC_DISCIPLE_OF_NARALEX)
                    DoCastSpellIfCan(pTarget, SPELL_NARALEXS_NIGHTMARE, CAST_INTERRUPT_PREVIOUS);
            }
            m_uiNaralexsNightmareTimer = urand(25000, 36000);
        }
        else
            m_uiNaralexsNightmareTimer -= uiDiff;

        // Thundercrack
        if (m_uiThundercrackTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_THUNDERCRACK) == CAST_OK)
                m_uiThundercrackTimer = urand(10000, 16000);
        }
        else
            m_uiThundercrackTimer -= uiDiff;

        // Terrify
        if (m_uiTerrifyTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (DoCastSpellIfCan(pTarget, SPELL_TERRIFY, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                    m_uiTerrifyTimer = urand(18000, 24000);
        }
        else
            m_uiTerrifyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mutanus_the_devourer(Creature* pCreature)
{
    return new boss_mutanus_the_devourerAI(pCreature);
}

void AddSC_boss_mutanus_the_devourer()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mutanus_the_devourer";
    pNewScript->GetAI = &GetAI_boss_mutanus_the_devourer;
    pNewScript->RegisterSelf();
}
