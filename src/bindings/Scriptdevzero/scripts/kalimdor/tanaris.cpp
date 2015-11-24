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
SDName: Tanaris
SD%Complete: 80
SDComment: Quest support: 648, 1560, 2882, 2954, 4005, 10277, 10279(Special flight path). Noggenfogger vendor
SDCategory: Tanaris
EndScriptData */

/* ContentData
mob_aquementas
npc_marin_noggenfogger
npc_oox17tn
npc_stone_watcher_of_norgannon
npc_tooga
npc_yehkinya
go_landmark_treasure
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "follower_ai.h"
#include "Language.h"

/*######
## mob_aquementas
######*/

#define AGGRO_YELL_AQUE     -1000168

#define SPELL_AQUA_JET      13586
#define SPELL_FROST_SHOCK   15089

struct MANGOS_DLL_DECL mob_aquementasAI : public ScriptedAI
{
    mob_aquementasAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 SendItem_Timer;
    uint32 SwitchFaction_Timer;
    bool isFriendly;

    uint32 FrostShock_Timer;
    uint32 AquaJet_Timer;

    void Reset()
    {
        SendItem_Timer = 0;
        SwitchFaction_Timer = 10000;
        m_creature->setFaction(35);
        isFriendly = true;

        AquaJet_Timer = 5000;
        FrostShock_Timer = 1000;
    }

    void SendItem(Unit* receiver)
    {
        if (((Player*)receiver)->HasItemCount(11169,1,false) &&
            ((Player*)receiver)->HasItemCount(11172,11,false) &&
            ((Player*)receiver)->HasItemCount(11173,1,false) &&
            !((Player*)receiver)->HasItemCount(11522,1,true))
        {
            if (Item* pItem = ((Player*)receiver)->StoreNewItemInInventorySlot(11522, 1))
                ((Player*)receiver)->SendNewItem(pItem, 1, true, false);
        }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(AGGRO_YELL_AQUE, m_creature, who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (isFriendly)
        {
            if (SwitchFaction_Timer < diff)
            {
                m_creature->setFaction(91);
                isFriendly = false;
            }else SwitchFaction_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!isFriendly)
        {
            if (SendItem_Timer < diff)
            {
                if (m_creature->getVictim()->GetTypeId() == TYPEID_PLAYER)
                    SendItem(m_creature->getVictim());
                SendItem_Timer = 5000;
            }else SendItem_Timer -= diff;
        }

        if (FrostShock_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_FROST_SHOCK);
            FrostShock_Timer = 15000;
        }else FrostShock_Timer -= diff;

        if (AquaJet_Timer < diff)
        {
            DoCastSpellIfCan(m_creature,SPELL_AQUA_JET);
            AquaJet_Timer = 15000;
        }else AquaJet_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_aquementas(Creature* pCreature)
{
    return new mob_aquementasAI(pCreature);
}


/*######
## npc_marin_noggenfogger
######*/

bool GossipHello_npc_marin_noggenfogger(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(2662))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_marin_noggenfogger(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_oox17tn
######*/

enum
{
    SAY_OOX_START           = -1000287,
    SAY_OOX_AGGRO1          = -1000288,
    SAY_OOX_AGGRO2          = -1000289,
    SAY_OOX_AMBUSH          = -1000290,
    SAY_OOX17_AMBUSH_REPLY  = -1000291,
    SAY_OOX_END             = -1000292,

    QUEST_RESCUE_OOX_17TN   = 648,

    NPC_SCORPION            = 7803,
    NPC_SCOFFLAW            = 7805,
    NPC_SHADOW_MAGE         = 5617
};

struct MANGOS_DLL_DECL npc_oox17tnAI : public npc_escortAI
{
    npc_oox17tnAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

	bool m_bAreScorpionsSpawned;
	bool m_bAreHumansSpawned;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            //1. Ambush: 3 scorpions
            case 22:
				if (!m_bAreScorpionsSpawned)
				{
					DoScriptText(SAY_OOX_AMBUSH, m_creature);
					m_creature->SummonCreature(NPC_SCORPION, -8340.70f, -4448.17f, 9.17f, 3.10f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
					m_creature->SummonCreature(NPC_SCORPION, -8343.18f, -4444.35f, 9.44f, 2.35f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
					m_creature->SummonCreature(NPC_SCORPION, -8348.70f, -4457.80f, 9.58f, 2.02f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);

					m_bAreScorpionsSpawned = true;
				}
                break;
            //2. Ambush: 2 Rogues & 1 Shadow Mage
            case 28:
                DoScriptText(SAY_OOX_AMBUSH, m_creature);

				if (!m_bAreHumansSpawned)
				{
					m_creature->SummonCreature(NPC_SCOFFLAW, -7488.02f, -4786.56f, 10.67f, 3.74f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
					m_creature->SummonCreature(NPC_SHADOW_MAGE, -7486.41f, -4791.55f, 10.54f, 3.26f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);

					if (Creature* pCreature = m_creature->SummonCreature(NPC_SCOFFLAW, -7488.47f, -4800.77f, 9.77f, 2.50f,TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
						DoScriptText(SAY_OOX17_AMBUSH_REPLY,pCreature);

					m_bAreHumansSpawned = true;
				}

                break;
            case 34:
                DoScriptText(SAY_OOX_END, m_creature);
                // Award quest credit
                pPlayer->GroupEventHappens(QUEST_RESCUE_OOX_17TN, m_creature);
                break;
        }
    }

    void Reset() 
	{ 
		if (!HasEscortState(STATE_ESCORT_ESCORTING))
		{
			m_bAreScorpionsSpawned = false;
			m_bAreHumansSpawned = false;
		}
	
	}

    void Aggro(Unit* /*who*/)
    {
        //For an small probability he say something when it aggros
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

CreatureAI* GetAI_npc_oox17tn(Creature* pCreature)
{
    return new npc_oox17tnAI(pCreature);
}

bool QuestAccept_npc_oox17tn(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RESCUE_OOX_17TN)
    {
        DoScriptText(SAY_OOX_START, pCreature);

        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        if (pPlayer->GetTeam() == ALLIANCE)
            pCreature->setFaction(FACTION_ESCORT_A_PASSIVE);

        if (pPlayer->GetTeam() == HORDE)
            pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

        if (npc_oox17tnAI* pEscortAI = dynamic_cast<npc_oox17tnAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
## npc_stone_watcher_of_norgannon
######*/

#define GOSSIP_ITEM_NORGANNON_1     "What function do you serve?"
#define GOSSIP_ITEM_NORGANNON_2     "What are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_3     "Where are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_4     "Excuse me? We've been \"reschedueled for visitations\"? What does that mean?!"
#define GOSSIP_ITEM_NORGANNON_5     "So, what's inside Uldum?"
#define GOSSIP_ITEM_NORGANNON_6     "I will return when i have the Plates of Uldum."

bool GossipHello_npc_stone_watcher_of_norgannon(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(2954) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(1674, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_stone_watcher_of_norgannon(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(1675, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(1676, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(1677, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(1678, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(1679, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(2954);
            break;
    }
    return true;
}

/*####
# npc_tooga
####*/

enum
{
    SAY_TOOG_THIRST             = -1000391,
    SAY_TOOG_WORRIED            = -1000392,
    SAY_TOOG_POST_1             = -1000393,
    SAY_TORT_POST_2             = -1000394,
    SAY_TOOG_POST_3             = -1000395,
    SAY_TORT_POST_4             = -1000396,
    SAY_TOOG_POST_5             = -1000397,
    SAY_TORT_POST_6             = -1000398,

    QUEST_TOOGA                 = 1560,
    NPC_TORTA                   = 6015,

    POINT_ID_TO_WATER           = 1
};

const float m_afToWaterLoc[] = {-7032.664551f, -4906.199219f, -1.606446f};

struct MANGOS_DLL_DECL npc_toogaAI : public FollowerAI
{
    npc_toogaAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    uint32 m_uiCheckSpeechTimer;
    uint32 m_uiPostEventTimer;
    uint32 m_uiPhasePostEvent;

    Unit* pTorta;

    void Reset()
    {
        m_uiCheckSpeechTimer = 2500;
        m_uiPostEventTimer = 1000;
        m_uiPhasePostEvent = 0;

        pTorta = NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE | STATE_FOLLOW_POSTEVENT) && pWho->GetEntry() == NPC_TORTA)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_TOOGA) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_TOOGA, m_creature);
                }

                pTorta = pWho;
                SetFollowComplete(true);
            }
        }
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        FollowerAI::MovementInform(uiMotionType, uiPointId);

        if (uiMotionType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_TO_WATER)
            SetFollowComplete();
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            //we are doing the post-event, or...
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                if (m_uiPostEventTimer < uiDiff)
                {
                    m_uiPostEventTimer = 5000;

                    if (!pTorta || !pTorta->isAlive())
                    {
                        //something happened, so just complete
                        SetFollowComplete();
                        return;
                    }

                    switch(m_uiPhasePostEvent)
                    {
                        case 1:
                            DoScriptText(SAY_TOOG_POST_1, m_creature);
                            break;
                        case 2:
                            DoScriptText(SAY_TORT_POST_2, pTorta);
                            break;
                        case 3:
                            DoScriptText(SAY_TOOG_POST_3, m_creature);
                            break;
                        case 4:
                            DoScriptText(SAY_TORT_POST_4, pTorta);
                            break;
                        case 5:
                            DoScriptText(SAY_TOOG_POST_5, m_creature);
                            break;
                        case 6:
                            DoScriptText(SAY_TORT_POST_6, pTorta);
                            m_creature->GetMotionMaster()->MovePoint(POINT_ID_TO_WATER, m_afToWaterLoc[0], m_afToWaterLoc[1], m_afToWaterLoc[2]);
                            break;
                    }

                    ++m_uiPhasePostEvent;
                }
                else
                    m_uiPostEventTimer -= uiDiff;
            }
            //...we are doing regular speech check
            else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            {
                if (m_uiCheckSpeechTimer < uiDiff)
                {
                    m_uiCheckSpeechTimer = 5000;

                    switch(urand(0, 50))
                    {
                        case 10: DoScriptText(SAY_TOOG_THIRST, m_creature); break;
                        case 25: DoScriptText(SAY_TOOG_WORRIED, m_creature); break;
                    }
                }
                else
                    m_uiCheckSpeechTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tooga(Creature* pCreature)
{
    return new npc_toogaAI(pCreature);
}

bool QuestAccept_npc_tooga(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOOGA)
    {
        if (npc_toogaAI* pToogaAI = dynamic_cast<npc_toogaAI*>(pCreature->AI()))
            pToogaAI->StartFollow(pPlayer, FACTION_ESCORT_N_FRIEND_PASSIVE, pQuest);
    }

    return true;
}

/*######
## npc_yehkinya
######*/

#define GOSSIP_SCROLL     "I'd like to be able to perform the ritual again."

enum eYehkinya
{
    SPELL_CREATE_ITEM_YEHKINYAS_SCROLL    = 12998,
    ITEM_YEHKINYAS_SCROLL                 = 10818,
    QUEST_THE_GOD_HAKKAR                  = 3528,
	QUEST_ID_CONFRONT_YEHKINYA			  = 8181,
	YEHKINYA_SAY_1						  = -1720217,
	YEHKINYA_SAY_2						  = -1720218
};

struct Loc
{
    float x, y, z;
};

static Loc Move[]=
{
    {-6907.f,-4843.f,8.37f},
	{-6909.f,-4849.f,8.29f},
	{-6909.f,-4855.f,14.29f},
	{-6904.f,-4955.f,16.37f}
};

struct MANGOS_DLL_DECL npc_yehkinyaAI : public ScriptedAI
{
    npc_yehkinyaAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro8181;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro8181 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
		m_creature->SetDisplayId(7902);
		m_creature->UpdateModelData();
		m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
	}

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{		
		if (uiQuestId == 8181)
		{
			if (!pPlayerGUID)
				return;

			m_uiPlayerGUID = pPlayerGUID;
			m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
			
			if(Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
				DoScriptText(YEHKINYA_SAY_1, m_creature, pPlayer);
			m_bOutro8181 = true; 
			m_uiSpeechTimer = 3000;
			m_uiSpeechStep = 1;
			
		}
	}

	void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
       if(uiPointId == 3)
		   m_creature->GetMotionMaster()->MovePoint(4, Move[3].x, Move[3].y, Move[3].z, false);
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro8181)							// handle RP at quest end 8181
		{
			if(!m_uiSpeechStep)
				return;
		
			if(m_uiSpeechTimer <= uiDiff)
            {
                switch(m_uiSpeechStep)
                {		
					case 1:
						m_creature->GetMotionMaster()->MovePoint(1, Move[0].x, Move[0].y, Move[0].z);
						m_uiSpeechTimer = 2000;
                        break;
					case 2:
						m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
						m_uiSpeechTimer = 2000;
                        break;
					case 3:						
						m_creature->GetMotionMaster()->MovePoint(2, Move[1].x, Move[1].y, Move[1].z);						
						m_uiSpeechTimer = 3000;
                        break;
                    case 4:
						m_creature->SetDisplayId(10991);
						m_creature->UpdateModelData();
						m_uiSpeechTimer = 5000;
                        break;
					case 5:
						m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
						m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
						m_creature->GetMotionMaster()->MovePoint(3, Move[2].x, Move[2].y, Move[2].z,false);
						m_uiSpeechTimer = 5000;
                        break;
					case 6:
						DoScriptText(YEHKINYA_SAY_2, m_creature, NULL);
						m_creature->ForcedDespawn(10000);
                        m_uiSpeechTimer = 9990;
                        break;
					case 7:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro8181 = false;
						break;
					/*default:
                        m_uiSpeechStep = 0;
                        return;*/
                }
                ++m_uiSpeechStep;
            }
            else
                m_uiSpeechTimer -= uiDiff;
		}

		if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_yehkinya(Creature* pCreature)
{
    return new npc_yehkinyaAI(pCreature);
}

bool OnQuestRewarded_npc_yehkinya(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_CONFRONT_YEHKINYA)
    {
		if (npc_yehkinyaAI* pYehkinyaAI = dynamic_cast<npc_yehkinyaAI*>(pCreature->AI()))
			pYehkinyaAI->StartOutro(pPlayer->GetObjectGuid(), 8181);
	}
	return true;
}

bool GossipHello_npc_yehkinya(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (!pPlayer->HasItemCount(ITEM_YEHKINYAS_SCROLL, 1, true) && pPlayer->GetQuestRewardStatus(QUEST_THE_GOD_HAKKAR))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SCROLL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_yehkinya(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_CREATE_ITEM_YEHKINYAS_SCROLL, false);
    }

    return true;
}

/*######
## go_landmark_treasure
######*/

enum eGold
{
    NPC_BUCCANEER         = 7902,
    NPC_PIRATE            = 7899,
    NPC_SWASHBUCKLER      = 7901,
    QUEST_CUERGOS_GOLD    = 2882,
    GO_TREASURE           = 142194,
    MAX_PIRATE            = 6
};

static float Point[6][3] = 
{
    {-10117.2f,-4083.79f,4.09952f},
    {-10134.6f,-4090.37f,2.61774f},
    {-10148.5f,-4083.61f,2.12749f},
    {-10159.1f,-4063.23f,3.00599f},
    {-10160.4f,-4049.66f,3.04275f},
    {-10156.3f,-4031.97f,3.6349f}
};

bool GOUse_go_landmark_treasure(Player *pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestStatus(QUEST_CUERGOS_GOLD) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 Entry;
        for(uint32 i = 0; i < MAX_PIRATE; ++i)
        {
            switch (urand(0,2))
            {
                case 0:
                    Entry = NPC_PIRATE;
                  break;
                case 1:
                    Entry = NPC_SWASHBUCKLER;
                    break;
                case 2:
                    Entry = NPC_BUCCANEER;
                    break;
            }
            if (Creature* Spawn = pPlayer->SummonCreature(Entry, -10038.2f, -4032.07f, 17.8228f, 3.42121f, TEMPSUMMON_TIMED_DESPAWN, 340000))
                Spawn->GetMotionMaster()->MovePoint(0, Point[i][0], Point[i][1], Point[i][2]);
        }
    }
    return true;
};

/*######
## npc_stone_watcher_of_norgannon
######*/

#define GOSSIP_ITEM_NNARAIN_1     "What function do you serve?"
#define GOSSIP_ITEM_NNARAIN_2     "What are the Plates of Uldum?"
#define GOSSIP_ITEM_NNARAIN_3     "Where are the Plates of Uldum?"
#define GOSSIP_ITEM_NNARAIN_4     "Excuse me? We've been \"reschedueled for visitations\"? What does that mean?!"
#define GOSSIP_ITEM_NNARAIN_5     "So, what's inside Uldum?"
#define GOSSIP_ITEM_NNARAIN_6     "I will return when i have the Plates of Uldum."

bool GossipHello_npc_narain_soothfancy(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    /*if (pPlayer->GetQuestStatus(2954) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	
	pPlayer->SEND_GOSSIP_MENU(1674, pCreature->GetObjectGuid());*/
	if (pPlayer->GetQuestStatus(8620) == QUEST_STATUS_INCOMPLETE)
        pPlayer->SEND_GOSSIP_MENU(11811, pCreature->GetObjectGuid());
    else
        pPlayer->SEND_GOSSIP_MENU(7902, pCreature->GetObjectGuid());

    return true;
}

//bool GossipSelect_npc_narain_soothfancy(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
//{
//    switch(uiAction)
//    {
//        case GOSSIP_ACTION_INFO_DEF:
//            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
//            pPlayer->SEND_GOSSIP_MENU(1675, pCreature->GetObjectGuid());
//            break;
//        case GOSSIP_ACTION_INFO_DEF+1:
//            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
//            pPlayer->SEND_GOSSIP_MENU(1676, pCreature->GetObjectGuid());
//            break;
//        case GOSSIP_ACTION_INFO_DEF+2:
//            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
//            pPlayer->SEND_GOSSIP_MENU(1677, pCreature->GetObjectGuid());
//            break;
//        case GOSSIP_ACTION_INFO_DEF+3:
//            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
//            pPlayer->SEND_GOSSIP_MENU(1678, pCreature->GetObjectGuid());
//            break;
//        case GOSSIP_ACTION_INFO_DEF+4:
//            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
//            pPlayer->SEND_GOSSIP_MENU(1679, pCreature->GetObjectGuid());
//            break;
//        case GOSSIP_ACTION_INFO_DEF+5:
//            pPlayer->CLOSE_GOSSIP_MENU();
//            pPlayer->AreaExploredOrEventHappens(2954);
//            break;
//    }
//    return true;
//}

/*######
## npc_anachronos
######*/

struct MANGOS_DLL_DECL npc_anachronosAI : public ScriptedAI
{
    npc_anachronosAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	bool m_bDidDespawn;

    void Reset()
    {
		m_bDidDespawn = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(HealthBelowPct(21) && !m_bDidDespawn)
		{
			m_creature->MonsterSay("A terrible and costly mistake you have made. It is not my time, mortals.", LANG_UNIVERSAL, NULL);
			m_creature->ForcedDespawn(1000);
			m_bDidDespawn = true;
		}

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_anachronos(Creature* pCreature)
{
    return new npc_anachronosAI(pCreature);
}

void GiveScepterToPlayer(uint32 itemID, Player* player)
{
    ItemPosCountVec dest;
    if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemID, 1) == EQUIP_ERR_OK)
    {
        Item* item = player->StoreNewItem( dest, itemID, true, Item::GenerateItemRandomPropertyId(itemID));
        player->SendNewItem(item, 1, true, false);
    }
    else
        player->GetSession()->SendNotification(LANG_NOT_FREE_TRADE_SLOTS);
}

bool GossipHello_npc_anachronos(Player* pPlayer, Creature* pCreature)
{
    if(pPlayer && pCreature && pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if(pPlayer && pPlayer->GetQuestStatus(8742) != QUEST_STATUS_COMPLETE)
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    else
    {
        if(!pPlayer->HasItemCount(21175, 1, true))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I am ready, Anachronos. Please grant me the Scepter of the Shifting Sands.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(7822, pCreature->GetObjectGuid());
        }
        else
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_anachronos(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if(pPlayer && pCreature)
    {
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                GiveScepterToPlayer(21175, pPlayer);
                pPlayer->CLOSE_GOSSIP_MENU();
                break;
        }
    }

    return true;
}

bool GossipHello_npc_meridith_the_mermaiden(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestRewardStatus(8599))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "That would be wonderful! Thank you, Meridith.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(7917, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(7916, pCreature->GetObjectGuid());


    return true;
}

bool GossipSelect_npc_meridith_the_mermaiden(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pCreature->MonsterSay("Lovely song, isn't it?", LANG_UNIVERSAL, pPlayer);
            pCreature->CastSpell(pPlayer, 25678, false);
            pPlayer->CLOSE_GOSSIP_MENU();
    }

    return true;
}

void AddSC_tanaris()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "mob_aquementas";
    pNewscript->GetAI = &GetAI_mob_aquementas;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_marin_noggenfogger";
    pNewscript->pGossipHello =  &GossipHello_npc_marin_noggenfogger;
    pNewscript->pGossipSelect = &GossipSelect_npc_marin_noggenfogger;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_oox17tn";
    pNewscript->GetAI = &GetAI_npc_oox17tn;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_oox17tn;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_stone_watcher_of_norgannon";
    pNewscript->pGossipHello =  &GossipHello_npc_stone_watcher_of_norgannon;
    pNewscript->pGossipSelect = &GossipSelect_npc_stone_watcher_of_norgannon;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_tooga";
    pNewscript->GetAI = &GetAI_npc_tooga;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_tooga;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_yehkinya";
	pNewscript->GetAI = &GetAI_npc_yehkinya;
	pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_yehkinya;
    pNewscript->pGossipHello =  &GossipHello_npc_yehkinya;
    pNewscript->pGossipSelect = &GossipSelect_npc_yehkinya;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_landmark_treasure";
    pNewscript->pGOUse = &GOUse_go_landmark_treasure;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_anachronos";
	pNewscript->GetAI = &GetAI_npc_anachronos;
    pNewscript->pGossipHello =  &GossipHello_npc_anachronos;
    pNewscript->pGossipSelect = &GossipSelect_npc_anachronos;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_meridith_the_mermaiden";
    pNewscript->pGossipHello = &GossipHello_npc_meridith_the_mermaiden;
    pNewscript->pGossipSelect = &GossipSelect_npc_meridith_the_mermaiden;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_narain_soothfancy";
    pNewscript->pGossipHello = &GossipHello_npc_narain_soothfancy;
    pNewscript->RegisterSelf();	
}
