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

enum LazyPeon
{
    SAY_WORK              = -1000159,

    GO_LUMBERPILE         = 175784,
    QUEST_LAZY_PEONS      = 5441,

    SPELL_BUFF_SLEEP      = 17743,
    SPELL_AWAKEN_PEON     = 19938,

	WORKING_TIME          = 60000
};

struct npc_lazy_peonAI : public ScriptedAI
{
    npc_lazy_peonAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bWorking;
    uint32 m_uiRebuffTimer;
	uint32 m_uiUpdateTimer;

    void Reset()
    {
        m_bWorking = false;
        m_uiRebuffTimer = 0;
		m_uiUpdateTimer = 1000;
		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
    }

	bool IsMoving()
	{
		return m_creature->GetMotionMaster() && (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE
				|| m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE);
	}

    void JustReachedHome()
    {
        //m_creature->LoadCreatureAddon();
		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
    }

	//void MoveInLineOfSight(Unit* U)
	//{
	//	if (!m_bWorking && !m_creature->HasAura(SPELL_BUFF_SLEEP,EFFECT_INDEX_0) && !IsMoving())
	//		m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
	//}

    void MovementInform(uint32 /*uiType*/, uint32 uiPointId)
    {
        if (uiPointId == 1)
            m_bWorking = true;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (m_bWorking || pCaster->GetTypeId() != TYPEID_PLAYER)
            return;

        if (pSpell->Id == SPELL_AWAKEN_PEON && ((Player*)pCaster)->GetQuestStatus(QUEST_LAZY_PEONS) == QUEST_STATUS_INCOMPLETE)
        {
			((Player*)pCaster)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
            DoScriptText(SAY_WORK, m_creature, pCaster);
            m_creature->RemoveAllAuras();
            m_uiRebuffTimer = WORKING_TIME;
            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_LUMBERPILE, 20.0f))
                m_creature->GetMotionMaster()->MovePoint(1, pGo->GetPositionX()-1, pGo->GetPositionY(), pGo->GetPositionZ());
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bWorking)
        {
			m_creature->HandleEmote(173);
			if (m_uiRebuffTimer > WORKING_TIME)
				m_uiRebuffTimer = 10000;
            if (m_uiRebuffTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MoveTargetedHome();
                m_bWorking = false;
                m_uiRebuffTimer = 0;
            }
            else
                m_uiRebuffTimer -= uiDiff;
        }
		else 
		{
			if (m_uiUpdateTimer <= uiDiff)
			{
				m_uiUpdateTimer = 1000;
				if (!m_creature->HasAura(SPELL_BUFF_SLEEP,EFFECT_INDEX_0) && !IsMoving())
					m_creature->CastSpell(m_creature,SPELL_BUFF_SLEEP,false);
			}
			else
				m_uiUpdateTimer -= uiDiff;
		}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lazy_peon(Creature* pCreature)
{
    return new npc_lazy_peonAI(pCreature);
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

    pNewscript = new Script;
    pNewscript->Name = "npc_lazy_peon";
    pNewscript->GetAI = &GetAI_npc_lazy_peon;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_bom_bay";
    pNewscript->GetAI = &GetAI_npc_bom_bay;
	pNewscript->pGossipHello = &GossipHello_npc_bom_bay;
    pNewscript->pGossipSelect = &GossipSelect_npc_bom_bay;
    pNewscript->RegisterSelf();
}
