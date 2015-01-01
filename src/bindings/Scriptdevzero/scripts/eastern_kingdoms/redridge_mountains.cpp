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
SDName: Redridge_Mountains
SD%Complete: 100
SDComment: Support for quest 219.
SDCategory: Redridge Mountains
EndScriptData */

/* ContentData
npc_corporal_keeshan
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_corporal_keeshan
######*/

enum eCorporalKeeshan
{
    QUEST_MISSING_IN_ACTION   = 219,

    NPC_MARSHAL_MARRIS        = 382,

    SAY_CORPORAL_1			      = -1000543,
    SAY_CORPORAL_2			      = -1000544,
    SAY_CORPORAL_3			      = -1000545,
    SAY_CORPORAL_4			      = -1000546,
    SAY_CORPORAL_5            = -1000547,

    SPELL_MOCKING_BLOW        = 21008,
    SPELL_SHIELD_BASH         = 11972
};

struct MANGOS_DLL_DECL npc_corporal_keeshanAI : public npc_escortAI
{
  	npc_corporal_keeshanAI(Creature* pCreature) : npc_escortAI(pCreature)
  	{
        Reset();
  	}

  	uint32 m_uiMockingBlow_Timer;
  	uint32 m_uiShieldBash_Timer;
  	uint32 EventTimer;
  	uint32 EventPhase;
  	bool CanWalk;

    void Reset()
  	{
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiMockingBlow_Timer = 5000;
        m_uiShieldBash_Timer = 8000;

        EventTimer = 0;
        EventPhase = 0;
        CanWalk = true;
  	}

  	void WaypointReached(uint32 uiPointID)
  	{
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;
        
        switch(uiPointID)
        {
            case 39:
        				EventPhase = 1;
        				EventTimer = 1;
        				CanWalk = false;
        				break;
      			case 65:
        				//if (m_creature->GetUnitMovementFlag() == MOVEMENTFLAG_WALK_MODE)
        				//	m_creature->RemoveUnitMovementFlag(MOVEMENTFLAG_WALK_MODE);
        				SetRun(true);
        				break;
      			case 115:
        				m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                        pPlayer->GroupEventHappens(QUEST_MISSING_IN_ACTION, m_creature);
        				EventPhase = 5;
        				EventTimer = 2500;
        				break;
        }
  	}

  	void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(QUEST_MISSING_IN_ACTION);
    }

  	void UpdateAI(const uint32 uiDiff)
  	{
        if (EventTimer)
        {
  			   if (EventTimer <= uiDiff)
           {
              switch(EventPhase)
              {
        					case 1:
          						m_creature->SetStandState(UNIT_STAND_STATE_SIT);
          						EventTimer = 1500;
          						break;
        					case 2:
          						DoScriptText(SAY_CORPORAL_2, m_creature);
          						EventTimer = 15000;
          						break;
        					case 3:
          						DoScriptText(SAY_CORPORAL_3, m_creature);
          						m_creature->SetStandState(UNIT_STAND_STATE_STAND);
          						EventTimer = 1500;
          						break;
        					case 4:
          						CanWalk = true;
          						EventTimer = 0;
          						break;
        					case 5:
          						if (Creature* MarshalMarris = GetClosestCreatureWithEntry(m_creature, NPC_MARSHAL_MARRIS, 20.0f))
                          MarshalMarris->SetFacingToObject(m_creature);
          						DoScriptText(SAY_CORPORAL_4, m_creature);
          						EventTimer = 5000;
          						CanWalk = false;
          						break;
        					case 6:
          						DoScriptText(SAY_CORPORAL_5, m_creature);
          						EventTimer = 0; // event done
          						CanWalk = true;
          						break;
              }
              ++EventPhase;
          }
  				else EventTimer -= uiDiff;
  		}

  		if (CanWalk)
          npc_escortAI::UpdateAI(uiDiff);

      if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
          return;

  		//m_uiMockingBlow_Timer
  		if (m_uiMockingBlow_Timer < uiDiff)
  		{
          DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOCKING_BLOW);
          m_uiMockingBlow_Timer = 5000;
  		}
      else
          m_uiMockingBlow_Timer -= uiDiff;

  		//m_uiShieldBash_Timer
  		if (m_uiShieldBash_Timer < uiDiff)
  		{
    			DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIELD_BASH);
    			m_uiShieldBash_Timer = 8000;
  		}
      else
          m_uiShieldBash_Timer -= uiDiff;

  		DoMeleeAttackIfReady();
  	}
};
CreatureAI* GetAI_npc_corporal_keeshan(Creature* pCreature)
{
    return new npc_corporal_keeshanAI (pCreature);
}

bool QuestAccept_npc_corporal_keeshan(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_IN_ACTION)
    {
        if (npc_corporal_keeshanAI* pEscortAI = dynamic_cast<npc_corporal_keeshanAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_CORPORAL_1, pCreature);
            pEscortAI->Start(false, pPlayer, pQuest);
        }
    }
    return true;
}

/*######
## AddSC()
######*/

void AddSC_redridge_mountains()
{
    Script* pNewscript;
    pNewscript = new Script;
    pNewscript->Name = "npc_corporal_keeshan";
    pNewscript->GetAI = &GetAI_npc_corporal_keeshan;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_corporal_keeshan;
    pNewscript->RegisterSelf();
}
