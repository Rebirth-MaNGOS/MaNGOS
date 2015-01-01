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
SDName: Boss_Gahz'ranka
SD%Complete: 85
SDComment: Massive Geyser with knockback not working. Spell buggy.
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"

enum eGahzranka
{
    SPELL_FROST_BREATH    = 16099,
    SPELL_MASSIVE_GEYSER  = 22421,
    SPELL_SLAM            = 24326,
    SPELL_TRASH           = 3391,
};

struct MANGOS_DLL_DECL boss_gahzrankaAI : public ScriptedAI
{
    boss_gahzrankaAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiFrostbreathTimer;
    uint32 m_uiMassiveGeyserTimer;
    uint32 m_uiSlamTimer;
    uint32 m_uiTrashTimer;

    void Reset()
    {
        m_uiFrostbreathTimer = 8000;
        m_uiMassiveGeyserTimer = 25000;
        m_uiSlamTimer = 17000;
        m_uiTrashTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frost Breath
        if (m_uiFrostbreathTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROST_BREATH);
            m_uiFrostbreathTimer = urand(7000, 11000);
        }
        else
            m_uiFrostbreathTimer -= uiDiff;

        // Massive Geyser
        if (m_uiMassiveGeyserTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MASSIVE_GEYSER);
            DoResetThreat();

            m_uiMassiveGeyserTimer = urand(22000, 32000);
        }
        else
            m_uiMassiveGeyserTimer -= uiDiff;

        // Slam
        if (m_uiSlamTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLAM);
            m_uiSlamTimer = urand(12000, 20000);
        }
        else
            m_uiSlamTimer -= uiDiff;

        // Trash
        if (m_uiTrashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRASH);
            m_uiTrashTimer = urand(5000, 10000);
        }
        else
            m_uiTrashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gahzranka(Creature* pCreature)
{
    return new boss_gahzrankaAI(pCreature);
}

void AddSC_boss_gahzranka()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_gahzranka";
    pNewScript->GetAI = &GetAI_boss_gahzranka;
    pNewScript->RegisterSelf();
}
