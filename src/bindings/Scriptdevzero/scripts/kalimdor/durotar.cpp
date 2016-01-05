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
SDName: Durotar
SD%Complete: 100
SDComment: Quest support: 5441
SDCategory: Durotar
EndScriptData */

/* ContentData
npc_lazy_peon
EndContentData */

#include "precompiled.h"

/*######
## npc_lazy_peon
######*/

enum
{
    SAY_PEON_AWAKE_1        = -1000159,
    SAY_PEON_AWAKE_2        = -1000796,

    SPELL_PEON_SLEEP        = 17743,
    SPELL_AWAKEN_PEON       = 19938,

    NPC_SLEEPING_PEON       = 10556,
    GO_LUMBERPILE           = 175784,
    GO_TREE_CHOP_DUMMY = 1757840
};

struct MANGOS_DLL_DECL npc_lazy_peonAI : public ScriptedAI
{
    npc_lazy_peonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    uint8 m_uiWoodRPStep;
    uint32 m_uiWoodRPTimer;
    uint32 m_uiRandomRPTimer;
    bool m_bReverse;
    bool m_bDoingRP;
    
    void Reset()
    {
        m_bReverse = false;
        m_uiWoodRPStep = 1;
        m_uiWoodRPTimer = 0;
        m_creature->CastSpell(m_creature,SPELL_PEON_SLEEP, true);
        m_uiRandomRPTimer = urand((1 * MINUTE * IN_MILLISECONDS), (2 * MINUTE * IN_MILLISECONDS));
    }

    void MoveToGO(int type = 0)
    {        
        if (type == 0)
        {            
            if (GameObject* pLumber = GetClosestGameObjectWithEntry(m_creature, GO_LUMBERPILE, 25.0f))
            {                
                float fX, fY, fZ;
                pLumber->GetContactPoint(m_creature, fX, fY, fZ, CONTACT_DISTANCE);
                if(!m_bReverse)
                    m_creature->GetMotionMaster()->MovePoint(1, fX, fY, fZ);
                else
                    m_creature->GetMotionMaster()->MovePoint(3, fX, fY, fZ);
            }
        }
        else
        {
             if (GameObject* pTree = GetClosestGameObjectWithEntry(m_creature, GO_TREE_CHOP_DUMMY, 30.0f))
            {
                float FX, FY, FZ;
                pTree->GetPosition(FX, FY, FZ);
                m_creature->GetMotionMaster()->MovePoint(2, FX, FY, FZ);
                m_bReverse = true;
            }
        }
    }
    
    void JustReachedHome()
    {
        int r = urand(0,1);
        if(r == 1)
            DoScriptText(SAY_PEON_AWAKE_2, m_creature);
        m_creature->CastSpell(m_creature, SPELL_PEON_SLEEP, true);        
    }
    
    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 1:
                m_uiWoodRPTimer = 1000;
                m_uiWoodRPStep = 1;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiWoodRPTimer && m_bDoingRP)
        {
            if(!m_uiWoodRPStep)
                return;
        
            if(m_uiWoodRPTimer <= uiDiff)
            {
                switch(m_uiWoodRPStep)
                {                   
                    case 1:
                        m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);           
                        m_uiWoodRPTimer = 5000;
                        break;
                    case 2: 
                        // move to the tree
                        MoveToGO(1);
                        m_uiWoodRPTimer = 8000;
                        break;
                    case 3:
                        m_creature->HandleEmoteState(EMOTE_STATE_WORK_CHOPWOOD);
                        m_uiWoodRPTimer = urand(10000, 15000);
                        break;
                    case 4:
                        m_creature->HandleEmoteState(EMOTE_STATE_NONE);      
                        // back to lumber
                        MoveToGO(0);
                        m_uiWoodRPTimer = 8000;
                        break;
                    case 5:
                        m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);                   
                        m_uiWoodRPTimer = 5000;
                        break;
                    case 6:
                        // move home to sleep                       
                        m_creature->GetMotionMaster()->MoveTargetedHome();
                        m_bDoingRP = false;
                        break;
                    /*default:
                        m_uiWoodRPStep = 0;
                        return;*/
                }
                ++m_uiWoodRPStep;
            }
            else
                m_uiWoodRPTimer -= uiDiff;
        }      
        else
        {            
            if (m_uiRandomRPTimer)
            {
                if (m_uiRandomRPTimer <= uiDiff)
                {
                    StartLumbering(m_creature);
                    m_uiRandomRPTimer = urand((1 * MINUTE * IN_MILLISECONDS), (2 * MINUTE * IN_MILLISECONDS));
                }
                else
                    m_uiRandomRPTimer -= uiDiff;
            }
        }
    }  
    
        // Can also be self invoked for random working
    void StartLumbering(Unit* pInvoker)
    {
        m_bDoingRP = true;      
        m_bReverse = false;
        m_creature->RemoveAurasDueToSpell(SPELL_PEON_SLEEP);
        MoveToGO(0);
         if (pInvoker->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_PEON_AWAKE_1, m_creature, pInvoker);
            ((Player*)pInvoker)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
        }                
    }   
};

CreatureAI* GetAI_npc_lazy_peon(Creature* pCreature)
{
    return new npc_lazy_peonAI(pCreature);
}

bool EffectDummyCreature_lazy_peon_awake(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    // always check spellid and effectindex
    if (uiSpellId == SPELL_AWAKEN_PEON && uiEffIndex == EFFECT_INDEX_0)
    {
        if (!pCreatureTarget->HasAura(SPELL_PEON_SLEEP) || pCaster->GetTypeId() != TYPEID_PLAYER || pCreatureTarget->GetEntry() != NPC_SLEEPING_PEON)
            return true;

        if (npc_lazy_peonAI* pPeonAI = dynamic_cast<npc_lazy_peonAI*>(pCreatureTarget->AI()))
            pPeonAI->StartLumbering(pCaster);

        // always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

/*######
## npc_bom_bay
######*/

enum eBomBay
{
	QUEST_ZALAZANE			        =   826,
    GOSSIP_TEXTID_BOM_BAY1			=   3794,
    GOSSIP_TEXTID_BOM_BAY2			=   3795,

	EMOTE_FLEE = -47,

	SPELL_HEX_FROG = 16707,
	SPELL_HEX_SNAKE = 16708,
	SPELL_HEX_CHIKEN = 16709,	
	SPELL_GROW	= 16711,
	//SPELL_SPECIAL_BREW = 16712,			// Drunk state never gets removed, so too risky using it
	SPELL_GHOSTLY	= 16713,
	SPELL_LAUNCH = 16716,
	
	SPELL_VOODOO = 17009,				// cast this, trigger random one of the above
};

struct npc_bom_bayAI : public ScriptedAI
{
    npc_bom_bayAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	bool m_bFleeing;
	bool m_bNoAttack;
	bool m_bLaugh;

	uint32 m_uiNoAttackTimer;
	uint32 m_uiLaughTimer;

    void Reset()
    {
		m_bLaugh = false;
		m_bFleeing = false;
		m_bNoAttack = false;
		m_uiNoAttackTimer = 0;
    }

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_VOODOO && pTarget->GetTypeId() == TYPEID_PLAYER)
		{
			int rand = urand(0,5);
			switch(rand)
			{
				case 0:
					m_creature->CastSpell(pTarget, SPELL_HEX_FROG, true);
					break;
				case 1:
					m_creature->CastSpell(pTarget, SPELL_HEX_SNAKE, true);
					break;
				case 2:
					m_creature->CastSpell(pTarget, SPELL_HEX_CHIKEN, true);
					break;
				case 3:
					m_creature->CastSpell(pTarget, SPELL_LAUNCH, true);
					m_bLaugh = true;
					m_uiLaughTimer = 8000;					
					break;
				case 4:
					m_creature->CastSpell(pTarget, SPELL_GHOSTLY, true);
					break;
				case 5:
					m_creature->CastSpell(pTarget, SPELL_GROW, true);
					break;
			}
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if(m_bLaugh)
			if (m_uiLaughTimer <= uiDiff)
			{
				m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH_NOSHEATHE);
				m_bLaugh = false;
			}
			else
				m_uiLaughTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(HealthBelowPct(15) && !m_bFleeing)
		{
			m_creature->DoFleeToGetAssistance();
			DoScriptText(EMOTE_FLEE, m_creature, NULL);
			m_bFleeing = true;
			m_bNoAttack = true;
			m_uiNoAttackTimer = 8000;
		}

		if (m_uiNoAttackTimer <= uiDiff)				// timer so npc starts to attack after he has fled
			m_bNoAttack = false;
		else
			m_uiNoAttackTimer -= uiDiff;

		DoMeleeAttackIfReady();
    }
};

#define GOSSIP_ITEM_BOMBAY1 "Low spirits"
#define GOSSIP_ITEM_BOMBAY2 "Bad hang nail"
#define GOSSIP_ITEM_BOMBAY3 "Feeling underpowered"
#define GOSSIP_ITEM_BOMBAY4 "Jungle Fever"
#define GOSSIP_ITEM_BOMBAY5 "Uni-brow"
#define GOSSIP_ITEM_BOMBAY6 "Whiplash"
#define GOSSIP_ITEM_BOMBAY7 "I don't want to go back to work"

bool GossipHello_npc_bom_bay(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_ZALAZANE) == QUEST_STATE_COMPLETE)
	{		
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BOMBAY7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BOM_BAY2, pCreature->GetObjectGuid());
	}
	else
		pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_BOM_BAY1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_bom_bay(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
			pCreature->CastSpell(pPlayer, SPELL_VOODOO, false);
            break;
    }
    return true;
}

CreatureAI* GetAI_npc_bom_bay(Creature* pCreature)
{
    return new npc_bom_bayAI(pCreature);
}

void AddSC_durotar()
{
    Script* pNewscript;
/*
    pNewscript = new Script;
    pNewscript->Name = "npc_lazy_peon";
    pNewscript->GetAI = &GetAI_npc_lazy_peon;
    pNewscript->RegisterSelf();*/
    
    pNewscript = new Script;
    pNewscript->Name = "npc_lazy_peon";
    pNewscript->GetAI = &GetAI_npc_lazy_peon;
    pNewscript->pEffectDummyNPC = &EffectDummyCreature_lazy_peon_awake;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_bom_bay";
    pNewscript->GetAI = &GetAI_npc_bom_bay;
	pNewscript->pGossipHello = &GossipHello_npc_bom_bay;
    pNewscript->pGossipSelect = &GossipSelect_npc_bom_bay;
    pNewscript->RegisterSelf();
}
