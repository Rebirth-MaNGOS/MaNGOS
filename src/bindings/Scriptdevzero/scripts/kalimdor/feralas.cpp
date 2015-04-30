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
SDName: Feralas
SD%Complete: 100
SDComment: Quest support: 976, 3520, 2767. Special vendor Gregan Brewspewer
SDCategory: Feralas
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"

/*######
## npc_gregan_brewspewer
######*/

bool GossipHello_npc_gregan_brewspewer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor() && pPlayer->GetQuestStatus(3909) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Buy somethin', will ya?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(2433, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_gregan_brewspewer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(2434, pCreature->GetObjectGuid());
    }
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
    return true;
}

/*######
## npc_oox22fe
######*/

enum
{
    SAY_OOX_START           = -1000287,
    SAY_OOX_AGGRO1          = -1000288,
    SAY_OOX_AGGRO2          = -1000289,
    SAY_OOX_AMBUSH          = -1000290,
    SAY_OOX_END             = -1000292,

    NPC_YETI                = 7848,
    NPC_GORILLA             = 5260,
    NPC_WOODPAW_REAVER      = 5255,
    NPC_WOODPAW_BRUTE       = 5253,
    NPC_WOODPAW_ALPHA       = 5258,
    NPC_WOODPAW_MYSTIC      = 5254,

    QUEST_RESCUE_OOX22FE    = 2767
};

struct MANGOS_DLL_DECL npc_oox22feAI : public npc_escortAI
{
    npc_oox22feAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void WaypointReached(uint32 i)
    {
        switch (i)
        {
            // First Ambush(3 Yetis)
            case 11:
                DoScriptText(SAY_OOX_AMBUSH,m_creature);
                m_creature->SummonCreature(NPC_YETI, -4841.01f, 1593.91f, 73.42f, 3.98f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_YETI, -4837.61f, 1568.58f, 78.21f, 3.13f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_YETI, -4841.89f, 1569.95f, 76.53f, 0.68f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                break;
            //Second Ambush(3 Gorillas)
            case 21:
                DoScriptText(SAY_OOX_AMBUSH,m_creature);
                m_creature->SummonCreature(NPC_GORILLA, -4595.81f, 2005.99f, 53.08f, 3.74f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_GORILLA, -4597.53f, 2008.31f, 52.70f, 3.78f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_GORILLA, -4599.37f, 2010.59f, 52.77f, 3.84f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                break;
            //Third Ambush(4 Gnolls)
            case 30:
                DoScriptText(SAY_OOX_AMBUSH,m_creature);
                m_creature->SummonCreature(NPC_WOODPAW_REAVER, -4425.14f, 2075.87f, 47.77f, 3.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_WOODPAW_BRUTE , -4426.68f, 2077.98f, 47.57f, 3.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_WOODPAW_MYSTIC, -4428.33f, 2080.24f, 47.43f, 3.87f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                m_creature->SummonCreature(NPC_WOODPAW_ALPHA , -4430.04f, 2075.54f, 46.83f, 3.81f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                break;
            case 37:
                DoScriptText(SAY_OOX_END,m_creature);
                // Award quest credit
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_RESCUE_OOX22FE, m_creature);
                break;
        }
    }

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
    }

    void Aggro(Unit* /*who*/)
    {
        //For an small probability the npc says something when he get aggro
        switch(urand(0, 9))
        {
           case 0: DoScriptText(SAY_OOX_AGGRO1, m_creature); break;
           case 1: DoScriptText(SAY_OOX_AGGRO2, m_creature); break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(m_creature);
    }
};

CreatureAI* GetAI_npc_oox22fe(Creature* pCreature)
{
    return new npc_oox22feAI(pCreature);
}

bool QuestAccept_npc_oox22fe(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RESCUE_OOX22FE)
    {
        DoScriptText(SAY_OOX_START, pCreature);
        //change that the npc is not lying dead on the ground
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        if (pPlayer->GetTeam() == ALLIANCE)
            pCreature->setFaction(FACTION_ESCORT_A_PASSIVE);

        if (pPlayer->GetTeam() == HORDE)
            pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

        if (npc_oox22feAI* pEscortAI = dynamic_cast<npc_oox22feAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
## npc_screecher_spirit
######*/

struct MANGOS_DLL_DECL npc_screecher : public ScriptedAI
{
	npc_screecher(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	uint32 SonicBurst_timer;

	void Reset() 
	{
		SonicBurst_timer = urand(5000,8000);
	}

	void SpellHit(Unit* /*U*/, const SpellEntry* S)
	{
		if(S->Id == 12699)
		{
			m_creature->SaveRespawnTime();
			m_creature->SetDeathState(JUST_DIED);
			m_creature->RemoveCorpse();
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if (SonicBurst_timer <= uiDiff)
		{
			SonicBurst_timer = urand(11000,15000);
			m_creature->CastSpell(m_creature->getVictim(),8281,false);
		}
		else
			SonicBurst_timer -= uiDiff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_screecher(Creature* pCreature)
{
    return new npc_screecher(pCreature);
}

bool GossipHello_npc_screecher_spirit(Player* pPlayer, Creature* pCreature)
{
    pPlayer->SEND_GOSSIP_MENU(2039, pCreature->GetObjectGuid());
    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    return true;
}

/*######
## Gordunni Trap
######*/

bool go_use_gordunni_trap(Player* pPlayer, GameObject* pGo)
{
	if (pGo->isSpawned())
	{
		pPlayer->CastSpell(pPlayer,19394,false);
		pGo->SetLootState(GO_JUST_DEACTIVATED);
	}
	return true;
}

/*#########
## npc_shay_leafrunner
#####*/

enum
{
	NPC_SHAY_LEAFRUNNER_RUNOFF_ONE = -1712015,
	NPC_SHAY_LEAFRUNNER_RUNOFF_TWO = -1712017,
	NPC_SHAY_LEAFRUNNER_RUNOFF_THREE = -1712018,
	NPC_SHAY_LEAFRUNNER_RUNOFF_FOUR = -1712019,
	NPC_SHAY_LEAFRUNNER_RETURN_ONE = -1712016,
	NPC_SHAY_LEAFRUNNER_RETURN_TWO = -1712020,

	NPC_SHAY_LEAFRUNNER = 7774,
	NPC_ROCKBITER = 7765,

	QUEST_WANDERING_SHAY = 2845,

	TIMER_SHAY_RUNOFF_MIN = 90000,
	TIMER_SHAY_RUNOFF_MAX = 120000,
};

struct MANGOS_DLL_DECL npc_shay_leafrunnerAI : public FollowerAI
{
    npc_shay_leafrunnerAI(Creature* pCreature) : FollowerAI(pCreature) 
	{ 
		runningOff = false;
		Reset(); 
	}

	uint32 m_uiRunOffTimer;
	bool runningOff;
	Unit* pShay;

	void Reset() 
	{
		if(!runningOff && !m_uiRunOffTimer)
		{
			m_creature->StopMoving();
			m_creature->GetMotionMaster()->Clear();
			m_creature->GetMotionMaster()->MoveIdle();
			SetFollowPaused(false);
			randReturnSpeach();
			m_uiRunOffTimer = urand(TIMER_SHAY_RUNOFF_MIN, TIMER_SHAY_RUNOFF_MAX);
		}
		m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
	}

	/*
	* Randomly picks one of Shay's four runaway speeches.
	*/

	void randRunoffSpeach()
	{
		switch(urand(1,4))
		{
		case 1:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RUNOFF_ONE, m_creature);
			break;
		case 2:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RUNOFF_TWO, m_creature);
			break;
		case 3:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RUNOFF_THREE, m_creature);
			break;
		case 4:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RUNOFF_FOUR, m_creature);
			break;
		}
	}

	/*
	* Randomly picks one of Shay's two return speeches.
	*/

	void randReturnSpeach()
	{
		switch(urand(1,2))
		{
		case 1:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RETURN_ONE, m_creature);
			break;
		case 2:
			DoScriptText(NPC_SHAY_LEAFRUNNER_RETURN_TWO, m_creature);
			break;
		}
	}

	/*
	* Checks if the bell used to bring Shay back has been rung.
	*/

	void hasBellRungYet()
	{
		if(HasFollowState(STATE_FOLLOW_PAUSED))
		{
			if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
			{
				SetFollowPaused(false);
				randReturnSpeach();
				m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
				runningOff = false;
				m_uiRunOffTimer = urand(TIMER_SHAY_RUNOFF_MIN, TIMER_SHAY_RUNOFF_MAX);
			}
			else if(m_creature->isInCombat())
			{
				runningOff = false;
			}
		}
	}

	/*
	* Checks if the escort has reached the destination, Rockbiter.
	*/

	void areWeThereYet()
	{
		if(GetClosestCreatureWithEntry(m_creature, NPC_ROCKBITER, 8.0f))
		{
			if (Player* pPlayer = GetLeaderForFollower())
            {
                if (pPlayer->GetQuestStatus(QUEST_WANDERING_SHAY) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_WANDERING_SHAY, m_creature);
            }
				SetFollowComplete();
		}
	}

	void StartFollow(Player* pLeader, uint32 uiFactionForFollower, const Quest* pQuest)
	{
		m_uiRunOffTimer = urand(TIMER_SHAY_RUNOFF_MIN, TIMER_SHAY_RUNOFF_MAX);
		FollowerAI::StartFollow(pLeader,uiFactionForFollower,pQuest);
	}

	bool isPaused()
	{
		return runningOff;
	}

	  void UpdateFollowerAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {

			if(HasFollowState(STATE_FOLLOW_INPROGRESS) && m_uiRunOffTimer)
			{

				if(m_uiRunOffTimer <= uiDiff)
				{
					Player* pPlayer = GetLeaderForFollower();
					randRunoffSpeach();
					SetFollowPaused(true);

					if(pPlayer)
						m_creature->GetMotionMaster()->MoveFleeing(pPlayer);

					m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
					runningOff = true;
					m_uiRunOffTimer = 0;
				}
				else
				{
					m_uiRunOffTimer -= uiDiff;
				}

			}
				
			if(m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLEEING_MOTION_TYPE)
			{
				m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
			}

			areWeThereYet();

			hasBellRungYet();

			if(GetLeaderForFollower() && GetLeaderForFollower()->GetQuestStatus(QUEST_WANDERING_SHAY) == QUEST_STATUS_FAILED)
			{
				SetFollowComplete();
			}

			return;
	  
		}

		hasBellRungYet();
	 
	    DoMeleeAttackIfReady();
	}


};

CreatureAI* GetAI_npc_shay_leafrunner(Creature* pCreature)
{
    return new npc_shay_leafrunnerAI(pCreature);
}

bool QuestAccept_npc_shay_leafrunner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WANDERING_SHAY)
    {
		if (npc_shay_leafrunnerAI* pShayAI = dynamic_cast<npc_shay_leafrunnerAI*>(pCreature->AI()))
            pShayAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
    }
    return true;
}

/*######
## AddSC
######*/

void AddSC_feralas()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_gregan_brewspewer";
    pNewscript->pGossipHello = &GossipHello_npc_gregan_brewspewer;
    pNewscript->pGossipSelect = &GossipSelect_npc_gregan_brewspewer;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_oox22fe";
    pNewscript->GetAI = &GetAI_npc_oox22fe;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_oox22fe;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_screecher_spirit";
    pNewscript->pGossipHello = &GossipHello_npc_screecher_spirit;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_screecher";
	pNewscript->GetAI = &GetAI_npc_screecher;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
	pNewscript->Name = "go_gordunni_trap";
    pNewscript->pGOUse = &go_use_gordunni_trap;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
	pNewscript->Name = "npc_shay_leafrunner";
	pNewscript->GetAI = &GetAI_npc_shay_leafrunner;
	pNewscript->pQuestAcceptNPC = &QuestAccept_npc_shay_leafrunner;
	pNewscript->RegisterSelf();
}
