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
SDComment: Correct Challenge to Urok gameobject effect kill some Urok's add. Gobj is only usable once(to start the event, then never again).
SDCategory: Blackrock Spire
EndScriptData */

#include "precompiled.h"
#include "blackrock_spire.h"

enum eUrokDoomhowl
{
	SPELL_INTIMIDATING_ROAR = 16508,
	SPELL_REND              = 16509,
	SPELL_STRIKE            = 15580,
	SPELL_SIMPLE_TELEPORT	= 16807,		// Spawn visual
	SPELL_SUMMON_UROK		= 16473			// Urok spawn visual
};

struct Locd
{
    float x, y, z, o;
};

static Locd aOgreSpawn[]=
{
    {-27.4654f, -386.146f, 48.5094f, 1.53423f},
	{-14.481f, -385.668f, 48.9201f, 1.59706f},
    {-12.2959f, -376.803f, 49.3246f, 1.59313f},
	{-24.613f, -369.723f, 49.6684f, 3.35637f},
	{-35.0492f, -370.308f, 50.3656f, 5.55941f},
	{-47.203f, -369.248f, 51.4629f, 1.12192f},
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

	GUIDList m_lOgreRune;

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
			m_creature->CastSpell(m_creature,SPELL_SUMMON_UROK,true);
			 if (Unit* pTarget = GetRandomPlayerInCurrentMap(30))
				m_creature->AI()->AttackStart(pTarget);
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

	void SpawnRunes(int action = 0)
	{
		if(action == 10)
		{		
			for(uint32 i = 0; i < 7; ++i)
			{
				if(Object* pRune = m_creature->SummonGameObject(GO_OGRE_RUNE,0,aOgreSpawn[i].x,aOgreSpawn[i].y,aOgreSpawn[i].z,0,GO_STATE_READY,100))
				{	
					m_lOgreRune.push_back(pRune->GetObjectGuid());
				}
			}
		}
		if(action == 11)
		{	
			for (GUIDList::iterator itr = m_lOgreRune.begin(); itr != m_lOgreRune.end(); itr++)
			{
				if (Object* pRune = m_creature->GetMap()->GetGameObject(*itr))
					pRune->RemoveFromWorld();
			}
		}
		else	// spawn a new rune that does the animation
			m_creature->SummonGameObject(GO_OGRE_RUNE,3000,aOgreSpawn[action].x,aOgreSpawn[action].y,aOgreSpawn[action].z,0,GO_STATE_ACTIVE,100);
	}

    void SpawnWaveOrBoss()
    {
		int r = urand(0,2);		// random kind of mob
		int a = urand(4,5);		// a bit random spawn
        switch(++EventPhase)
        {	
			case 1:
				SpawnRunes(10);
				SpawnRunes(0);			// lots of them, oh well
				SpawnRunes(1);
				SpawnRunes(2);
				m_creature->SummonCreature(NPC_UROK_OGRE_MAGUS,aOgreSpawn[0].x,aOgreSpawn[0].y,aOgreSpawn[0].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				m_creature->SummonCreature(NPC_UROK_ENFORCER, aOgreSpawn[1].x,aOgreSpawn[1].y,aOgreSpawn[1].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				m_creature->SummonCreature(NPC_UROK_ENFORCER, aOgreSpawn[2].x,aOgreSpawn[2].y,aOgreSpawn[2].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 30000;
			    break;
            case 2:
				SpawnRunes(3);
				m_creature->SummonCreature(NPC_UROK_ENFORCER, aOgreSpawn[3].x,aOgreSpawn[3].y,aOgreSpawn[3].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 2000;
				break;
  			case 3:
				SpawnRunes(a);
				m_creature->SummonCreature(NPC_UROK_ENFORCER, aOgreSpawn[a].x,aOgreSpawn[a].y,aOgreSpawn[a].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 32000;
				break;
  			case 4:		
				SpawnRunes(1);
				m_creature->SummonCreature((r>0?NPC_UROK_ENFORCER:NPC_UROK_OGRE_MAGUS), aOgreSpawn[1].x,aOgreSpawn[1].y,aOgreSpawn[1].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 5000;
				break;
  			case 5:
				SpawnRunes(0);
				m_creature->SummonCreature(NPC_UROK_ENFORCER,aOgreSpawn[0].x,aOgreSpawn[0].y,aOgreSpawn[0].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
                EventTimer = 10000;
				break;
			case 6:
				SpawnRunes(2);
				m_creature->SummonCreature((r>0?NPC_UROK_ENFORCER:NPC_UROK_OGRE_MAGUS), aOgreSpawn[2].x,aOgreSpawn[2].y,aOgreSpawn[2].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 8000;
				break;
			case 7:
				SpawnRunes(0);
				m_creature->SummonCreature(NPC_UROK_OGRE_MAGUS,aOgreSpawn[0].x,aOgreSpawn[0].y,aOgreSpawn[0].z,0, TEMPSUMMON_DEAD_DESPAWN, 0);
				EventTimer = 8000;
				break;
            case 8:
				SpawnRunes(11);
				SetVisible();
				//EventPhase = 0;
				EventTimer = 0; // event done
                break;
    		}
    }
    
    void JustSummoned(Creature* pSummoned)
    {
		pSummoned->CastSpell(pSummoned,SPELL_SIMPLE_TELEPORT,true);

        if (Unit* pTarget = GetRandomPlayerInCurrentMap(30))
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
        if ((m_pInstance && m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == NOT_STARTED || m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == DONE)/* || !GetClosestGameObjectWithEntry(m_creature, GO_CHALLENGE_TO_UROK_TRAP, 200.f)*/)
            return;
        
        // If summoning adds event is not started, start him :)
        if (m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == SPECIAL && !EventTimer && !EventPhase)
            EventTimer = 1;
        
        if (EventTimer)
        {
            if (EventTimer <= uiDiff)            
                SpawnWaveOrBoss();         
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
