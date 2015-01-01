/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* ScriptData
SDName: Boss Tendris Warpwood
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum eTendris
{
    SPELL_ENTANGLE          = 22994,
    SPELL_GRASPING_VINES    = 22924,
    SPELL_TRAMPLE           = 5568,
    SPELL_UPPERCUT          = 22916,

    SAY_AGGRO               = -1429014,
};

struct MANGOS_DLL_DECL boss_tendris_warpwoodAI : public ScriptedAI
{
    boss_tendris_warpwoodAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiEntangleTimer;
    uint32 m_uiGraspingVinesTimer;
    uint32 m_uiTrampleTimer;
    uint32 m_uiUppercutTimer;

    void Reset()
    {
        m_uiEntangleTimer = urand(6000,8000);
        m_uiGraspingVinesTimer = urand(5000,7000);
        m_uiTrampleTimer = urand(3000,5000);
        m_uiUppercutTimer = urand(2000,4000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TENDRIS_WARPWOOD, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TENDRIS_WARPWOOD, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TENDRIS_WARPWOOD, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Entangle
        if (m_uiEntangleTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_ENTANGLE);
            m_uiEntangleTimer = urand(5000,8000);
        }
        else
            m_uiEntangleTimer -= uiDiff;

        // Grasping Vines
        if (m_uiGraspingVinesTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GRASPING_VINES);
            m_uiGraspingVinesTimer = urand(8000,10000);
        }
        else
            m_uiGraspingVinesTimer -= uiDiff;

        // Trample
        if (m_uiTrampleTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_TRAMPLE);
            m_uiTrampleTimer = urand(7000,9000);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        // Uppercut
        if (m_uiUppercutTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_UPPERCUT);
            m_uiUppercutTimer = urand(6000,8000);
        }
        else
            m_uiUppercutTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_tendris_warpwood(Creature* pCreature)
{
    return new boss_tendris_warpwoodAI(pCreature);
}

void AddSC_boss_tendris_warpwood()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_tendris_warpwood";
    pNewscript->GetAI = &GetAI_boss_tendris_warpwood;
    pNewscript->RegisterSelf();
}
