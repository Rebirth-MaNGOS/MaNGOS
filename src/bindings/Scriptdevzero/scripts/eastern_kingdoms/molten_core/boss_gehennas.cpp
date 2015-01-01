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
SDName: Boss_Gehennas
SD%Complete: 100
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eGehennas
{
    SPELL_GEHENNAS_CURSE        = 19716,
    SPELL_RAIN_OF_FIRE          = 19717,
    SPELL_SHADOWBOLT            = 19728,
};

struct MANGOS_DLL_DECL boss_gehennasAI : public ScriptedAI
{
    boss_gehennasAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiGehennasCurseTimer;
    uint32 m_uiRainOfFireTimer;
    uint32 m_uiShadowBoltTimer;

    void Reset()
    {
        m_uiGehennasCurseTimer = 12000;
        m_uiRainOfFireTimer = 10000;
        m_uiShadowBoltTimer = 6000;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GEHENNAS, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GEHENNAS, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Gehennas Curse
        if (m_uiGehennasCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GEHENNAS_CURSE);
            m_uiGehennasCurseTimer = urand(22000, 30000);
        }
        else
            m_uiGehennasCurseTimer -= uiDiff;

        // Rain of Fire
        if (m_uiRainOfFireTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCastSpellIfCan(pTarget, SPELL_RAIN_OF_FIRE);

            m_uiRainOfFireTimer = urand(4000, 12000);
        }
        else
            m_uiRainOfFireTimer -= uiDiff;

        // Shadow Bolt
        if (m_uiShadowBoltTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                DoCastSpellIfCan(pTarget, SPELL_SHADOWBOLT);
            m_uiShadowBoltTimer = 7000;
        }
        else
            m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gehennas(Creature* pCreature)
{
    return new boss_gehennasAI(pCreature);
}

void AddSC_boss_gehennas()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_gehennas";
    pNewscript->GetAI = &GetAI_boss_gehennas;
    pNewscript->RegisterSelf();
}
