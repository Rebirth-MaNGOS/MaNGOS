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
}
