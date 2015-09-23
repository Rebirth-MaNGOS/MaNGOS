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
SDName: Urok Doomhowl
SD%Complete: 90
SDComment: Correct Challenge to Urok gameobject effect kill some Urok's add
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eUrokDoomhowl
{
	SPELL_INTIMIDATING_ROAR = 16508,
	SPELL_REND              = 16509,
	SPELL_STRIKE            = 15580
};

struct MANGOS_DLL_DECL boss_urok_doomhowlAI : public ScriptedAI
{
    boss_urok_doomhowlAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    uint32 m_uiIntimidatingRoarTimer;
    uint32 m_uiRendTimer;
    uint32 m_uiStrikeTimer;

    uint32 EventTimer;
    uint32 EventPhase;

    void Reset()
    {
        m_uiIntimidatingRoarTimer = urand(10000,20000);
        m_uiRendTimer = 10000;
        m_uiStrikeTimer = urand(5000,7000);

        // If event is done or in_progress, return
        if (m_pInstance && m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == SPECIAL)
            return;

        EventTimer = 0;
        EventPhase = 0;

        // If boss is dead and event is done, return
        if (m_pInstance && m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == DONE && m_creature->isDead() && !m_creature->isAlive())
            return;

        SetVisible(false);
    }

    void SetVisible(bool set_visible = true)
    {
        if (set_visible)
        {
            m_creature->SetVisibility(VISIBILITY_ON);
  			m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
  			m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NOT_ATTACKABLE_1);
        }
        else
        {
            m_creature->AttackStop();
  			m_creature->CombatStop();
  			m_creature->SetVisibility(VISIBILITY_OFF);
  			m_creature->setFaction(FACTION_FRIENDLY_FOR_ALL);
  			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NOT_ATTACKABLE_1);
        }
    }

    void SpawnWaveOrBoss()
    {
        switch(++EventPhase)
        {
            case 1:
				DoSpawnCreature(NPC_UROK_ENFORCER, 4.f, 2.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
				DoSpawnCreature(NPC_UROK_ENFORCER, 2.f, 4.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
				DoSpawnCreature(NPC_UROK_ENFORCER, 3.f, 3.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
			    break;
            case 2:
				DoSpawnCreature(NPC_UROK_ENFORCER, 4.f, 2.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
				DoSpawnCreature(NPC_UROK_OGRE_MAGUS, 2.f, 4.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
				break;
  			case 3:
  			case 4:
  			case 5:
				DoSpawnCreature(NPC_UROK_OGRE_MAGUS, 4.f, 2.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
				DoSpawnCreature(NPC_UROK_OGRE_MAGUS, 2.f, 4.f, 0.f, 0.f, TEMPSUMMON_DEAD_DESPAWN, 0);
                break;
            case 6:
				SetVisible();
				//EventPhase = 0;
				EventTimer = 0; // event done
                break;
    		}
    }
    
    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AI()->AttackStart(pTarget);
    }
    
    void JustDied(Unit* /*pKiller*/)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_UROK_DOOMHOWL, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // If event is done or gameobject Challenge to Urok TRAP is not spawned, return
        if ((m_pInstance && m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == DONE) || !GetClosestGameObjectWithEntry(m_creature, GO_CHALLENGE_TO_UROK_TRAP, 200.f))
            return;
        
        // If summoning adds event is not started, start him :)
        if (m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == SPECIAL && !EventTimer && !EventPhase)
            EventTimer = 1;
        
        if (EventTimer)
        {
            if (EventTimer <= uiDiff)
            {
                EventTimer = 30000;
                SpawnWaveOrBoss();
            }
            else
                EventTimer -= uiDiff;
        }

        // Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Intimidating Roar spell
        if (m_uiIntimidatingRoarTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTIMIDATING_ROAR);
            m_uiIntimidatingRoarTimer = urand(12000,18000);
        }
        else
            m_uiIntimidatingRoarTimer -= uiDiff;

        // Rend spell
        if (m_uiRendTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);
            m_uiRendTimer = urand(8000,12000);
        }
        else
            m_uiRendTimer -= uiDiff;
        
        // Strike spell
        if (m_uiStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(4000,7000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_urok_doomhowl(Creature* pCreature)
{
    return new boss_urok_doomhowlAI(pCreature);
}

void AddSC_boss_urok_doomhowl()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "boss_urok_doomhowl";
    pNewscript->GetAI = &GetAI_boss_urok_doomhowl;
    pNewscript->RegisterSelf();
}
