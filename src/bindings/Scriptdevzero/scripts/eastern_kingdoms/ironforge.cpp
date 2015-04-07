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
SDName: Ironforge
SD%Complete: 100
SDComment: Quest support: 3702
SDCategory: Ironforge
EndScriptData */

/* ContentData
npc_royal_historian_archesonus
EndContentData */

#include "precompiled.h"
#include "follower_ai.h"

/*####
# npc_deeprun_rat
####*/

enum
{
    SPELL_FLUTE = 21050,
    SPELL_ENTHRALLED = 21051,
    SPELL_BASH_RATS = 21052,

    QUEST_ROUNDUP = 6661,

    NPC_MONTY = 12997,
    NPC_RAT = 13016,
    NPC_RAT_F = 13017
};

struct MANGOS_DLL_DECL npc_deeprun_ratAI : public FollowerAI
{
    npc_deeprun_ratAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    uint32 m_uiTimer;

    void Reset()
    {
        m_uiTimer = 0;

        if (m_creature->GetEntry() == NPC_RAT_F)
        {
            if (!m_creature->HasSplineFlag(SPLINEFLAG_WALKMODE))
                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);

            m_creature->UpdateEntry(NPC_RAT);
            m_creature->RemoveAurasDueToSpell(SPELL_ENTHRALLED);
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_creature->getVictim())
            return;

        if (pWho->GetEntry() == NPC_MONTY && m_creature->GetEntry() == NPC_RAT_F)
        {
            if (m_creature->IsWithinDistInMap(pWho, 10.0f) && !HasFollowState(STATE_FOLLOW_COMPLETE))
            {
                if (GetLeaderForFollower()->GetQuestStatus(QUEST_ROUNDUP) == QUEST_STATUS_COMPLETE)
                {
                    m_uiTimer = 30000;
                    SetFollowComplete(true);
                    m_creature->GetMotionMaster()->MovePoint(0, -20.5f, 34.7f, -4.3f);
                }
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_FLUTE && pCaster->GetTypeId() == TYPEID_PLAYER && m_creature->GetEntry() == NPC_RAT)
        {
            if (((Player*)pCaster)->GetQuestStatus(QUEST_ROUNDUP) == QUEST_STATUS_INCOMPLETE ||
                ((Player*)pCaster)->GetQuestStatus(QUEST_ROUNDUP) == QUEST_STATUS_COMPLETE)
            {
                m_creature->UpdateEntry(NPC_RAT_F);
                m_creature->CastSpell(m_creature, SPELL_ENTHRALLED, false);

                StartFollow((Player*)pCaster);

                if (m_creature->HasSplineFlag(SPLINEFLAG_WALKMODE))
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            }

        }
        else if (pSpell->Id == SPELL_BASH_RATS && m_creature->GetEntry() == NPC_RAT_F)
            SetFollowComplete();
        else if (pSpell->Id == SPELL_ENTHRALLED && m_creature->getFaction() != FACTION_ESCORT_A_PASSIVE)
            m_creature->setFaction(FACTION_ESCORT_A_PASSIVE);
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (m_uiTimer && HasFollowState(STATE_FOLLOW_COMPLETE))
        {
            if (m_uiTimer < uiDiff)
                SetFollowComplete();
            else
                m_uiTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_deeprun_rat(Creature* pCreature)
{
    return new npc_deeprun_ratAI(pCreature);
}

/*######
## npc_royal_historian_archesonus
######*/

#define GOSSIP_ITEM_ROYAL   "I am ready to listen"
#define GOSSIP_ITEM_ROYAL_1 "That is tragic. How did this happen?"
#define GOSSIP_ITEM_ROYAL_2 "Interesting, continue please."
#define GOSSIP_ITEM_ROYAL_3 "Unbelievable! How dare they??"
#define GOSSIP_ITEM_ROYAL_4 "Of course I will help!"

bool GossipHello_npc_royal_historian_archesonus(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(3702) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(2235, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}


bool GossipSelect_npc_royal_historian_archesonus(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2236, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2237, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2238, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ROYAL_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(2239, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(3702);
            break;
    }
    return true;
}

/*######
## npc_haggle
######*/

enum
{
	HAGGLE_SAY_1 = -1720079,
	HAGGLE_SAY_2 = -1720080,
	HAGGLE_SAY_3 = -1720081,
	HAGGLE_SAY_4 = -1720082,
	HAGGLE_SAY_5 = -1720083,
	HAGGLE_SAY_6 = -1720084,
	HAGGLE_SAY_7 = -1720085,

	SPELL_BUFF_SLEEP      = 17743,
};

struct MANGOS_DLL_DECL npc_haggleAI : public ScriptedAI
{
    npc_haggleAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiRandomWakeUpTimer;

    void Reset()
    {		
		m_uiRandomWakeUpTimer = 0;
    }
	
	void MovementInform(uint32 /*uiMotiontype*/, uint32 uiPointId)
	{
		switch(uiPointId)
        {
			case 3:
			case 17:
			case 19:
			case 23:
			case 35:
			case 40:
				int rand = urand(1,5);
				if(rand == 1)
					DoScriptText(HAGGLE_SAY_7, m_creature);
				break;
		}
		if(uiPointId)
		{
			int randomAction = urand(1,100);					// sometimes do random say
			if (randomAction < 5)
				RandomSay();

			if (randomAction > 35 && randomAction < 39)			// sometimes fall asleep
			if (!m_creature->HasAura(SPELL_BUFF_SLEEP,EFFECT_INDEX_0))
				m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,true);
            m_uiRandomWakeUpTimer = urand(30000,75000);
		}
	}

	void RandomSay()
	{
		int r = urand(1,6);
		switch(r)
		{
			case 1:
				DoScriptText(HAGGLE_SAY_1, m_creature);
				break;
			case 2:
				DoScriptText(HAGGLE_SAY_2, m_creature);
				break;
			case 3:
				DoScriptText(HAGGLE_SAY_3, m_creature);
				break;
			case 4:
				DoScriptText(HAGGLE_SAY_4, m_creature);
				break;
			case 5:
				DoScriptText(HAGGLE_SAY_5, m_creature);
				break;
			case 6:
				DoScriptText(HAGGLE_SAY_6, m_creature);
				break;
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
		// Random wake up
        if (m_uiRandomWakeUpTimer < uiDiff)
		{            
			if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != WAYPOINT_MOTION_TYPE)
				m_creature->GetMotionMaster()->MoveWaypoint();
			m_creature->RemoveAllAuras();
		}
        else
            m_uiRandomWakeUpTimer -= uiDiff;	

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_haggle(Creature* pCreature)
{
    return new npc_haggleAI(pCreature);
}

void AddSC_ironforge()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_deeprun_rat";
    pNewscript->GetAI = &GetAI_npc_deeprun_rat;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_royal_historian_archesonus";
    pNewscript->pGossipHello =  &GossipHello_npc_royal_historian_archesonus;
    pNewscript->pGossipSelect = &GossipSelect_npc_royal_historian_archesonus;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "npc_haggle";
	pNewscript->GetAI = &GetAI_npc_haggle;
	pNewscript->RegisterSelf();
}
