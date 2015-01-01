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
SDName: Solakar_Flamewreath
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eSolakar
{
    SAY_HATCHER         = -1229016,

    SPELL_WAR_STOMP     = 16727
};

struct MANGOS_DLL_DECL boss_solakar_flamewreathAI : public ScriptedAI
{
    boss_solakar_flamewreathAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    bool bEventStarted;
    bool bHatcherYell;

    uint32 m_uiWarStompTimer;
    uint32 EventTimer;
    uint32 EventPhase;
	uint32 ResetTimer;

	GUIDList Summons;

    void Reset()
    {
        m_uiWarStompTimer = 10000;

		DespawnSummons();
		Summons.clear();
        EventTimer = 0;
        EventPhase = 0;
		ResetTimer = 0;
        bEventStarted = false;
        bHatcherYell = false;

        SetSolakarVisible(false);
    }

	void JustReachedHome()
	{
		if (m_pInstance)
			m_pInstance->SetData(TYPE_ROOKERY,NOT_STARTED);
	}

	void DespawnSummons()
	{
		if (Summons.empty())
			return;

		for (GUIDList::iterator itr = Summons.begin(); itr != Summons.end(); itr++)
			if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))	
			{
				summon->SetHealth(0);
				summon->SetDeathState(JUST_DIED);
			}
	}
    
    void SetSolakarVisible(bool set_visible = true)
    {
		if (set_visible)
		{
			m_creature->SetVisibility(VISIBILITY_ON);
			m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			m_creature->SetInCombatWithZone();
		}
		else
		{
			m_creature->AttackStop();
			m_creature->CombatStop();
			m_creature->SetVisibility(VISIBILITY_OFF);
			m_creature->setFaction(FACTION_FRIENDLY_FOR_ALL);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}
    }

    void SpawnSolakarMobsWave()
    {
		DoSpawnCreature(NPC_ROOKERY_GUARDIAN, 4.f, 4.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
		DoSpawnCreature(NPC_ROOKERY_GUARDIAN, 4.f, 6.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
		DoSpawnCreature(NPC_ROOKERY_HATCHER, 7.f, 4.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
		DoSpawnCreature(NPC_ROOKERY_HATCHER, 7.f, 6.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
    }

    void JustSummoned(Creature* pSummoned)
    {
		if (!bHatcherYell && pSummoned->GetEntry() == NPC_ROOKERY_HATCHER)
		{
            bHatcherYell = true;
            DoScriptText(SAY_HATCHER, pSummoned);
		}

		Summons.push_back(pSummoned->GetObjectGuid());
		pSummoned->SetInCombatWithZone();

		//if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
  //          pSummoned->AI()->AttackStart(pTarget);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROOKERY, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!bEventStarted && m_pInstance && m_pInstance->GetData(TYPE_ROOKERY) == IN_PROGRESS)
        {
            if (m_pInstance->GetData(TYPE_ROOKERY) != DONE)
            {
                bEventStarted = true;
                EventPhase = 1;
                EventTimer = 1;
            }
        }

        if (EventTimer)
        {
            if (EventTimer <= uiDiff)
            {
                if (EventPhase != 6)
                {
					ResetTimer = 3000;
                    SpawnSolakarMobsWave();
                    EventPhase++;
                    EventTimer = 30000;
                }
                else
                {
                    SetSolakarVisible();
                    EventPhase = 0;
                    EventTimer = 0;
                }
            }
            else
                EventTimer -= uiDiff;
        }

		if (ResetTimer)
		{
			if (ResetTimer <= uiDiff)
			{
				ResetTimer = 3000;
				for (GUIDList::iterator itr = Summons.begin(); itr != Summons.end(); itr++)
					if (Creature* summon = m_creature->GetMap()->GetCreature(*itr))
						if (summon->isAlive() && (!summon->getVictim() || !summon->SelectHostileTarget()))
						{
							if (m_pInstance)
								m_pInstance->SetData(TYPE_ROOKERY,NOT_STARTED);
							Reset();
							break;
						}
			}
			else
				ResetTimer -= uiDiff;
		}

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // War Stomp
        if (m_uiWarStompTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WAR_STOMP);
            m_uiWarStompTimer = urand(10000,15000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_solakar_flamewreath(Creature* pCreature)
{
    return new boss_solakar_flamewreathAI(pCreature);
}

void AddSC_boss_solakar_flamewreath()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_solakar_flamewreath";
    pNewscript->GetAI = &GetAI_boss_solakar_flamewreath;
    pNewscript->RegisterSelf();
}
