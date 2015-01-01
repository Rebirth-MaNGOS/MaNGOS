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
SDName: Boss_Mother_Smolderweb
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum Spells
{
    SPELL_CRYSTALLIZE               = 16104,
    SPELL_MOTHERS_MILK              = 16468,
    SPELL_SUMMON_SPIRE_SPIDERLING   = 16103
};

struct MANGOS_DLL_DECL boss_mother_smolderwebAI : public ScriptedAI
{
    boss_mother_smolderwebAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCrystallizeTimer;
    uint32 m_uiMothersMilkTimer;

    void Reset()
    {
        m_uiCrystallizeTimer = urand(5000,10000);
        m_uiMothersMilkTimer = urand(10000,15000);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        for(uint32 i = 0; i < 4; ++i)
            DoSpawnCreature(NPC_SPIRE_SPIDERLING, irand(-5,5), irand(-5,5), 0, 0, TEMPSUMMON_DEAD_DESPAWN, 5000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Crystallize
        if (m_uiCrystallizeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRYSTALLIZE);
            m_uiCrystallizeTimer = urand(10000,15000);
        }
        else
            m_uiCrystallizeTimer -= uiDiff;

        // Mothers Milk
        if (m_uiMothersMilkTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOTHERS_MILK);
            m_uiMothersMilkTimer = urand(5000,15000);
        }
        else
            m_uiMothersMilkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_mother_smolderweb(Creature* pCreature)
{
    return new boss_mother_smolderwebAI(pCreature);
}

void AddSC_boss_mother_smolderweb()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_mother_smolderweb";
    pNewscript->GetAI = &GetAI_boss_mother_smolderweb;
    pNewscript->RegisterSelf();
}
