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
SDName: Boss_Doctor_Theolen_Krastinov
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum
{
    EMOTE_GENERIC_FRENZY_KILL   = -1000001,

    SPELL_BACKHAND              = 18103,
    SPELL_FRENZY                = 8269,
    SPELL_REND                  = 16509
};

struct MANGOS_DLL_DECL boss_doctor_theolen_krastinovAI : public ScriptedAI
{
    boss_doctor_theolen_krastinovAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiBackhandTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiRendTimer;

    void Reset()
    {
        m_uiBackhandTimer = urand(5000,6000);
        m_uiFrenzyTimer = 1000;
        m_uiRendTimer = urand(9000,11000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DOCTOR_THEOLEN_KRASTINOV, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DOCTOR_THEOLEN_KRASTINOV, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DOCTOR_THEOLEN_KRASTINOV, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Backhand
        if (m_uiBackhandTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BACKHAND);
            m_uiBackhandTimer = urand(5000,7000);
        }
        else
            m_uiBackhandTimer -= uiDiff;

        // Frenzy
        if (HealthBelowPct(33))
        {
            if (m_uiFrenzyTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FRENZY) == CAST_OK)
                {
                    DoScriptText(EMOTE_GENERIC_FRENZY_KILL, m_creature);
                    m_uiFrenzyTimer = 120000;
                }
            }
            else
                m_uiFrenzyTimer -= uiDiff;
        }

        // Rend
        if (m_uiRendTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);
            m_uiRendTimer = urand(8000,10000);
        }
        else
            m_uiRendTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_doctor_theolen_krastinov(Creature* pCreature)
{
    return new boss_doctor_theolen_krastinovAI(pCreature);
}

void AddSC_boss_doctor_theolen_krastinov()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_doctor_theolen_krastinov";
    pNewscript->GetAI = &GetAI_boss_doctor_theolen_krastinov;
    pNewscript->RegisterSelf();
}
