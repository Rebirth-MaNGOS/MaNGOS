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
SDName: Boss Rattlegore
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "precompiled.h"
#include "scholomance.h"

enum Spells
{
    SPELL_KNOCK_AWAY        = 10101,
    SPELL_STRIKE            = 18368,
    SPELL_WAR_STOMP         = 16727,
};

struct MANGOS_DLL_DECL boss_rattlegoreAI : public ScriptedAI
{
    boss_rattlegoreAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    //uint32 m_uiCallTimer;
    uint32 m_uiKnockAwayTimer;
    uint32 m_uiStrikeTimer;
    uint32 m_uiWarStompTimer;

    void Reset()
    {
        //m_uiCallTimer = 0;
        m_uiKnockAwayTimer = urand(6000,8000);
        m_uiStrikeTimer = urand(2000,4000);
        m_uiWarStompTimer = urand(12000,14000);
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_KNOCK_AWAY && pTarget->GetTypeId() == TYPEID_PLAYER)
			m_creature->getThreatManager().modifyThreatPercent(pTarget, -40);				//added threat reduction
	}
		
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Knock Away
        if (m_uiKnockAwayTimer < uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_KNOCK_AWAY);		
            m_uiKnockAwayTimer = urand(7000,9000);
        }
        else
            m_uiKnockAwayTimer -= uiDiff;

        // Strike
        if (m_uiStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(3000,6000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        // War Stomp
        if (m_uiWarStompTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(12000,16000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        //// Call Construct
        //if (HealthBelowPct(33))
        //{
        //    if (m_uiCallTimer < uiDiff)
        //    {
        //        if (Creature* pConstruct = GetClosestCreatureWithEntry(m_creature, NPC_RISEN_CONSTRUCT, 100.0f))
        //            pConstruct->AI()->AttackStart(m_creature->getVictim());
        //        m_uiCallTimer = 30000;
        //    }
        //    else
        //        m_uiCallTimer -= uiDiff;
        //}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rattlegore(Creature* pCreature)
{
    return new boss_rattlegoreAI(pCreature);
}

void AddSC_boss_rattlegore()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_rattlegore";
    pNewscript->GetAI = &GetAI_boss_rattlegore;
    pNewscript->RegisterSelf();
}
