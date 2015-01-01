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

#include "precompiled.h"
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_NET         = 6533
};

struct MANGOS_DLL_DECL boss_gelihastAI : public ScriptedAI
{
    boss_gelihastAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackfathom_deeps*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackfathom_deeps *m_pInstance;

    uint32 uiNetTimer;

    void Reset()
    {
        uiNetTimer = urand(2000,4000);
    }

    void JustReachedHome()
    { 
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GELIHAST, NOT_STARTED);
    }

    void Aggro(Unit* /*who*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GELIHAST, IN_PROGRESS);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GELIHAST, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (uiNetTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_NET);
            uiNetTimer = urand(4000,7000);
        } else uiNetTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gelihast(Creature* pCreature)
{
    return new boss_gelihastAI (pCreature);
}

void AddSC_boss_gelihast()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_gelihast";
    pNewscript->GetAI = &GetAI_boss_gelihast;
    pNewscript->RegisterSelf();
}
