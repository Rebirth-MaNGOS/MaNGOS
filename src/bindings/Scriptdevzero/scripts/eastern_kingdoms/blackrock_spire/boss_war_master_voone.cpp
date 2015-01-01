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
SDName: Boss_Warmaster_Voone
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"

enum Spells
{
    SPELL_CLEAVE            = 15284,
    SPELL_MORTAL_STRIKE     = 15708,
    SPELL_PUMMEL            = 15615,
    SPELL_SNAP_KICK         = 15618,
    SPELL_THRASH            = 3391,
    SPELL_THROW_AXE         = 16075,
    SPELL_UPPERCUT          = 10966
};

struct MANGOS_DLL_DECL boss_war_master_vooneAI : public ScriptedAI
{
    boss_war_master_vooneAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCleaveTimer;
    uint32 m_uiMortalStrikeTimer;
    uint32 m_uiPummelTimer;
    uint32 m_uiSnapkickTimer;
    uint32 m_uiThrashTimer;
    uint32 m_uiThrowAxeTimer;
    uint32 m_uiUppercutTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(10000,14000);
        m_uiMortalStrikeTimer = urand(12000,16000);
        m_uiPummelTimer = urand(25000,30000);
        m_uiSnapkickTimer = urand(4000,7000);
        m_uiThrashTimer = urand(5000,8000);
        m_uiThrowAxeTimer = urand(500,1000);
        m_uiUppercutTimer = urand(20000,24000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(5000,9000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Mortal Strike
        if (m_uiMortalStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
            m_uiMortalStrikeTimer = urand(6000,10000);
        }
        else
            m_uiMortalStrikeTimer -= uiDiff;

        // Pummel
        if (m_uiPummelTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PUMMEL);
            m_uiPummelTimer = urand(12000,14000);
        }
        else
            m_uiPummelTimer -= uiDiff;

        // Snap Kick
        if (m_uiSnapkickTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SNAP_KICK);
            m_uiSnapkickTimer = urand(4000,6000);
        }
        else
            m_uiSnapkickTimer -= uiDiff;

        // Trash
        if (m_uiThrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THRASH);
            m_uiThrashTimer = urand(4000,6000);
        }
        else
            m_uiThrashTimer -= uiDiff;

        // Throw Axe
        if (m_uiThrowAxeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_THROW_AXE);
            m_uiThrowAxeTimer = urand(8000,10000);
        }
        else
            m_uiThrowAxeTimer -= uiDiff;

        // Uppercut
        if (m_uiUppercutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_UPPERCUT);
            m_uiUppercutTimer = urand(8000,12000);
        }
        else
            m_uiUppercutTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_war_master_voone(Creature* pCreature)
{
    return new boss_war_master_vooneAI(pCreature);
}

void AddSC_boss_war_master_voone()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_war_master_voone";
    pNewscript->GetAI = &GetAI_boss_war_master_voone;
    pNewscript->RegisterSelf();
}
