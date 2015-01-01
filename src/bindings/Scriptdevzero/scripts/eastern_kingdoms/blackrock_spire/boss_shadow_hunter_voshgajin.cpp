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
SDName: Boss_Shadow_Hunter_Voshgajin
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"

enum Spells
{
    SPELL_CLEAVE              = 20691,
    SPELL_CURSE_OF_BLOOD      = 16098,
    SPELL_HEX                 = 16097
};

struct MANGOS_DLL_DECL boss_shadow_hunter_voshgajinAI : public ScriptedAI
{
    boss_shadow_hunter_voshgajinAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCurseOfBloodTimer;
    uint32 m_uiHexTimer;
    uint32 m_uiCleaveTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(10000,12000);
        m_uiCurseOfBloodTimer = urand(1000,3000);
        m_uiHexTimer = urand(6000,7000);        
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = urand(4000,6000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Curse Of Blood
        if (m_uiCurseOfBloodTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_BLOOD);
            m_uiCurseOfBloodTimer = urand(30000,40000);
        }
        else
            m_uiCurseOfBloodTimer -= uiDiff;

        // Hex
        //if (m_uiHexTimer <= uiDiff)
        //{
        //    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
        //        DoCastSpellIfCan(pTarget, SPELL_HEX);

        //    m_uiHexTimer = urand(10000,15000);
        //}
        //else
        //    m_uiHexTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_shadow_hunter_voshgajin(Creature* pCreature)
{
    return new boss_shadow_hunter_voshgajinAI(pCreature);
}

void AddSC_boss_shadow_hunter_voshgajin()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_shadow_hunter_voshgajin";
    pNewscript->GetAI = &GetAI_boss_shadow_hunter_voshgajin;
    pNewscript->RegisterSelf();
}
