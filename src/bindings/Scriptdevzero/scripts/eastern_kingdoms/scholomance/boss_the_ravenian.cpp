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
SDName: Boss_the_ravenian
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CLEAVE            = 20691,
    SPELL_KNOCK_AWAY        = 18670,
    SPELL_SUNDERING_CLEAVE  = 17963,
    SPELL_TRAMPLE           = 15550
};

struct MANGOS_DLL_DECL boss_the_ravenianAI : public ScriptedAI
{
    boss_the_ravenianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiSunderingCleaveTimer;
    uint32 m_uiTrampleTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(4000,6000);
        m_uiKnockAwayTimer = urand(14000,18000);
        m_uiSunderingCleaveTimer = urand(8000,12000);
        m_uiTrampleTimer = urand(20000,24000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_RAVENIAN, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_RAVENIAN, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_THE_RAVENIAN, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(3000,7000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Knock Away
        if (m_uiKnockAwayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(9000,11000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Sundering Cleave
        if (m_uiSunderingCleaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDERING_CLEAVE);
            m_uiSunderingCleaveTimer = urand(12000,14000);
        }
        else
            m_uiSunderingCleaveTimer -= uiDiff;

        // Trample
        if (m_uiTrampleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRAMPLE);
            m_uiTrampleTimer = urand(10000,15000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_the_ravenian(Creature* pCreature)
{
    return new boss_the_ravenianAI(pCreature);
}

void AddSC_boss_the_ravenian()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_the_ravenian";
    pNewscript->GetAI = &GetAI_boss_the_ravenian;
    pNewscript->RegisterSelf();
}
