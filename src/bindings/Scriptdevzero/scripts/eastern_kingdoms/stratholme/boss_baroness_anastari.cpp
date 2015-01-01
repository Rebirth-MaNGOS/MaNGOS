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
SDName: Boss_Baroness_Anastari
SD%Complete: 90
SDComment:
SDCategory: Stratholme
EndScriptData */

#include "precompiled.h"
#include "stratholme.h"

enum Spells
{
    SPELL_BANSHEE_CURSE       = 16867,
    SPELL_BANSHEE_WAIL        = 16565,
    SPELL_POSSESS             = 17244,
    SPELL_SILENCE             = 18327,
};

struct MANGOS_DLL_DECL boss_baroness_anastariAI : public ScriptedAI
{
    boss_baroness_anastariAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_stratholme*)pCreature->GetInstanceData();
        Reset();
    }

    instance_stratholme* m_pInstance;

    uint32 m_uiBansheeCurseTimer;
    uint32 m_uiBansheeWailTimer;
    uint32 m_uiPossessTimer;
    uint32 m_uiSilenceTimer;

    void Reset()
    {
        m_uiBansheeCurseTimer = urand(5000,7000);
        m_uiBansheeWailTimer = urand(0,1000);
        m_uiPossessTimer = urand(30000,35000);
        m_uiSilenceTimer = urand(9000,11000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARONESS_ANASTARI, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Banshee Curse spell
        if (m_uiBansheeCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BANSHEE_CURSE);
            m_uiBansheeCurseTimer = urand(10000,15000);
        }
        else
            m_uiBansheeCurseTimer -= uiDiff;

        // Banshee Wail spell
        if (m_uiBansheeWailTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BANSHEE_WAIL);
            m_uiBansheeWailTimer = urand(3000,5000);
        }
        else
            m_uiBansheeWailTimer -= uiDiff;

        // Possess spell
        if (m_uiPossessTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                DoCastSpellIfCan(pTarget, SPELL_POSSESS);

            m_uiPossessTimer = urand(30000,40000);
        }
        else
            m_uiPossessTimer -= uiDiff;

        // Silence spell
        if (m_uiSilenceTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SILENCE);
            m_uiSilenceTimer = urand(10000,14000);
        }
        else
            m_uiSilenceTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_baroness_anastari(Creature* pCreature)
{
    return new boss_baroness_anastariAI(pCreature);
}

void AddSC_boss_baroness_anastari()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_baroness_anastari";
    pNewscript->GetAI = &GetAI_boss_baroness_anastari;
    pNewscript->RegisterSelf();
}
