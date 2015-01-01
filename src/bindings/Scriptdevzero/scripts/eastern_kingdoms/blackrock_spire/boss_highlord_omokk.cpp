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
SDName: Boss_Highlord_Omokk
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"

enum Spells
{
    SPELL_CLEAVE            = 15284,
    SPELL_FRENZY            = 8269,
    SPELL_KNOCK_AWAY        = 10101
};

struct MANGOS_DLL_DECL boss_highlord_omokkAI : public ScriptedAI
{
    boss_highlord_omokkAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCleaveTimer;
    uint32 m_uiKnockAwayTimer;

    void Reset()
    {        
        m_uiCleaveTimer = urand(6000,8000);
        m_uiKnockAwayTimer = urand(2000,4000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (HealthBelowPct(50))
            DoCastSpellIfCan(m_creature, SPELL_FRENZY, CAST_AURA_NOT_PRESENT);

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(6000,6000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Knock Away
        if (m_uiKnockAwayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(7000,9000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_highlord_omokk(Creature* pCreature)
{
    return new boss_highlord_omokkAI(pCreature);
}

void AddSC_boss_highlord_omokk()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_highlord_omokk";
    pNewscript->GetAI = &GetAI_boss_highlord_omokk;
    pNewscript->RegisterSelf();
}
