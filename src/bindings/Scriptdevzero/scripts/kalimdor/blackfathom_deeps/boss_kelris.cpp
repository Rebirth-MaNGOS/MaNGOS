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
SDName: Boss Kelris
SD%Complete: 100%
SDComment: 
SDCategory: Blackfathom Deeps
EndScriptData */

#include "precompiled.h"
#include "blackfathom_deeps.h"

enum
{
    SAY_AGGRO               = -1048002,
    SAY_SLEEP               = -1048001,
    SAY_DEATH               = -1048000,

    SPELL_MIND_BLAST        = 15587,
    SPELL_SLEEP             = 8399
};

struct MANGOS_DLL_DECL boss_kelrisAI : public ScriptedAI
{
    boss_kelrisAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackfathom_deeps*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackfathom_deeps *m_pInstance;

    uint32 uiMindBlastTimer;
    uint32 uiSleepTimer;

    void Reset()
    {
        uiMindBlastTimer = urand(2000,5000);
        uiSleepTimer = urand(9000,12000);
    }

    void JustReachedHome()
    { 
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELRIS, NOT_STARTED);
    }

    void Aggro(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, m_creature); 
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELRIS, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_DEATH, m_creature); 
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELRIS, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiMindBlastTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
            uiMindBlastTimer = urand(6000,8000);
        } else uiMindBlastTimer -= uiDiff;

        if (uiSleepTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoScriptText(SAY_SLEEP, m_creature); 
                DoCastSpellIfCan(pTarget, SPELL_SLEEP);
            }
            uiSleepTimer = urand(10000,15000);
        } else uiSleepTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelris(Creature* pCreature)
{
    return new boss_kelrisAI (pCreature);
}

void AddSC_boss_kelris()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_kelris";
    pNewscript->GetAI = &GetAI_boss_kelris;
    pNewscript->RegisterSelf();
}
