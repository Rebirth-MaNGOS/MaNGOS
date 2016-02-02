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

/*####
# npc_grand_mason_marblesten
####*/

enum eMarblesten
{
	MARBLESTEN_SAY_1					 = -1720214,
	MARBLESTEN_SAY_2					 = -1720215,
	MARBLESTEN_SAY_3					 = -1720216,

	QUEST_ID_A_KINGS_TRIBUTE			 = 689,
	GO_MEMORIAL_TO_SULLY_BALLOO			 = 139852
};

struct Loc
{
    float x, y, z;
};

static Loc Move[]=
{
    {-5032.62f,-1015.52f,508.87f},
	{-5030.15f,-1007.26f,505.26f},
	{-5022.60f,-1009.0f,502.20f},
	{-5026.23f,-1014.53f,502.20f},
	{-5027.37f,-1018.53f,502.20f},	// memorial, now backwards
	{-5026.23f,-1014.53f,502.20f},
	{-5022.60f,-1009.0f,502.20f},
	{-5030.15f,-1007.26f,505.26f},
	{-5032.62f,-1015.52f,508.87f}
};

struct MANGOS_DLL_DECL npc_grand_mason_marblestenAI : public ScriptedAI
{
    npc_grand_mason_marblestenAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{ 
		Reset(); 
	}
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro689;

	ObjectGuid m_uiPlayerGUID;

    void Reset()
	{
		m_bOutro689 = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
		m_uiPlayerGUID.Clear();
	}

	void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
	{		
		if (uiQuestId == 689)
		{
			if (!pPlayerGUID)
				return;

			m_uiPlayerGUID = pPlayerGUID;
			m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
			
			if(Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
				DoScriptText(MARBLESTEN_SAY_1, m_creature, pPlayer);
			m_bOutro689 = true; 
			m_uiSpeechTimer = 1000;
			m_uiSpeechStep = 1;
			m_creature->GetMotionMaster()->MovePoint(1, Move[0].x, Move[0].y, Move[0].z);
		}
	}

	void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        switch(uiPointId)
        {
        case 1:
            m_creature->GetMotionMaster()->MovePoint(2, Move[1].x, Move[1].y, Move[1].z);
            break;
        case 2:
            m_creature->GetMotionMaster()->MovePoint(3, Move[2].x, Move[2].y, Move[2].z);
            return;
        case 3:
            m_creature->GetMotionMaster()->MovePoint(4, Move[3].x, Move[3].y, Move[3].z);
            return;
        case 4:
            m_creature->GetMotionMaster()->MovePoint(5, Move[4].x, Move[4].y, Move[4].z);
            break;
		case 6:
            m_creature->GetMotionMaster()->MovePoint(7, Move[7].x, Move[7].y, Move[7].z);
            return;
		case 7:
            m_creature->GetMotionMaster()->MovePoint(8, Move[8].x, Move[8].y, Move[8].z);
            return;
		case 8:
            m_creature->GetMotionMaster()->MoveTargetedHome();
            break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro689)							// handle RP at quest end 689
		{
			if(!m_uiSpeechStep)
				return;
		
			if(m_uiSpeechTimer <= uiDiff)
            {
				Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
				GameObject* pGo = m_creature->GetClosestGameObjectWithEntry(m_creature, GO_MEMORIAL_TO_SULLY_BALLOO, 20.f);
                switch(m_uiSpeechStep)
                {					
                    case 1:
						if (pGo)
                            pGo->SetGoState(GO_STATE_READY);
						m_uiSpeechTimer = 29000;
                        break;
					case 2:
						m_creature->HandleEmoteState(EMOTE_STATE_NONE);
                        if (pGo)
                            pGo->SetGoState(GO_STATE_ACTIVE);
						m_uiSpeechTimer = 3000;
                        break;
					case 3:
						DoScriptText(MARBLESTEN_SAY_2, m_creature, NULL);
                        m_uiSpeechTimer = 6000;
                        break;
					case 4:
						DoScriptText(MARBLESTEN_SAY_3, m_creature, pPlayer);
						m_creature->SetFacingTo(1.29f);
                        m_uiSpeechTimer = 4000;
						break;
					case 5:
						m_creature->GetMotionMaster()->MovePoint(6, Move[5].x, Move[5].y, Move[5].z);
                        m_uiSpeechTimer = 10000;
						break;
					case 6:
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
						m_bOutro689 = false;
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

CreatureAI* GetAI_npc_grand_mason_marblesten(Creature* pCreature)
{
    return new npc_grand_mason_marblestenAI(pCreature);
}

bool OnQuestRewarded_npc_grand_mason_marblesten(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_A_KINGS_TRIBUTE)
    {
		if (npc_grand_mason_marblestenAI* pMarblestenAI = dynamic_cast<npc_grand_mason_marblestenAI*>(pCreature->AI()))
			pMarblestenAI->StartOutro(pPlayer->GetObjectGuid(), 689);
	}
	return true;
}

/*####
# npc_jordan_stilwell
####*/

enum eStilwell
{
    QUEST_ID_TEST_OF_RIGHTEOUS               = 1806,
    
    STILWELL_SAY_1                                           = -1720234,
    STILWELL_SAY_2                                           = -1720235,
    
    SPELL_FORGE_VERIGANS_FIST                   =  8912,
    GO_VERIGANS_FIST                                      = 102413
};

struct MANGOS_DLL_DECL npc_jordan_stilwellAI : public ScriptedAI
{
    npc_jordan_stilwellAI(Creature* pCreature) : ScriptedAI(pCreature) 
    { 
        Reset(); 
    }
    
    uint8 m_uiSpeechStep;
    uint32 m_uiSpeechTimer;
    bool m_bOutro1806;

    ObjectGuid m_uiPlayerGUID;

    void Reset()
    {
        m_bOutro1806 = false;
        m_uiSpeechStep = 1;
        m_uiSpeechTimer = 0;
        m_uiPlayerGUID.Clear();
    }

    void StartOutro(ObjectGuid pPlayerGUID, uint32 uiQuestId = 0)
    {       
        if (uiQuestId == 1806)
        {
            if (!pPlayerGUID)
                return;

            m_uiPlayerGUID = pPlayerGUID;
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                        
            m_bOutro1806 = true; 
            m_uiSpeechTimer = 3000;
            m_uiSpeechStep = 1;
            m_creature->GetMotionMaster()->MovePoint(1, -5094.48f, -786.08f, 495.20f);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiSpeechTimer && m_bOutro1806)
        {
            if(!m_uiSpeechStep)
                return;
        
            if(m_uiSpeechTimer <= uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID);
                switch(m_uiSpeechStep)
                {                   
                    case 1:
                        // turn + say
                        DoScriptText(STILWELL_SAY_1, m_creature, pPlayer);
                        m_creature->SetFacingTo(2.98f);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 2:
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 3:
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 4:                        
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 3000;
                        break;
                    case 5:        
                        // Shouldn't be able to interact with the mace
                        if (GameObject* pFist = m_creature->SummonGameObject(GO_VERIGANS_FIST, 18000, -5095.60f, -785.54f, 496.28f,1.31f, GO_STATE_READY, 0.f))
                            pFist->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
                        
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 3000;
                        break;
                     case 6:        
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 3000;
                        break;
                     case 7:        
                        m_creature->CastSpell(m_creature, SPELL_FORGE_VERIGANS_FIST, false);
                        m_uiSpeechTimer = 5000;
                        break;
                     case 8:
                         // bow and say 
                         m_creature->MonsterSay("I thank the Light for this blessing. May it be used for justice and and to defend all good creatures.", LANG_COMMON, NULL);
                         m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                         m_uiSpeechTimer = 7000;
                        break;
                     case 9:
                         // walk home
                         m_creature->GetMotionMaster()->MovePoint(2, -5089.28f, -782.95f, 495.279f);
                         m_uiSpeechTimer = 3000;
                         break;
                     case 10:
                         // turn and say
                         DoScriptText(STILWELL_SAY_2, m_creature, pPlayer);
                         m_creature->GetMotionMaster()->MoveTargetedHome();
                     case 11:
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                        m_bOutro1806 = false;
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

CreatureAI* GetAI_npc_jordan_stilwell(Creature* pCreature)
{
    return new npc_jordan_stilwellAI(pCreature);
}

bool QuestAccept_npc_jordan_stilwell(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ID_TEST_OF_RIGHTEOUS)
    {
        if (npc_jordan_stilwellAI* pStilwellAI = dynamic_cast<npc_jordan_stilwellAI*>(pCreature->AI()))
            pStilwellAI->StartOutro(pPlayer->GetObjectGuid(), 1806);
    }
    return true;
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

	pNewscript = new Script;
    pNewscript->Name = "npc_grand_mason_marblesten";
    pNewscript->GetAI = &GetAI_npc_grand_mason_marblesten;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_grand_mason_marblesten;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_jordan_stilwell";
    pNewscript->GetAI = &GetAI_npc_jordan_stilwell;
    pNewscript->pQuestAcceptNPC = &QuestAccept_npc_jordan_stilwell;
    pNewscript->RegisterSelf();
}
