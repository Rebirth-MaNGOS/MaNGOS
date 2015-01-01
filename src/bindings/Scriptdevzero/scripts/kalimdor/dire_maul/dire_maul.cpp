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
SDName: Dire_Maul
SD%Complete: 100
SDComment:
SDCategory: Dire Maul
EndScriptData */ 

/* ContentData
go_skeletal_remains_of_kariel_winthalus
go_broken_trap
go_prison_pylon
npc_ironbark_the_redeemed
npc_knot_thimblejack
npc_lorekeeper_lydros
npc_mizzle_the_crafty
npc_shendralar_ancient
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "dire_maul.h"

/*####
## go_skeletal_remains_of_kariel_winthalus
####*/

enum eSkeletalRemainsOfKarielWinthalus
{
    QUEST_ELVEN_LEGENDS_A   = 7482,
    QUEST_ELVEN_LEGENDS_H   = 7481
};

bool GOUse_go_skeletal_remains_of_kariel_winthalus(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestStatus(QUEST_ELVEN_LEGENDS_A) == QUEST_STATUS_INCOMPLETE ||
        pPlayer->GetQuestStatus(QUEST_ELVEN_LEGENDS_H) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(pPlayer->GetTeam() == ALLIANCE ? QUEST_ELVEN_LEGENDS_A : QUEST_ELVEN_LEGENDS_H);

    return true;
}

/*####
# go_broken_trap
####*/

enum eBrokenTrap
{
    QUEST_A_BROKEN_TRAP   = 1193,
    SPELL_TRAP            = 27753,
};

bool QuestRewarded_go_broken_trap(Player* pPlayer, GameObject* pGo, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_BROKEN_TRAP)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        //pPlayer->CastSpell(pPlayer, SPELL_TRAP, true);
		pGo->SetGoState(GO_STATE_ACTIVE);
		pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND + GO_FLAG_NO_INTERACT);
    }
    return true;
}

/*####
# go_prison_pylon
####*/

bool GOUse_go_prison_pylon(Player* /*pPlayer*/, GameObject* pGo)
{
    if (GetClosestCreatureWithEntry(pGo, NPC_MANA_REMNANT, 50.0f) || GetClosestCreatureWithEntry(pGo, NPC_ARCANE_ABERRATION, 50.0f))
        return true;

    instance_dire_maul* m_pInstance = (instance_dire_maul*)pGo->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_BARRIER) != DONE && pGo->GetGoState() != GO_STATE_ACTIVE)
    {
        m_pInstance->SetData(TYPE_BARRIER, IN_PROGRESS);
        pGo->SetGoState(GO_STATE_ACTIVE);
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
    }

    return true;
}

/*####
# go_warpwood_pod
####*/

bool GOUse_go_warpwood_pod(Player* /*pPlayer*/, GameObject* pGo)
{
	switch (urand(0,3))
	{
		case 0:
			for (int i = 0; i < 6; i++)
				pGo->SummonCreature(13022, pGo->GetPositionX()+frand(0,2), pGo->GetPositionY()+frand(0,2), pGo->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
			return true;
		case 1:
			if (Creature* pSummon = pGo->SummonCreature(13022, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
				pSummon->CastSpell(pSummon,24840,false);
			return true;
		case 2:
			return true;
		case 3:
			return false;
	}
	return false;
}

/*####
# npc_ironbark_the_redeemed
####*/

enum eIronbark
{
    GOSSIP_IRONBARK         = 6695,
    SAY_START               = -1429006,
    SAY_END                 = -1429007
};

#define GOSSIP_PLAYER "Thank you, Ironbark. We are ready for you to open the door."

struct MANGOS_DLL_DECL npc_ironbark_the_redeemedAI : public npc_escortAI
{
    npc_ironbark_the_redeemedAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();
        Reset();
    }

    instance_dire_maul* m_pInstance;

    void Reset()
    {
        m_creature->UpdateEntry(NPC_IRONBARK_THE_REDEEMED);
    }

    void WaypointReached(uint32 uiPointID)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        if (uiPointID == 4)
        {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            DoScriptText(SAY_END, m_creature);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_IRONBARK_THE_REDEEMED, DONE);
        }
    }
};

CreatureAI* GetAI_npc_ironbark_the_redeemed(Creature* pCreature)
{
    return new npc_ironbark_the_redeemedAI(pCreature);
}

bool GossipHello_npc_ironbark_the_redeemed(Player* pPlayer, Creature* pCreature)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_ZEVRIM_THORNHOOF) == DONE && m_pInstance->GetData(TYPE_IRONBARK_THE_REDEEMED) != DONE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PLAYER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

	pPlayer->SEND_GOSSIP_MENU(GOSSIP_IRONBARK, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_ironbark_the_redeemed(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_ironbark_the_redeemedAI* pEscortAI = dynamic_cast<npc_ironbark_the_redeemedAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, NULL, true, true);

        DoScriptText(SAY_START, pCreature);
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*####
# npc_knot_thimblejack
####*/

#define GOSSIP_KNOT_OGRE_SUIT "Teach me the ogre suit!"
#define GOSSIP_KNOT_ADVICE "Why should I bother fixing the trap? Why not just eliminate the guard the old fashioned way?"

enum eKnot
{
    GOSSIP_KNOT_THIMBLEJACK_1       = 6875,
    GOSSIP_KNOT_THIMBLEJACK_3       = 6883,
    GOSSIP_KNOT_THIMBLEJACK_2       = 6795,    

    QUEST_FREE_KNOT                 = 5525,
    QUEST_FREE_KNOT_REPEATABLE      = 7429,
    QUEST_THE_GORDOK_OGRE_SUIT      = 5518,

    ITEM_GORDOK_SHACKLE_KEY         = 18250,

    SPELL_BUFF_SUIT                 = 22736,
    SPELL_T_OGRE_SUIT               = 22813,
    SPELL_T_TEACHING_OGRE_SUIT      = 22814,
    SPELL_L_OGRE_SUIT               = 22815,
    SPELL_L_TEACHING_OGRE_SUIT      = 22816
};

bool GossipHello_npc_knot_thimblejack(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestRewardStatus(QUEST_THE_GORDOK_OGRE_SUIT))
    {
        if (pPlayer->GetBaseSkillValue(SKILL_TAILORING) >= 275 && !pPlayer->HasSpell(SPELL_T_OGRE_SUIT))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_KNOT_OGRE_SUIT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetBaseSkillValue(SKILL_LEATHERWORKING) >= 275 && !pPlayer->HasSpell(SPELL_L_OGRE_SUIT))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_KNOT_OGRE_SUIT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    if (pPlayer->HasItemCount(ITEM_GORDOK_SHACKLE_KEY, 1, true))
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_KNOT_THIMBLEJACK_1, pCreature->GetObjectGuid());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, GOSSIP_KNOT_ADVICE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_KNOT_THIMBLEJACK_2, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_knot_thimblejack(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pCreature->CastSpell(pPlayer, SPELL_T_TEACHING_OGRE_SUIT, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pCreature->CastSpell(pPlayer, SPELL_L_TEACHING_OGRE_SUIT, true);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_KNOT_THIMBLEJACK_3, pCreature->GetObjectGuid());
            break;
    }

    return true;
}

bool QuestRewarded_npc_knot_thimblejack(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    instance_dire_maul* m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();

    if (pQuest->GetQuestId() == QUEST_FREE_KNOT || pQuest->GetQuestId() == QUEST_FREE_KNOT_REPEATABLE)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KNOT_THIMBLEJACK, DONE);

        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->RemoveFromWorld();
    }
    return true;
}

/*######
## npc_lorekeeper_lydros
######*/

enum eLorekeeperLydros
{
    QUEST_FORORS_COMPENDIUM           = 7507,
    QUEST_THE_FORGING_OF_QUELSERRAR_1 = 7508,
    ITEM_A_DULL_AND_FLAT_ELVEN_BLADE  = 18513,
    
    // Gossip ID
    GOSSIP_LYDROS_1                   = 30017,
    GOSSIP_LYDROS_2                   = 30018,
    GOSSIP_LYDROS_3                   = 30019,
    GOSSIP_LYDROS_4                   = 30020,
    GOSSIP_LYDROS_5                   = 30021,
    GOSSIP_LYDROS_6                   = 30022,
    
    GOSSIP_LYDROS_PLAYER_0            = -3429000,
    GOSSIP_LYDROS_PLAYER_1            = -3429001,
    GOSSIP_LYDROS_PLAYER_2            = -3429002,
    GOSSIP_LYDROS_PLAYER_5            = -3429003,
    GOSSIP_LYDROS_PLAYER_6            = -3429004,
};

bool GossipHello_npc_lorekeeper_lydros(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestRewardStatus(QUEST_FORORS_COMPENDIUM) && !pPlayer->GetQuestRewardStatus(QUEST_THE_FORGING_OF_QUELSERRAR_1))
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_1, pCreature->GetObjectGuid());
        return true;
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_lorekeeper_lydros(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        /*case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_1, pCreature->GetObjectGuid());
            break;*/
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LYDROS_PLAYER_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_LYDROS_6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            pPlayer->CLOSE_GOSSIP_MENU();
            if (Item* pItem = ((Player*)pPlayer)->StoreNewItemInInventorySlot(ITEM_A_DULL_AND_FLAT_ELVEN_BLADE, 1))
                ((Player*)pPlayer)->SendNewItem(pItem, 1, true, false);
            break;
    }

    return true;
}

/*####
# npc_mizzle_the_crafty
####*/

enum eMizzle
{
    GOSSIP_MIZLLE_1         = 6882,
    GOSSIP_MIZLLE_2         = 6895,
};

bool GossipHello_npc_mizzle_the_crafty(Player* pPlayer, Creature* pCreature)
{
    instance_dire_maul* m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();

    if (m_pInstance && m_pInstance->GetData(TYPE_KING_GORDOK) == DONE && m_pInstance->GetData(TYPE_TRIBUTE_RUN) != DONE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "continue...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_MIZLLE_1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_mizzle_the_crafty(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    instance_dire_maul* m_pInstance = (instance_dire_maul*)pCreature->GetInstanceData();

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm ready to collect my tribute!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_MIZLLE_2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            if (m_pInstance && m_pInstance->GetData(TYPE_TRIBUTE_RUN) != DONE)
                m_pInstance->SetData(TYPE_TRIBUTE_RUN, DONE);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
    }
    return true;
}

/*####
# npc_shendralar_ancient
####*/

enum eAncient
{
    GOSSIP_SPIRIT_1         = 6896,
    GOSSIP_SPIRIT_2         = 6897,
    GOSSIP_SPIRIT_3         = 6898,
    GOSSIP_SPIRIT_4         = 6899,
    GOSSIP_SPIRIT_5         = 6900,
    GOSSIP_SPIRIT_6         = 6901,
    GOSSIP_SPIRIT_7         = 6902,

    GOSSIP_SPIRIT_PLAYER_1  = -3429005,
    GOSSIP_SPIRIT_PLAYER_2  = -3429006,
    GOSSIP_SPIRIT_PLAYER_3  = -3429007,
    GOSSIP_SPIRIT_PLAYER_4  = -3429008,
    GOSSIP_SPIRIT_PLAYER_5  = -3429009,
    GOSSIP_SPIRIT_PLAYER_6  = -3429010,
    GOSSIP_SPIRIT_PLAYER_7  = -3429011,
};

bool GossipHello_npc_shendralar_ancient(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_1, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_shendralar_ancient(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SPIRIT_PLAYER_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_SPIRIT_7, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            if (pCreature->isQuestGiver())
                pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
            break;
    }
    return true;
}

void AddSC_dire_maul()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_skeletal_remains_of_kariel_winthalus";
    pNewscript->pGOUse = &GOUse_go_skeletal_remains_of_kariel_winthalus;
    pNewscript->RegisterSelf();
        
    pNewscript = new Script;
    pNewscript->Name = "go_broken_trap";
    pNewscript->pQuestRewardedGO = &QuestRewarded_go_broken_trap;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_prison_pylon";
    pNewscript->pGOUse = &GOUse_go_prison_pylon;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "go_warpwood_pod";
    pNewscript->pGOUse = &GOUse_go_warpwood_pod;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_ironbark_the_redeemed";
    pNewscript->GetAI = &GetAI_npc_ironbark_the_redeemed;
    pNewscript->pGossipHello = &GossipHello_npc_ironbark_the_redeemed;
    pNewscript->pGossipSelect = &GossipSelect_npc_ironbark_the_redeemed;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_knot_thimblejack";
    pNewscript->pGossipHello = &GossipHello_npc_knot_thimblejack;
    pNewscript->pGossipSelect = &GossipSelect_npc_knot_thimblejack;
    pNewscript->pQuestRewardedNPC = &QuestRewarded_npc_knot_thimblejack;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_lorekeeper_lydros";
    pNewscript->pGossipHello = &GossipHello_npc_lorekeeper_lydros;
    pNewscript->pGossipSelect = &GossipSelect_npc_lorekeeper_lydros;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_mizzle_the_crafty";
    pNewscript->pGossipHello = &GossipHello_npc_mizzle_the_crafty;
    pNewscript->pGossipSelect = &GossipSelect_npc_mizzle_the_crafty;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_shendralar_ancient";
    pNewscript->pGossipHello = &GossipHello_npc_shendralar_ancient;
    pNewscript->pGossipSelect = &GossipSelect_npc_shendralar_ancient;
    pNewscript->RegisterSelf();
}
