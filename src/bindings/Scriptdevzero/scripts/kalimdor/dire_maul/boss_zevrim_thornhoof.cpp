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
SDName: Boss Hydrospawn
SD%Complete:
SDComment:
SDCategory: Dire Maul
EndScriptData */

#include "precompiled.h"
#include "dire_maul.h"

enum Spells
{
    SPELL_INTENSE_PAIN              = 22478,
    SPELL_SACRIFICE                 = 22651
};

struct MANGOS_DLL_DECL boss_zevrim_thornhoofAI : public ScriptedAI
{
    boss_zevrim_thornhoofAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    uint32 m_uiIntensePainTimer;
    uint32 m_uiSacrificeTimer;

    void Reset()
    {
        m_uiIntensePainTimer = urand(1000,3000);
        m_uiSacrificeTimer = urand(5000,7000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZEVRIM_THORNHOOF, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZEVRIM_THORNHOOF, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ZEVRIM_THORNHOOF, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {       
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Intense Pain
        if (m_uiIntensePainTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTENSE_PAIN);
            m_uiIntensePainTimer = urand(4000,6000);
        }
        else
            m_uiIntensePainTimer -= uiDiff;

        // Sacrifice
        if (m_uiSacrificeTimer <= uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            DoCastSpellIfCan(pTarget ? pTarget : m_creature->getVictim(), SPELL_SACRIFICE);
            m_uiSacrificeTimer = urand(6000,9000);
        }
        else
            m_uiSacrificeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_zevrim_thornhoof(Creature* pCreature)
{
    return new boss_zevrim_thornhoofAI(pCreature);
}

void AddSC_boss_zevrim_thornhoof()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_zevrim_thornhoof";
    pNewscript->GetAI = &GetAI_boss_zevrim_thornhoof;
    pNewscript->RegisterSelf();
}
