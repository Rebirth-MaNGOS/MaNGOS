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
SDName: boss_lord_argelmarch
SD%Complete: 70
SDComment: Simple aggro event is implemented
SDCategory: Blackrock Depths
EndScriptData */

#include "precompiled.h"
#include "blackrock_depths.h"

enum
{
	SPELL_CHAIN_LIGHTNING  = 15305,
	SPELL_LIGHTNING_SHIELD = 15507,
	SPELL_SHOCK            = 15605,
};

struct MANGOS_DLL_DECL boss_lord_argelmarchAI : public ScriptedAI
{
    boss_lord_argelmarchAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_depths*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_depths* m_pInstance;

    uint32 m_uiChainLightning_Timer;
	uint32 m_uiLightningShield_Timer;
	uint32 m_uiShock_Timer;

    void Reset()
    {
        m_uiChainLightning_Timer = 10000;
		m_uiLightningShield_Timer = 2000;
		m_uiShock_Timer = 5000;
    }

	void Aggro(Unit* /*pWho*/)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_MANUFACTORY, IN_PROGRESS);
	}

	void JustReachedHome()
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_MANUFACTORY, FAIL);
	}

	void JustDied(Unit*)
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_MANUFACTORY, DONE);
	}

    void UpdateAI(const uint32 uiDiff)
    {
		//Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //m_uiChainLightning_Timer
        if (m_uiChainLightning_Timer < uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHAIN_LIGHTNING);
            m_uiChainLightning_Timer = 10000;
        }
        else
            m_uiChainLightning_Timer -= uiDiff;

		//m_uiLightningShield_Timer
        if (m_uiLightningShield_Timer < uiDiff)
        {
			DoCastSpellIfCan(m_creature, SPELL_LIGHTNING_SHIELD);
			m_uiLightningShield_Timer = 12*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiLightningShield_Timer -= uiDiff;

		//m_uiShock_Timer
        if (m_uiShock_Timer < uiDiff)
        {
			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShock_Timer = 6000;
        }
        else
            m_uiShock_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_argelmarch(Creature* pCreature)
{
    return new boss_lord_argelmarchAI(pCreature);
}

void AddSC_boss_lord_argelmarch()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_lord_argelmarch";
    pNewscript->GetAI = &GetAI_boss_lord_argelmarch;
    pNewscript->RegisterSelf();
}
