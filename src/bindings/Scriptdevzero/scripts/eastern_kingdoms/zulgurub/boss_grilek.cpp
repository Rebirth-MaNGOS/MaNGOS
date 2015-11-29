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
SDName: Boss_Grilek
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "precompiled.h"
#include "zulgurub.h"

enum eGrilek
{
    SPELL_AVATAR            = 24646,
    SPELL_ENTANGLING_ROOTS  = 24648,
    SPELL_GROUND_TREMOR     = 6524,
};

struct MANGOS_DLL_DECL boss_grilekAI : public ScriptedAI
{
    boss_grilekAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		m_bSpawnLightning = false;		
		Reset();
	}

    uint32 m_uiAvatarTimer;
    uint32 m_uiEntanglingRootsTimer;
    uint32 m_uiGroundTremorTimer;
	bool m_bSpawnLightning;

    void Reset()
    {
        m_uiAvatarTimer = urand(15000, 25000);
        m_uiEntanglingRootsTimer = urand(15000, 25000);
        m_uiGroundTremorTimer = urand(8000, 16000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_bSpawnLightning)
		{
			m_creature->CastSpell(m_creature, SPELL_RED_LIGHTNING, true);
			m_bSpawnLightning = true;
		}
        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Avatar
        if (m_uiAvatarTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_AVATAR);
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);
            
            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(), -50);

            AttackStart(pTarget ? pTarget : m_creature->getVictim());

            m_uiAvatarTimer = urand(25000, 35000);
        }
        else
            m_uiAvatarTimer -= uiDiff;

        // Entangling Roots
        if (m_uiEntanglingRootsTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_ENTANGLING_ROOTS);
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1);

            if (m_creature->getThreatManager().getThreat(m_creature->getVictim()))
                m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-50);

            AttackStart(pTarget ? pTarget : m_creature->getVictim());

            m_uiEntanglingRootsTimer = urand(25000, 35000);
        }
        else
            m_uiEntanglingRootsTimer -= uiDiff;

        // Ground Tremor
        if (m_uiGroundTremorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_GROUND_TREMOR);
            m_uiGroundTremorTimer = urand(12000, 16000);
        }
        else
            m_uiGroundTremorTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_grilek(Creature* pCreature)
{
    return new boss_grilekAI(pCreature);
}

void AddSC_boss_grilek()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_grilek";
    pNewScript->GetAI = &GetAI_boss_grilek;
    pNewScript->RegisterSelf();
}
