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
SDName: Boss_Magmus
SD%Complete: 100
SDComment:
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum
{
    // Magmus spells
    SPELL_FIERYBURST        = 15668, // 13900
    SPELL_WARSTOMP          = 15593,  // 24375

    // Ironhand Guard
    SPELL_GOUT_OF_FLAME     = 15529,
};

struct MANGOS_DLL_DECL boss_magmusAI : public ScriptedAI
{
    boss_magmusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiFieryBurst_Timer;
    uint32 m_uiWarStomp_Timer;

    void Reset()
    {
        m_uiFieryBurst_Timer = 5000;
        m_uiWarStomp_Timer = 0;
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IRON_HALL, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IRON_HALL, FAIL);
    }

    void JustDied(Unit* /*pVictim*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IRON_HALL, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fiery Burst
        if (m_uiFieryBurst_Timer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIERYBURST);
            m_uiFieryBurst_Timer = 6000;
        }
        else
            m_uiFieryBurst_Timer -= uiDiff;

        // War Stomp
		if (HealthBelowPct(51))
        {
            if (m_uiWarStomp_Timer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_WARSTOMP);
                m_uiWarStomp_Timer = 8000;
            }
            else
                m_uiWarStomp_Timer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_magmus(Creature* pCreature)
{
    return new boss_magmusAI(pCreature);
}

/*######
## mob_ironhand_guard
######*/

struct MANGOS_DLL_DECL mob_ironhand_guardAI : public ScriptedAI
{
    mob_ironhand_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
		Reset();
	}

	instance_blackrock_depths* m_pInstance;
    uint32 m_uiGoutOfFlame_Timer;

    void Reset()
    {
        m_uiGoutOfFlame_Timer = urand(4000, 8000);

		m_creature->addUnitState(UNIT_STAT_CAN_NOT_REACT);
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_ROTATE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		// When Magmus's Encounter is in progress, Statues will breath Flames
		if (m_pInstance && m_pInstance->GetData(TYPE_IRON_HALL) == IN_PROGRESS)
        {
            if (m_uiGoutOfFlame_Timer <= uiDiff)
            {
			    DoCastSpellIfCan(m_creature, SPELL_GOUT_OF_FLAME, CAST_FORCE_TARGET_SELF);
                m_uiGoutOfFlame_Timer = urand(13000, 18000);
            }
            else
                m_uiGoutOfFlame_Timer -= uiDiff;
        }
        else if (m_creature->IsNonMeleeSpellCasted(false))
        {
            // Interrupt already casted Flames
            m_creature->InterruptNonMeleeSpells(false);
        }
    }
};

CreatureAI* GetAI_mob_ironhand_guard(Creature* pCreature)
{
    return new mob_ironhand_guardAI(pCreature);
}

void AddSC_boss_magmus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_magmus";
    pNewScript->GetAI = &GetAI_boss_magmus;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_ironhand_guard";
    pNewScript->GetAI = &GetAI_mob_ironhand_guard;
    pNewScript->RegisterSelf();
}
