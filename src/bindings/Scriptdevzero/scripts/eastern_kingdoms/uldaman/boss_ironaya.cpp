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
SDName: Boss_Ironaya
SD%Complete: 100
SDComment:
SDCategory: Uldaman
EndScriptData */

#include "precompiled.h"

#define SAY_AGGRO               -1070000

#define SPELL_ARCING_SMASH      8374
#define SPELL_KNOCK_AWAY        10101
#define SPELL_WAR_STOMP         11876


struct MANGOS_DLL_DECL boss_ironayaAI : public ScriptedAI
{
    boss_ironayaAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 ArcingSmashTimer;
    uint32 WarStompTimer;
    bool hasCastedKnockAway;

    void Reset()
    {
        ArcingSmashTimer = urand(3000,4000);
        WarStompTimer = urand(5000,7000);
        hasCastedKnockAway = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // If we are <50% hp do knockaway ONCE
        if (!hasCastedKnockAway && HealthBelowPct(50))
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);

            // current aggro target is knocked away pick new target
            Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0);

            if (!Target || Target == m_creature->getVictim())
                Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1);

            if (Target)
                m_creature->TauntApply(Target);

            // Shouldn't cast this agian
            hasCastedKnockAway = true;
        }

        // Arcing Smash spell
        if (ArcingSmashTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCING_SMASH);
            ArcingSmashTimer = urand(7000,10000);
        }
        else
            ArcingSmashTimer -= uiDiff;
        
        // War Stomp spell
        if (WarStompTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            WarStompTimer = urand(20000,25000);
        }
        else
            WarStompTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ironaya(Creature* pCreature)
{
    return new boss_ironayaAI(pCreature);
}

void AddSC_boss_ironaya()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ironaya";
    pNewScript->GetAI = &GetAI_boss_ironaya;
    pNewScript->RegisterSelf();
}
