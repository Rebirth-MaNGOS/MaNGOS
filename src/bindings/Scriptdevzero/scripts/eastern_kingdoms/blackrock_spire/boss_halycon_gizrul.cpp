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
SDName: Bosses Halycon and Gizrul The Slavener
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum Spells
{
    // Halycon
    SPELL_REND            = 13738,
    SPELL_TRASH           = 3391,
    // Gizrul
    SPELL_FATAL_BITE      = 16495,
    SPELL_FRENZY          = 8269,
    SPELL_INFECTED_BITE   = 16128
};

struct MANGOS_DLL_DECL boss_halyconAI : public ScriptedAI
{
    boss_halyconAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiRendTimer;
    uint32 m_uiTrashTimer;

    void Reset()
    {
        m_uiRendTimer = urand(3000,5000);
        m_uiTrashTimer = urand(8000,12000);
    }
    
    void JustDied(Unit* /*pKiller*/)
    {
        if (Creature* pGrizul = m_creature->SummonCreature(NPC_GIZRUL_THE_SLAVENER, -167.49f, -376.25f, 64.40f, 0, TEMPSUMMON_DEAD_DESPAWN, 60000))
        {
            pGrizul->GetMotionMaster()->MovePoint(0, -167.72f, -325.32f, 64.40f);
            CreatureCreatePos pos(pGrizul->GetMap(), -167.72f, -325.32f, 64.40f, 0.0f);
            pGrizul->SetSummonPoint(pos);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Rend
        if (m_uiRendTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);
            m_uiRendTimer = urand(5000,8000);
        }
        else
            m_uiRendTimer -= uiDiff;

        // Trash
        if (m_uiTrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRASH);
            m_uiTrashTimer = urand(4000,6000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_halycon(Creature* pCreature)
{
    return new boss_halyconAI(pCreature);
}

struct MANGOS_DLL_DECL boss_gizrul_the_slavenerAI : public ScriptedAI
{
    boss_gizrul_the_slavenerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFatalBiteTimer;
    uint32 m_uiInfectedBiteTimer;

    void Reset()
    {
        m_uiFatalBiteTimer = urand(8000,12000);
        m_uiInfectedBiteTimer = urand(3000,5000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (HealthBelowPct(50))
            DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT);

        // Fatal Bite
        if (m_uiFatalBiteTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FATAL_BITE);
            m_uiFatalBiteTimer = urand(3000,6000);
        }
        else
            m_uiFatalBiteTimer -= uiDiff;

        // Infected Bite
        if (m_uiInfectedBiteTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_INFECTED_BITE);
            m_uiInfectedBiteTimer = urand(10000,15000);
        }
        else
            m_uiInfectedBiteTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gizrul_the_slavener(Creature* pCreature)
{
    return new boss_gizrul_the_slavenerAI(pCreature);
}

void AddSC_boss_halycon_gizrul()
{
    Script* pNewscript;
    
    pNewscript = new Script;
    pNewscript->Name = "boss_halycon";
    pNewscript->GetAI = &GetAI_boss_halycon;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "boss_gizrul_the_slavener";
    pNewscript->GetAI = &GetAI_boss_gizrul_the_slavener;
    pNewscript->RegisterSelf();
}
