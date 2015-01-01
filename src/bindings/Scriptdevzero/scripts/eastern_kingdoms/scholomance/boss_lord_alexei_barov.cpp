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
SDName: Boss_Lord_Alexei_Barov
SD%Complete: 100
SDComment: aura applied/defined in database
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_IMMOLATE              = 15570,
    SPELL_UNHOLY_AURA           = 17467,
    SPELL_VEIL_OF_SHADOW        = 17820
};

struct MANGOS_DLL_DECL boss_lord_alexei_barovAI : public ScriptedAI
{
    boss_lord_alexei_barovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiImmolateTimer;
    uint32 m_uiVeilofShadowTimer;

    void Reset()
    {
        m_uiImmolateTimer = urand(4000,6000);
        m_uiVeilofShadowTimer = urand(10000,15000);

        m_creature->LoadCreatureAddon();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_ALEXEI_BAROV, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_ALEXEI_BAROV, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LORD_ALEXEI_BAROV, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoCastSpellIfCan(m_creature, SPELL_UNHOLY_AURA, CAST_AURA_NOT_PRESENT);

        // Immolate
        if (m_uiImmolateTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_IMMOLATE);
            m_uiImmolateTimer = urand(8000,10000);
        }
        else
            m_uiImmolateTimer -= uiDiff;

        // Veil of Shadow
        if (m_uiVeilofShadowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VEIL_OF_SHADOW);
            m_uiVeilofShadowTimer = urand(15000,20000);
        }
        else
            m_uiVeilofShadowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_lord_alexei_barov(Creature* pCreature)
{
    return new boss_lord_alexei_barovAI(pCreature);
}

void AddSC_boss_lord_alexei_barov()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_lord_alexei_barov";
    pNewscript->GetAI = &GetAI_boss_lord_alexei_barov;
    pNewscript->RegisterSelf();
}
