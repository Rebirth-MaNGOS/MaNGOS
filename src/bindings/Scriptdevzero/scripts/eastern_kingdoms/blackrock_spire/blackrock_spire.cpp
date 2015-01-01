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
SDName: Blackrock_Spire
SD%Complete: 100
SDComment:
SDCategory: Blackrock Spire
EndScriptData */

/* ContentData
at_backrock_spire
go_father_flame
go_challenge_to_urok
mob_blackhand_dragon_handler
npc_grayhoof_valthalak_trigger
npc_finkle_einhorn
npc_awbee
EndContentData */

#include "precompiled.h"
#include "blackrock_spire.h"

/*######
## at_backrock_spire
######*/

enum eTriggers
{
    ITEM_UNADORNED_SEAL_OF_ASCENSION    = 12219
};

bool AreaTrigger_at_backrock_spire(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    instance_blackrock_spire* m_pInstance = (instance_blackrock_spire*)pPlayer->GetInstanceData();

    if (!m_pInstance)
        return true;

    switch (pAt->id)
    {
        case AT_UBRS_PREQUEST_1:
        case AT_UBRS_PREQUEST_2:
        case AT_UBRS_PREQUEST_3:
            if (pPlayer->HasItemCount(ITEM_UNADORNED_SEAL_OF_ASCENSION, 1))
            {
                Creature* pInfiltrator = m_pInstance->GetSingleCreatureFromStorage(NPC_SCARSHIELD_INFILTRATOR);
                if (pInfiltrator && pInfiltrator->isAlive() && pInfiltrator->GetEntry() == NPC_SCARSHIELD_INFILTRATOR)
                    pInfiltrator->UpdateEntry(NPC_VAELAN);
            }
            break;
        case AT_UBRS_ENTRY:
            if (m_pInstance->GetData(TYPE_REND_BLACKHAND) != DONE)
                m_pInstance->SortDragonspineMobs();
            break;
        case AT_STADIUM:
            if (m_pInstance->GetData(TYPE_REND_BLACKHAND) == NOT_STARTED)
                m_pInstance->SetData(TYPE_REND_BLACKHAND, IN_PROGRESS);
            break;
        default:
            break;
    }

    return true;
}

/*######
## go_father_flame
######*/

bool GOUse_go_father_flame(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_blackrock_spire* m_pInstance = (instance_blackrock_spire*)pGo->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_ROOKERY) == DONE ||
        m_pInstance->GetData(TYPE_ROOKERY) == IN_PROGRESS ||
        m_pInstance->GetData(TYPE_DRAKKISATH) == DONE) // when Drakkisath is dead, event doesn't start
            return false;

        m_pInstance->SetData(TYPE_ROOKERY, IN_PROGRESS);
    }

    return false;
}

/*######
## go_challenge_to_urok
######*/

bool GOUse_go_challenge_to_urok(Player* /*pPlayer*/, GameObject* pGo)
{
    if (instance_blackrock_spire* m_pInstance = (instance_blackrock_spire*)pGo->GetInstanceData())
        if (m_pInstance->GetData(TYPE_UROK_DOOMHOWL) == NOT_STARTED)
            m_pInstance->SetData(TYPE_UROK_DOOMHOWL, SPECIAL);

    if (Creature* pAdd = GetClosestCreatureWithEntry(pGo, NPC_UROK_ENFORCER | NPC_UROK_OGRE_MAGUS, 20.0f))
    {
        if (pAdd->getVictim())
            pAdd->getVictim()->CombatStopWithPets(true);

        pAdd->CombatStopWithPets(true);
        pAdd->SetDeathState(JUST_DIED);
    }
    pGo->SummonLinkedTrapIfAny();
    pGo->SetRespawnTime(5);
    return false;
}

/*######
## mob_blackhand_dragon_handler
######*/

enum eBlackhandDragonHandler
{
    SPELL_MEND_DRAGON       = 16637
};

struct MANGOS_DLL_DECL mob_blackhand_dragon_handlerAI : public ScriptedAI
{
    mob_blackhand_dragon_handlerAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
	}

    uint32 m_uiMendDragonTimer;

    void Reset()
    {
        m_uiMendDragonTimer = urand(4000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mend Dragon
        if (m_uiMendDragonTimer <= uiDiff)
        {
            std::list<Creature*> m_lDragons;
            GetCreatureListWithEntryInGrid(m_lDragons, m_creature, NPC_CHROMATIC_DRAGONSPAWN, 40.0f);

            if (!m_lDragons.empty())
            {
                for (std::list<Creature*>::iterator itr = m_lDragons.begin(); itr != m_lDragons.end(); ++itr)
                {
                    if ((*itr) && (*itr)->isAlive() && (*itr)->GetHealthPercent() < 85.0f)
                    {
                        DoCastSpellIfCan((*itr), SPELL_MEND_DRAGON, CAST_INTERRUPT_PREVIOUS);
                        break;
                    }
                }
            }

            m_uiMendDragonTimer = urand(8000, 20000);
        }
        else
            m_uiMendDragonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_blackhand_dragon_handler(Creature* pCreature)
{
    return new mob_blackhand_dragon_handlerAI (pCreature);
}

/*######
## npc_grayhoof_valthalak_trigger
######*/

struct MANGOS_DLL_DECL npc_grayhoof_valthalak_triggerAI : public Scripted_NoMovementAI
{
    npc_grayhoof_valthalak_triggerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (instance_blackrock_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_blackrock_spire* m_pInstance;

    void Reset()
    {

    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || !m_pInstance)
            return;

        GameObject* pBeckoning = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_BECKONING, 40.0f);
        GameObject* pInvocation = GetClosestGameObjectWithEntry(m_creature, GO_BRAZIER_OF_INVOCATION, 40.0f);

        if (!pBeckoning && !pInvocation)
            return;
        
        if (pBeckoning)
            pBeckoning->RemoveFromWorld();

        if (pInvocation)
            pInvocation->RemoveFromWorld();

        switch (m_creature->GetEntry())
        {
            case NPC_MOR_GRAYHOOF_TRIGGER:
                if (m_pInstance->GetData(TYPE_MOR_GRAYHOOF) == NOT_STARTED)
                {
                    m_pInstance->SetData(TYPE_MOR_GRAYHOOF, IN_PROGRESS);
                    if (Creature* pMor = DoSpawnCreature(NPC_MOR_GRAYHOOF, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
                        pMor->AI()->AttackStart(pWho);
                }
                break;
            case NPC_LORD_VALTHALAK_TRIGGER:
                if (m_pInstance->GetData(TYPE_LORD_VALTHALAK) == NOT_STARTED)
                {
                    m_pInstance->SetData(TYPE_LORD_VALTHALAK, IN_PROGRESS);
                    if (Creature* pValthalak = DoSpawnCreature(NPC_LORD_VALTHALAK, 0, 0, 0, 0, TEMPSUMMON_DEAD_DESPAWN, 30000))
                        pValthalak->AI()->AttackStart(pWho);
                }
                break;
        }
    }
};

CreatureAI* GetAI_npc_grayhoof_valthalak_trigger(Creature* pCreature)
{
    return new npc_grayhoof_valthalak_triggerAI(pCreature);
}

/*######
## npc_finkle_einhorn
######*/

// TODO: Move to DB.
#define GOSSIP_ITEM_FINKLE_1 "How the hell did you get in there to begin with?"
#define GOSSIP_ITEM_FINKLE_2 "I'm all ears."
#define GOSSIP_ITEM_FINKLE_3 "So let me get this straight. You were swimming, nay, doing backstrokes in molten lava with this... this lava suit of yours? Yes? And out of nowhere, this here beast swallowed you whole?"
#define GOSSIP_ITEM_FINKLE_4 "I must know something: How did you go to the bathroom with that thing on?"
#define GOSSIP_ITEM_FINKLE_5 "Oh?"
#define GOSSIP_ITEM_FINKLE_6 "I'll consider that the next time I run into one of these things."
#define GOSSIP_ITEM_FINKLE_7 "To Malyfous, I assume."

bool GossipHello_npc_finkle_einhorn(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(3659, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_finkle_einhorn(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN)
    {
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			    pPlayer->SEND_GOSSIP_MENU(3660, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			    pPlayer->SEND_GOSSIP_MENU(3661, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			    pPlayer->SEND_GOSSIP_MENU(3662, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			    pPlayer->SEND_GOSSIP_MENU(3663, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			    pPlayer->SEND_GOSSIP_MENU(3664, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FINKLE_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			    pPlayer->SEND_GOSSIP_MENU(3665, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                if (pCreature->isQuestGiver())
                    pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
                pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
                break;
        }
    }

    return true;
}

/*######
## npc_awbee
######*/

// TODO: Move to DB.
#define QUEST_THE_MATRON_PROTECTORATE 5160
#define GOSSIP_ITEM_AWBEE_1 "You will be ok, Awbee. Your assailants have been terminated."
#define GOSSIP_ITEM_AWBEE_2 "Continue please..."
#define GOSSIP_ITEM_AWBEE_3 "Horrifying."
#define GOSSIP_ITEM_AWBEE_4 "Absolutely."

bool GossipHello_npc_awbee(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_MATRON_PROTECTORATE) == QUEST_STATUS_NONE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AWBEE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(3797, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_awbee(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN)
    {
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AWBEE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			    pPlayer->SEND_GOSSIP_MENU(3798, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AWBEE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			    pPlayer->SEND_GOSSIP_MENU(3799, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AWBEE_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			    pPlayer->SEND_GOSSIP_MENU(3800, pCreature->GetObjectGuid());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                if (pCreature->isQuestGiver())
                    pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
                pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
                break;
        }
    }

    return true;
}

void AddSC_blackrock_spire()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "at_backrock_spire";
    pNewscript->pAreaTrigger = &AreaTrigger_at_backrock_spire;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_father_flame";
    pNewscript->pGOUse = &GOUse_go_father_flame;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_challenge_to_urok";
    pNewscript->pGOUse = &GOUse_go_challenge_to_urok;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "mob_blackhand_dragon_handler";
    pNewscript->GetAI = &GetAI_mob_blackhand_dragon_handler;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_grayhoof_valthalak_trigger";
    pNewscript->GetAI = &GetAI_npc_grayhoof_valthalak_trigger;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_finkle_einhorn";
    pNewscript->pGossipHello = &GossipHello_npc_finkle_einhorn;
    pNewscript->pGossipSelect = &GossipSelect_npc_finkle_einhorn;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_awbee";
    pNewscript->pGossipHello = &GossipHello_npc_awbee;
    pNewscript->pGossipSelect = &GossipSelect_npc_awbee;
    pNewscript->RegisterSelf();
}
