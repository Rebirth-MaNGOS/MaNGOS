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
SDName: Boss_Gyth
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eGyth
{
    SPELL_CHROMATIC_CHAOS         = 16337,
    SPELL_CORROSIVE_ACID_BREATH   = 16359,
    SPELL_FLAME_BREATH            = 16390,
    SPELL_FREEZE                  = 16350,
    SPELL_KNOCK_AWAY              = 10101,

    MODEL_GYTH                    = 9806,
    MODEL_GYTH_REND               = 9723,
};

struct MANGOS_DLL_DECL boss_gythAI : public ScriptedAI
{
    boss_gythAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    bool m_bRendSummoned;
    uint32 m_uiCorrosiveAcidBreathTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiFreezeTimer;
    uint32 m_uiKnockAwayTimer;

    void Reset()
    {
        m_bRendSummoned = false;
        m_uiCorrosiveAcidBreathTimer = urand(8000,10000);
        m_uiFlameBreathTimer = urand(0,3000);
        m_uiFreezeTimer = urand(12000,14000);
        m_uiKnockAwayTimer = urand(5000,7000);

        m_creature->SetDisplayId(MODEL_GYTH_REND);
    }

    void JustReachedHome()
    {
        m_creature->RemoveFromWorld();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_REND_BLACKHAND, NOT_STARTED);
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoCastSpellIfCan(m_creature, SPELL_CHROMATIC_CHAOS);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        // Finish event if gyth somehow died without summoning Warchief
        if (m_pInstance && m_creature->GetDisplayId() == MODEL_GYTH_REND)
            m_pInstance->SetData(TYPE_REND_BLACKHAND, DONE);
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_KNOCK_AWAY && pTarget->GetTypeId() == TYPEID_PLAYER)
			m_creature->getThreatManager().modifyThreatPercent(pTarget, -40);				//added threat reduction
	}

    void UpdateAI(const uint32 uiDiff)
    {
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Summon Warchief Rend Blackhand
        if (!m_bRendSummoned && HealthBelowPct(16) && m_creature->GetHealth() > 0)
        {
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->SetDisplayId(MODEL_GYTH);
            m_creature->SummonCreature(NPC_WARCHIEF_REND_BLACKHAND, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_DEAD_DESPAWN, 120000);
            m_creature->GenericTextEmote("Warchief Rend Blackhand is knocked off his drake!", NULL, false);
			m_bRendSummoned = true;
        }

        // Corrosive Acid Breath
        if (m_uiCorrosiveAcidBreathTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORROSIVE_ACID_BREATH);
            m_uiCorrosiveAcidBreathTimer = urand(6000,10000);
        }
        else
            m_uiCorrosiveAcidBreathTimer -= uiDiff;

        // Flame Breath 
        if (m_uiFlameBreathTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_BREATH);
            m_uiFlameBreathTimer = urand(4000,8000);
        }
        else
            m_uiFlameBreathTimer -= uiDiff;

        // Freeze
        if (m_uiFreezeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FREEZE);
            m_uiFreezeTimer = urand(10000,15000);
        }
        else
            m_uiFreezeTimer -= uiDiff;

        // Knock Away
        if (m_uiKnockAwayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);
            m_uiKnockAwayTimer = urand(6000,10000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gyth(Creature* pCreature)
{
    return new boss_gythAI(pCreature);
}

void AddSC_boss_gyth()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_gyth";
    pNewscript->GetAI = &GetAI_boss_gyth;
    pNewscript->RegisterSelf();
}
