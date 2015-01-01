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
SDName: Boss_Lucifron
SD%Complete: 90
SDComment:
SDCategory: Molten Core
EndScriptData */

#include "precompiled.h"
#include "molten_core.h"

enum eLucifron
{
    // Lucifron
    SPELL_IMPENDING_DOOM    = 19702,
    SPELL_LUCIFRON_CURSE    = 19703,
    SPELL_SHADOW_SHOCK      = 20603,

    // Flamewaker Protector
    SPELL_DOMINATE_MIND     = 20604,
    SPELL_CLEAVE            = 20605,
};

struct MANGOS_DLL_DECL boss_lucifronAI : public ScriptedAI
{
    boss_lucifronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiImpendingDoomTimer;
    uint32 m_uiLucifronCurseTimer;
    uint32 m_uiShadowShockTimer;

    void Reset()
    {
        m_creature->SetBoundingValue(0, 3);
        m_creature->SetBoundingValue(1, 2);

        m_uiImpendingDoomTimer = 10000;                         // Initial cast after 10 seconds so the debuffs alternate
        m_uiLucifronCurseTimer = 20000;                         // Initial cast after 20 seconds
        m_uiShadowShockTimer = 6000;                            // 6 seconds
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LUCIFRON, FAIL);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LUCIFRON, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Impending Doom
        if (m_uiImpendingDoomTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMPENDING_DOOM);
            m_uiImpendingDoomTimer = 20000;
        }
        else
            m_uiImpendingDoomTimer -= uiDiff;

        // Lucifron's Curse
        if (m_uiLucifronCurseTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_LUCIFRON_CURSE);
            m_uiLucifronCurseTimer = 15000;
        }
        else
            m_uiLucifronCurseTimer -= uiDiff;

        // Shadow Shock
        if (m_uiShadowShockTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = 6000;
        }
        else
            m_uiShadowShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lucifron(Creature* pCreature)
{
    return new boss_lucifronAI(pCreature);
}

struct MANGOS_DLL_DECL mob_flamewaker_protectorAI : public ScriptedAI
{
    mob_flamewaker_protectorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_molten_core*)pCreature->GetInstanceData();
        Reset();
    }

    instance_molten_core* m_pInstance;

    uint32 m_uiCleaveTimer;
    uint32 m_uiDominateMindTimer;

    void Reset()
    {
        m_uiCleaveTimer = 3000;
        m_uiDominateMindTimer = 30000;
    }

	void JustRespawned()
	{
		//Make invisible if Lucifron is dead
		if (m_pInstance && m_pInstance->GetData(TYPE_LUCIFRON) == DONE)
		{
			m_creature->setFaction(35);
			m_creature->SetVisibility(VISIBILITY_OFF);
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
            m_uiCleaveTimer = 6000;
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Dominate Mind
        if (m_uiDominateMindTimer <= uiDiff)
        {
            // Boss Lucifron must be in Line of Sight
            if (m_pInstance)
            {
                Creature* pLucifron = m_pInstance->GetSingleCreatureFromStorage(NPC_LUCIFRON);
                if (pLucifron && m_creature->IsWithinLOSInMap(pLucifron))
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_DOMINATE_MIND);
            }

            m_uiDominateMindTimer = 30000;
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_flamewaker_protector(Creature* pCreature)
{
    return new mob_flamewaker_protectorAI(pCreature);
}

void AddSC_boss_lucifron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lucifron";
    pNewScript->GetAI = &GetAI_boss_lucifron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flamewaker_protector";
    pNewScript->GetAI = &GetAI_mob_flamewaker_protector;
    pNewScript->RegisterSelf();
}
