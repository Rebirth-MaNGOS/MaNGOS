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
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 4285,4287,4288(crystal pylons), 4296, 5088, 6481, 10990, 10991, 10992. Field_Repair_Bot->Teaches spell 22704. Barov_journal->Teaches spell 26089
SDCategory: Game Objects
EndScriptData */

/* ContentData
go_redpaths_shield_and_davils_libram
go_blastenheimer_cannon
go_cat_figurine (the "trap" version of GO, two different exist)
go_crystal_pylon
go_barov_journal
go_field_repair_bot_74A
go_orb_of_command
go_resonite_cask
go_sacred_fire_of_life
go_tablet_of_madness
go_tablet_of_the_seven
go_lard_picnic_basket
EndContentData */

#include "precompiled.h"
#include "Language.h"

/*######
## go_redpaths_shield_and_davils_libram
######*/

enum
{
    SPELL_HAUNTED               = 18347,
    QUEST_HEROES_OF_DARROWSHIRE = 5168
};

bool GOUse_go_redpaths_shield_and_davils_libram(Player* pPlayer, GameObject* /*pGo*/)
{
    pPlayer->CastSpell(pPlayer, SPELL_HAUNTED, true);
    return false;
}

/*######
## go_blastenheimer_cannon
######*/

bool GOUse_go_blastenheimer_cannon(Player *pPlayer, GameObject* /*pGo*/)
{
    pPlayer->CastSpell(pPlayer, 11027, true);
    pPlayer->CastSpell(pPlayer, 24742, true);
    return true;
}

/*######
## go_cat_figurine
######*/

enum
{
    SPELL_SUMMON_GHOST_SABER    = 5968,
};

bool GOUse_go_cat_figurine(Player* pPlayer, GameObject* /*pGo*/)
{
    pPlayer->CastSpell(pPlayer, SPELL_SUMMON_GHOST_SABER, true);
    return false;
}

/*######
## go_crystal_pylons (3x)
######*/

enum eCrystalPylons
{
    GO_THE_NORTHERN_PYLON       = 164955,
    GO_THE_EASTERN_PYLON        = 164957,
    GO_THE_WESTERN_PYLON        = 164956,

    QUEST_THE_NORTHERN_PYLON    = 4285,
    QUEST_THE_EASTERN_PYLON     = 4287,
    QUEST_THE_WESTERN_PYLON     = 4288
};

bool GOUse_go_crystal_pylon(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
    {
        pPlayer->PrepareQuestMenu(pGo->GetObjectGuid());
        pPlayer->SendPreparedQuest(pGo->GetObjectGuid());
    }

    switch(pGo->GetEntry())
    {
        case GO_THE_NORTHERN_PYLON:
            if (pPlayer->GetQuestStatus(QUEST_THE_NORTHERN_PYLON) == QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(QUEST_THE_NORTHERN_PYLON);
            break;
        case GO_THE_EASTERN_PYLON:
            if (pPlayer->GetQuestStatus(QUEST_THE_EASTERN_PYLON) == QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(QUEST_THE_EASTERN_PYLON);
            break;
        case GO_THE_WESTERN_PYLON:
            if (pPlayer->GetQuestStatus(QUEST_THE_WESTERN_PYLON) == QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(QUEST_THE_WESTERN_PYLON);
            break;
    }

    return true;
}

/*######
## go_barov_journal
######*/

enum
{
    SPELL_TAILOR_FELCLOTH_BAG = 26086,
    SPELL_LEARN_FELCLOTH_BAG  = 26095
};

bool GOUse_go_barov_journal(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->HasSkill(SKILL_TAILORING) && pPlayer->GetBaseSkillValue(SKILL_TAILORING) >= 280 && !pPlayer->HasSpell(SPELL_TAILOR_FELCLOTH_BAG))
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_FELCLOTH_BAG, false);

    return true;
}

/*######
## go_field_repair_bot_74A
######*/

enum
{
    SPELL_ENGINEER_FIELD_REPAIR_BOT_74A = 22704,
    SPELL_LEARN_FIELD_REPAIR_BOT_74A    = 22864
};

bool GOUse_go_field_repair_bot_74A(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->HasSkill(SKILL_ENGINEERING) && pPlayer->GetBaseSkillValue(SKILL_ENGINEERING) >= 300 && !pPlayer->HasSpell(SPELL_ENGINEER_FIELD_REPAIR_BOT_74A))
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_FIELD_REPAIR_BOT_74A, false);

    return true;
}

/*######
## go_orb_of_command
######*/

enum
{
    QUEST_BLACKHANDS_COMMAND = 7761,
    SPELL_TELEPORT_TO_BWL    = 23460
};

bool GOUse_go_orb_of_command(Player* pPlayer, GameObject* /*pGo*/)
{
	// Implemented the teleport as a dummy effect since the spell wouldn't work.

   /* if (pPlayer->GetQuestRewardStatus(QUEST_BLACKHANDS_COMMAND))
        pPlayer->CastSpell(pPlayer, SPELL_TELEPORT_TO_BWL, true);*/

	Group* player_group = pPlayer ? pPlayer->GetGroup() : NULL;

	if (pPlayer && player_group && pPlayer->GetQuestRewardStatus(QUEST_BLACKHANDS_COMMAND) && player_group->isRaidGroup())
	{
			pPlayer->TeleportTo(469, -7673.03f, -1106.08f, 396.651f, 0.703353f);
	}

    return true;
}

/*####################
## at_orb_of_command
####################*/

bool AreaTrigger_at_orb_of_command(Player* pPlayer, AreaTriggerEntry const* pAt)
{
	if (pAt->id == 3847)
	{
		Group* player_group = pPlayer ? pPlayer->GetGroup() : NULL;
			
		if (pPlayer)
		{
			Corpse* close_player_corpse =  pPlayer->GetCorpse();

			if (player_group && close_player_corpse && pPlayer->GetQuestRewardStatus(QUEST_BLACKHANDS_COMMAND) && player_group->isRaidGroup() && pPlayer->isDead() && close_player_corpse->GetMapId() == 469)
			{
					pPlayer->ResurrectPlayer(0.5f);
					pPlayer->SpawnCorpseBones();
					pPlayer->TeleportTo(469, -7673.03f, -1106.08f, 396.651f, 0.703353f);
			}
		}
	}
	return false;
}



/*######
## go_resonite_cask
######*/

enum
{
    NPC_GOGGEROC    = 11920
};

bool GOUse_go_resonite_cask(Player* /*pPlayer*/, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pGO->SummonCreature(NPC_GOGGEROC, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);

    return false;
}

/*######
## go_sacred_fire_of_life
######*/

enum
{
    NPC_ARIKARA     = 10882,
};

bool GOUse_go_sacred_fire_of_life(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ARIKARA, -5008.338f, -2118.894f, 83.657f, 0.874f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

    return true;
}

/*######
## go_tablet_of_madness
######*/

enum
{
    SPELL_ALCHEMY_GURUBASHI_MOJO_MADNESS = 24266,
    SPELL_LEARN_GURUBASHI_MOJO_MADNESS   = 24267
};

bool GOUse_go_tablet_of_madness(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->HasSkill(SKILL_ALCHEMY) && pPlayer->GetSkillValue(SKILL_ALCHEMY) >= 300 && !pPlayer->HasSpell(SPELL_ALCHEMY_GURUBASHI_MOJO_MADNESS))
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_GURUBASHI_MOJO_MADNESS, false);

    return true;
}

/*######
## go_tablet_of_the_seven
######*/

enum eTablet
{
    QUEST_TABLET_OF_THE_SEVEN       = 4296,
    SPELL_CREATE_TABLET_TRANSCRIPT  = 15065,
};

/*bool GOGossipHello_go_tablet_of_the_seven(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
        return true;

    if (pPlayer->GetQuestStatus(QUEST_TABLET_OF_THE_SEVEN) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Transcript the Tablet", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    return true;
}

bool GOGossipSelect_go_tablet_of_the_seven(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_TABLET_TRANSCRIPT, true);

    return true;
}*/

bool GOUse_go_tablet_of_the_seven(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
        return true;

    if (pPlayer->GetQuestStatus(QUEST_TABLET_OF_THE_SEVEN) == QUEST_STATUS_INCOMPLETE)
        pPlayer->CastSpell(pPlayer, SPELL_CREATE_TABLET_TRANSCRIPT, true);

    return true;
}

/*######
## go_chromaggus_lever
#####*/


bool GOUse_go_chromaggus_lever(Player* pPlayer, GameObject* pGo)
{
	if(GameObject* chromDoor = GetClosestGameObjectWithEntry(pGo, 179116, 80.0f))
	{
		chromDoor->UseDoorOrButton();
		pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
	}

	if(Creature* chromaggus = GetClosestCreatureWithEntry(pGo, 14020, 80.0f))
		if(pPlayer)
			chromaggus->AI()->AttackStart(pPlayer);

	return false;
}

/*######
## go_draconic_for_dummies_book
#####*/

bool GOUse_go_draconic_for_dummies_book(Player* pPlayer, GameObject* pGo)
{
    if(pPlayer)
    {
        if(pPlayer->IsActiveQuest(8620))/*>HasQuest(8620)*/
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take this book for the good of Azeroth!>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }
        pPlayer->SEND_GOSSIP_MENU(1212, pGo->GetObjectGuid());
	}
	return true;
}

bool pGossipHelloGO_go_draconic_for_dummies_book(Player* pPlayer, GameObject* pGo)
{
    uint32 m_itemId;

    switch(pGo->GetEntry())
    {
        case 180665:
            m_itemId = 21107;
            break;
        case 180666:
            m_itemId = 21106;
            break;
        case 180667:
            m_itemId = 21109;
            break;
    }

    if(pPlayer)
    {
        if(pPlayer->IsActiveQuest(8620) && !pPlayer->HasItemCount(m_itemId, 1, true))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<Take this book for the good of Azeroth!>", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }
        pPlayer->SEND_GOSSIP_MENU(1212, pGo->GetObjectGuid());
	}
	return true;
}

void GiveDummyToPlayer(uint32 itemID, Player* pPlayer)
{
    ItemPosCountVec dest;
    if (pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemID, 1) == EQUIP_ERR_OK)
    {
        Item* item = pPlayer->StoreNewItem( dest, itemID, true, Item::GenerateItemRandomPropertyId(itemID));
        pPlayer->SendNewItem(item, 1, true, false);
    }
    else
    {
        pPlayer->GetSession()->SendNotification("That item is unique.");
        pPlayer->CLOSE_GOSSIP_MENU();
    }
}

bool GossipSelect_go_draconic_for_dummies_book(Player* pPlayer, GameObject* pGo, uint32 /*uiSender*/, uint32 uiAction)
{
    if(pPlayer && pGo)
    {
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                {
                uint32 m_goEntry = pGo->GetEntry();
                switch(m_goEntry)
                    {
                        case 180665:
                            {
                            GiveDummyToPlayer(21107, pPlayer);
                            break;
                            }
                        case 180666:
                            {
                            GiveDummyToPlayer(21106, pPlayer);
                            break;
                            }
                        case 180667:
                            {
                            GiveDummyToPlayer(21109, pPlayer);
                            break;
                            }
                        default:
                            break;
                    }
                }
                
                break;
        }
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

void AddSC_go_scripts()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "go_redpaths_shield_and_davils_libram";
    pNewscript->pGOUse = &GOUse_go_redpaths_shield_and_davils_libram;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_blastenheimer_cannon";
    pNewscript->pGOUse = &GOUse_go_blastenheimer_cannon;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_cat_figurine";
    pNewscript->pGOUse = &GOUse_go_cat_figurine;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_crystal_pylon";
    pNewscript->pGOUse = &GOUse_go_crystal_pylon;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_barov_journal";
    pNewscript->pGOUse = &GOUse_go_barov_journal;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_field_repair_bot_74A";
    pNewscript->pGOUse = &GOUse_go_field_repair_bot_74A;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_orb_of_command";
    pNewscript->pGOUse = &GOUse_go_orb_of_command;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "at_orb_of_command";
    pNewscript->pAreaTrigger = &AreaTrigger_at_orb_of_command;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_resonite_cask";
    pNewscript->pGOUse = &GOUse_go_resonite_cask;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_sacred_fire_of_life";
    pNewscript->pGOUse = &GOUse_go_sacred_fire_of_life;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_tablet_of_madness";
    pNewscript->pGOUse = &GOUse_go_tablet_of_madness;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_tablet_of_the_seven";
    //pNewscript->pGOGossipHello = &GOGossipHello_go_tablet_of_the_seven;
    //pNewscript->pGOGossipSelect = &GOGossipSelect_go_tablet_of_the_seven;
    pNewscript->pGOUse = &GOUse_go_tablet_of_the_seven;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "go_chromaggus_lever";
    pNewscript->pGOUse = &GOUse_go_chromaggus_lever;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_draconic_for_dummies_book";
    pNewscript->pGossipHelloGO = &pGossipHelloGO_go_draconic_for_dummies_book;
    pNewscript->pGossipSelectGO = &GossipSelect_go_draconic_for_dummies_book;
    pNewscript->RegisterSelf();

}
