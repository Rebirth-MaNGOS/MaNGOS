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
SDName: Boss Illyanna Ravenoak
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_CONCUSSIVE_SHOT     = 22914,
    SPELL_IMMOLATION_TRAP     = 22910,
    SPELL_MULTI_SHOT          = 20735,
    SPELL_VOLLEY              = 22908,
};

struct MANGOS_DLL_DECL boss_illyanna_ravenoakAI : public ScriptedAI
{
    boss_illyanna_ravenoakAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiConcussiveShotTimer;
    uint32 m_uiImmolationTrapTimer;
    uint32 m_uiMultiShotTimer;
    uint32 m_uiVolleyTimer;

    void Reset()
    {
        m_uiConcussiveShotTimer = urand(1000,3000);
        m_uiImmolationTrapTimer = urand(4000,5000);
        m_uiMultiShotTimer = urand(6000,8000);
        m_uiVolleyTimer = urand(10000,12000);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Concussive Shot
        if (m_uiConcussiveShotTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_CONCUSSIVE_SHOT);
            m_uiConcussiveShotTimer = urand(10000,15000);
        }
        else
            m_uiConcussiveShotTimer -= uiDiff;

        // Immolation Trap
        if (m_uiImmolationTrapTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMMOLATION_TRAP);
            m_uiImmolationTrapTimer = urand(4000,7000);
        }
        else
            m_uiImmolationTrapTimer -= uiDiff;

        // Multi Shot
        if (m_uiMultiShotTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_MULTI_SHOT);
            m_uiMultiShotTimer = urand(6000,8000);
        }
        else
            m_uiMultiShotTimer -= uiDiff;

        // Volley
        if (m_uiVolleyTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_VOLLEY);
            m_uiVolleyTimer = urand(25000,30000);
        }
        else
            m_uiVolleyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_illyanna_ravenoak(Creature* pCreature)
{
    return new boss_illyanna_ravenoakAI(pCreature);
}

void AddSC_boss_illyanna_ravenoak()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_illyanna_ravenoak";
    pNewscript->GetAI = &GetAI_boss_illyanna_ravenoak;
    pNewscript->RegisterSelf();
}
