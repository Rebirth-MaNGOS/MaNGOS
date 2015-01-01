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
SDName: Boss_instructormalicia
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CALL_OF_GRAVES        = 17831,
    SPELL_CORRUPTION            = 18376,
    SPELL_FLASH_HEAL            = 17843,
    SPELL_HEAL                  = 15586,
    SPELL_RENEW                 = 8362,
    SPELL_SLOW                  = 13747
};

struct MANGOS_DLL_DECL boss_instructor_maliciaAI : public ScriptedAI
{
    boss_instructor_maliciaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiCallOfGravesTimer;
    uint32 m_uiCorruptionTimer;
    uint32 m_uiFlashHealTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiRenewTimer;
    uint32 m_uiSlowTimer;
    uint32 m_uiFlashCounter;
    uint32 m_uiTouchCounter;

    void Reset()
    {
        m_uiCallOfGravesTimer = 4000;
        m_uiCorruptionTimer = 8000;
        m_uiFlashHealTimer = 35000;
        m_uiHealTimer = 45000;
        m_uiRenewTimer = 15000;
        m_uiSlowTimer = 25000;
        m_uiFlashCounter = 0;
        m_uiTouchCounter = 0;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INSTRUCTOR_MALICIA, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INSTRUCTOR_MALICIA, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INSTRUCTOR_MALICIA, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Call Of Graves
        if (m_uiCallOfGravesTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CALL_OF_GRAVES);
            m_uiCallOfGravesTimer = urand(65000,80000);
        }
        else
            m_uiCallOfGravesTimer -= uiDiff;

        // Corruption
        if (m_uiCorruptionTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_CORRUPTION);

            m_uiCorruptionTimer = urand(8000,15000);
        }
        else
            m_uiCorruptionTimer -= uiDiff;

        // Flash Heal
        if (m_uiFlashHealTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_FLASH_HEAL);

            // 3 Flash heals will be casted
            if (m_uiFlashCounter < 2)
            {
                m_uiFlashHealTimer = 5000;
                ++m_uiFlashCounter;
            }
            else
            {
                m_uiFlashCounter = 0;
                m_uiFlashHealTimer = 30000;
            }
        }
        else
            m_uiFlashHealTimer -= uiDiff;

        // Heal
        if (m_uiHealTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_HEAL);

            // 3 Heal will be casted
            if (m_uiTouchCounter < 2)
            {
                m_uiHealTimer = 5500;
                ++m_uiTouchCounter;
            }
            else
            {
                m_uiTouchCounter = 0;
                m_uiHealTimer = 30000;
            }
        }
        else
            m_uiHealTimer -= uiDiff;

        // Slow
        if (m_uiSlowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SLOW);
            m_uiSlowTimer = urand(10000,15000);
        }
        else
            m_uiSlowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_instructor_malicia(Creature* pCreature)
{
    return new boss_instructor_maliciaAI(pCreature);
}

void AddSC_boss_instructor_malicia()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_instructor_malicia";
    pNewscript->GetAI = &GetAI_boss_instructor_malicia;
    pNewscript->RegisterSelf();
}
