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
SDName: Boss_Ramstein_the_Gorger
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_KNOCKOUT        = 17307,
    SPELL_TRAMPLE         = 5568
};

struct MANGOS_DLL_DECL boss_ramstein_the_gorgerAI : public ScriptedAI
{
    boss_ramstein_the_gorgerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiTrampleTimer;
    uint32 m_uiKnockoutTimer;

    void Reset()
    {
        m_uiTrampleTimer = urand(3000,4000);
        m_uiKnockoutTimer = urand(8000,12000);
    }

    void JustDied(Unit* pKiller)
    {
        for(uint8 i = 0; i < 30; ++i)
        {
            if (Creature* pMob = m_creature->SummonCreature(NPC_MINDLESS_UNDEAD, 3969.35f+irand(-10,10), -3391.87f+irand(-10,10), 119.11f, 5.91f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1800000))
                pMob->AI()->AttackStart(pKiller);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLAUGHTER_SQUARE, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knockout spell
        if (m_uiKnockoutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCKOUT);
            m_uiKnockoutTimer = urand(8000,12000);
        }
        else
            m_uiKnockoutTimer -= uiDiff;

        // Trample spell
        if (m_uiTrampleTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_TRAMPLE);
            m_uiTrampleTimer = urand(6000,8000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ramstein_the_gorger(Creature* pCreature)
{
    return new boss_ramstein_the_gorgerAI(pCreature);
}

void AddSC_boss_ramstein_the_gorger()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ramstein_the_gorger";
    pNewscript->GetAI = &GetAI_boss_ramstein_the_gorger;
    pNewscript->RegisterSelf();
}
