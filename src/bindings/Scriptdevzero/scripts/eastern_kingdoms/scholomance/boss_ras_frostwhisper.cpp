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
SDName: Boss_Ras_Frostwhisper
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_CHILL_NOVA        = 18099,
    SPELL_FEAR              = 12096,
    SPELL_FREEZE            = 18763,
    SPELL_FROST_ARMOR       = 18100,
    SPELL_FROSTBOLT_VOLLEY  = 8398,
    SPELL_KNOCK_AWAY        = 11130
};

struct MANGOS_DLL_DECL boss_ras_frostwhisperAI : public ScriptedAI
{
    boss_ras_frostwhisperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_scholomance*)pCreature->GetInstanceData();
        Reset();
    }

    instance_scholomance* m_pInstance;

    uint32 m_uiChillNovaTimer;
    uint32 m_uiFearTimer;
    uint32 m_uiFreezeTimer;
    uint32 m_uiFrostboltVolleyTimer;
    uint32 m_uiKnockAwayTimer;

    void Reset()
    {
        m_uiChillNovaTimer = urand(3000,5000);
        m_uiFearTimer = urand(25000,30000);
        m_uiFreezeTimer = urand(12000,15000);
        m_uiFrostboltVolleyTimer = urand(18000,22000);
        m_uiKnockAwayTimer = urand(8000,10000);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAS_FROSTWHISPER, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAS_FROSTWHISPER, IN_PROGRESS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAS_FROSTWHISPER, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoCastSpellIfCan(m_creature, SPELL_FROST_ARMOR, CAST_AURA_NOT_PRESENT);

        // Chill Nova
        if (m_uiChillNovaTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHILL_NOVA);
            m_uiChillNovaTimer = urand(12000,17000);
        }
        else
            m_uiChillNovaTimer -= uiDiff;

        // Fear
        if (m_uiFearTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_FEAR);
            m_uiFearTimer = 25000;
        }
        else
            m_uiFearTimer -= uiDiff;

        // Freeze
        if (m_uiFreezeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FREEZE);
            m_uiFreezeTimer = urand(30000,40000);
        }
        else
            m_uiFreezeTimer -= uiDiff;

        // Frostbolt Volley
        if (m_uiFrostboltVolleyTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FROSTBOLT_VOLLEY);
            m_uiFrostboltVolleyTimer = urand(9000,12000);
        }
        else
            m_uiFrostboltVolleyTimer -= uiDiff;

        // Knock Away
        if (m_uiKnockAwayTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(6000,8000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ras_frostwhisper(Creature* pCreature)
{
    return new boss_ras_frostwhisperAI(pCreature);
}

void AddSC_boss_ras_frostwhisper()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_ras_frostwhisper";
    pNewscript->GetAI = &GetAI_boss_ras_frostwhisper;
    pNewscript->RegisterSelf();
}
