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
SDName: Npc_Professions
SD%Complete: 80
SDComment: Provides learn/unlearn/relearn-options for professions
SDCategory: NPCs
EndScriptData */

#include "precompiled.h"

/*
A few notes for future developement:
- A full implementation of gossip for GO's is required. They must have the same scripting capabilities as creatures. Basically,
there is no difference here (except that default text is chosen with `gameobject_template`.`data3` (for GO type2, different dataN for a few others)
- It's possible blacksmithing still require some tweaks and adjustments due to the way we _have_ to use reputation.
*/

/*
-- UPDATE `gameobject_template` SET `ScriptName` = 'go_soothsaying_for_dummies' WHERE `entry` = 177226;
*/

/*###
# gossip item and box texts
###*/

#define GOSSIP_LEARN_WEAPON         "Please teach me how to become a Weaponsmith"
#define GOSSIP_UNLEARN_WEAPON       "I wish to unlearn the art of Weaponsmithing"
#define GOSSIP_LEARN_ARMOR          "Please teach me how to become a Armorsmith"
#define GOSSIP_UNLEARN_ARMOR        "I wish to unlearn the art of Armorsmithing"

#define GOSSIP_LEARN                "Please teach me blacksmith specialty"
#define GOSSIP_UNLEARN_SMITH_SPEC   "I wish to unlearn my blacksmith specialty"
#define BOX_UNLEARN_ARMORORWEAPON   "Do you really want to unlearn your blacksmith specialty and lose all associated recipes?"

#define GOSSIP_LEARN_DRAGON         "I am absolutely certain that I want to learn dragonscale leatherworking"
#define GOSSIP_UNLEARN_DRAGON       "I wish to unlearn Dragonscale Leatherworking"
#define GOSSIP_LEARN_ELEMENTAL      "I am absolutely certain that I want to learn elemental leatherworking"
#define GOSSIP_UNLEARN_ELEMENTAL    "I wish to unlearn Elemental Leatherworking"
#define GOSSIP_LEARN_TRIBAL         "I am absolutely certain that I want to learn tribal leatherworking"
#define GOSSIP_UNLEARN_TRIBAL       "I wish to unlearn Tribal Leatherworking"

#define BOX_UNLEARN_LEATHER_SPEC    "Do you really want to unlearn your leatherworking specialty and lose all associated recipes?"

#define GOSSIP_LEARN_GOBLIN         "I am absolutely certain that I want to learn Goblin engineering"
#define GOSSIP_UNLEARN_GOBLIN       "I wish to unlearn Goblin engineering"
#define GOSSIP_LEARN_GNOMISH        "I am absolutely certain that I want to learn Gnomish engineering"
#define GOSSIP_UNLEARN_GNOMISH      "I wish to unlearn Gnomish engineering"

#define BOX_UNLEARN_ENGINEER_SPEC   "Do you really want to unlearn your engineering specialty and lose all associated recipes?"

/*###
# spells defines
###*/

enum Spells
{
    S_WEAPON                = 9787,
    S_ARMOR                 = 9788,
    S_LEARN_WEAPON          = 9789,
    S_LEARN_ARMOR           = 9790,
    
    S_REP_ARMOR             = 17451,
    S_REP_WEAPON            = 17452,
    REP_ARMOR               = 46,
    REP_WEAPON              = 289,
    
    S_DRAGON                = 10656,
    S_ELEMENTAL             = 10658,
    S_TRIBAL                = 10660,
    S_LEARN_DRAGON          = 10657,
    S_LEARN_ELEMENTAL       = 10659,
    S_LEARN_TRIBAL          = 10661,
    
    S_GOBLIN                = 20222,
    S_GNOMISH               = 20219,
    S_LEARN_GOBLIN          = 20221,
    S_LEARN_GNOMISH         = 20220,
};

/*###
# formulas to calculate unlearning cost
###*/

int32 GetUnlearnCost(Player* pPlayer)
{
    uint32 level = pPlayer->getLevel();
    if (level < 50)
        return 100000;
    else if (level < 55)
        return 250000;
    else
        return 500000;
}

void ProfessionUnlearnSpells(Player* pPlayer, uint32 uiType)
{
    switch (uiType)
    {
        case S_WEAPON:                                       // S_UNLEARN_WEAPON
            pPlayer->removeSpell(S_WEAPON);
            pPlayer->removeSpell(10007);                     // Phantom Blade
            pPlayer->removeSpell(10003);                     // The Shatterer
            pPlayer->removeSpell(10015);                     // Truesilver Champion
            break;
        case S_ARMOR:                                        // S_UNLEARN_ARMOR
            pPlayer->removeSpell(S_ARMOR);
            pPlayer->removeSpell(9974);                      // Truesilver Breastplate
            pPlayer->removeSpell(9954);                      // Truesilver Gauntlets
            break;
        case S_DRAGON:                                       // S_UNLEARN_DRAGON
            pPlayer->removeSpell(S_DRAGON);
            pPlayer->removeSpell(24654);                     // Blue Dragonscale Leggings
            pPlayer->removeSpell(10650);                     // Dragonscale Breastplate
            pPlayer->removeSpell(10619);                     // Dragonscale Gauntlets
            pPlayer->removeSpell(24655);                     // Green Dragonscale Gauntlets
            break;
        case S_ELEMENTAL:                                    // S_UNLEARN_ELEMENTAL
            pPlayer->removeSpell(S_ELEMENTAL);
            pPlayer->removeSpell(10630);                     // Gauntlets of the Sea
            pPlayer->removeSpell(10632);                     // Helm of Fire
            break;
        case S_TRIBAL:                                       // S_UNLEARN_TRIBAL
            pPlayer->removeSpell(S_TRIBAL);
            pPlayer->removeSpell(10647);                     // Feathered Breastplate
            pPlayer->removeSpell(10621);                     // Wolfshead Helm
            break;
        case S_GOBLIN:                                       // S_GOBLIN
            pPlayer->removeSpell(S_GOBLIN);
            pPlayer->removeSpell(12715);                     // Goblin Rocket Fuel Recipe
            pPlayer->removeSpell(15628);                     // Pet Bombling
            pPlayer->removeSpell(13240);                     // The Mortar: Reloaded
            pPlayer->removeSpell(12760);                     // Goblin Sapper Charge
            pPlayer->removeSpell(12718);                     // Goblin Construction Helmet
            pPlayer->removeSpell(12717);                     // Goblin Mining Helmet
            pPlayer->removeSpell(12716);                     // Goblin Mortar
            pPlayer->removeSpell(8895);                      // Goblin Rocket Boots
            pPlayer->removeSpell(12755);                     // Goblin Bomb Dispenser
            pPlayer->removeSpell(12754);                     // The Big One
            pPlayer->removeSpell(12908);                     // Goblin Dragon Gun
            pPlayer->removeSpell(12758);                     // Goblin Rocket Helmet
            pPlayer->removeSpell(23486);                     // Dimensional Ripper - Everlook
            pPlayer->removeSpell(23078);                     // Goblin Jumper Cables XL
            break;
        case S_GNOMISH:                                      // S_GNOMISH
            pPlayer->removeSpell(S_GNOMISH);
            pPlayer->removeSpell(12895);                     // Inlaid Mithril Cylinder Plans
            pPlayer->removeSpell(15633);                     // Lil' Smoky
            pPlayer->removeSpell(12899);                     // Gnomish Shrink Ray
            pPlayer->removeSpell(12897);                     // Gnomish Goggles
            pPlayer->removeSpell(12902);                     // Gnomish Net-o-Matic Projector
            pPlayer->removeSpell(12903);                     // Gnomish Harm Prevention Belt
            pPlayer->removeSpell(12905);                     // Gnomish Rocket Boots
            pPlayer->removeSpell(12906);                     // Gnomish Battle Chicken
            pPlayer->removeSpell(12907);                     // Gnomish Mind Control Cap
            pPlayer->removeSpell(12759);                     // Gnomish Death Ray
            pPlayer->removeSpell(23129);                     // World Enlarger
            pPlayer->removeSpell(23489);                     // Ultrasafe Transporter - Gadgetzan
            pPlayer->removeSpell(23096);                     // Gnomish Alarm-o-Bot
            break;
    }
}

/*###
# start menues blacksmith
###*/

bool GossipHello_npc_prof_blacksmith(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->HasSkill(SKILL_BLACKSMITHING) && pPlayer->GetBaseSkillValue(SKILL_BLACKSMITHING) >= 225 && pCreature->isTrainer())
    {
        switch (pCreature->GetEntry())
        {
            case 11146:                                     //Ironus Coldsteel
            case 11178:                                     //Borgosh Corebender
                if (pPlayer->HasSpell(S_WEAPON))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
            case 5164:                                      //Grumnus Steelshaper
            case 11177:                                     //Okothos Ironrager
                if (pPlayer->HasSpell(S_ARMOR))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
        }
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_prof_blacksmith(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*###
# start menues Engineering
###*/

bool GossipHello_npc_prof_engineering(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->HasSkill(SKILL_ENGINEERING) && pPlayer->GetBaseSkillValue(SKILL_ENGINEERING) >= 200 && pCreature->isTrainer())
    {
        switch (pCreature->GetEntry())
        {
            case 8126:                                     // Nixx Sprocketspring
			case 8738:									   // Vazario Linkgrease
                if (pPlayer->HasSpell(S_GOBLIN))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
            case 7944:                                     // Tinkmaster Overspark
            case 7406:                                     // Oglethorpe Obnoticus
                if (pPlayer->HasSpell(S_GNOMISH))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
        }
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_prof_engineering(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*###
# engineering trinkets
###*/

enum
{
    NPC_ZAP                     = 14742,
    NPC_JHORDY                  = 14743,

    SPELL_TO_EVERLOOK           = 23486,
    SPELL_TO_GADGET             = 23489,
    SPELL_LEARN_TO_EVERLOOK     = 23490,
    SPELL_LEARN_TO_GADGET       = 23491,

    ITEM_GNOMISH_CARD           = 10790,
    ITEM_GOBLIN_CARD            = 10791
};

#define GOSSIP_ITEM_ZAP         "[PH] Unknown"
#define GOSSIP_ITEM_JHORDY      "I must build a beacon for this marvelous device!"

bool GossipHello_npc_engineering_tele_trinket(Player* pPlayer, Creature* pCreature)
{
    uint32 uiNpcTextId = 0;
    std::string strGossipItem;
    bool bCanLearn = false;

    if (pPlayer->HasSkill(SKILL_ENGINEERING))
    {
        switch(pCreature->GetEntry())
        {
            case NPC_ZAP:
                uiNpcTextId = 7249;
                if (pPlayer->GetBaseSkillValue(SKILL_ENGINEERING) >= 260 && pPlayer->HasSpell(S_GOBLIN))
                {
                    if (!pPlayer->HasSpell(SPELL_TO_EVERLOOK))
                    {
                        bCanLearn = true;
                        strGossipItem = GOSSIP_ITEM_ZAP;
                    }
                    else if (pPlayer->HasSpell(SPELL_TO_EVERLOOK))
                        uiNpcTextId = 0;
                }
                break;
            case NPC_JHORDY:
                uiNpcTextId = 7251;
                if (pPlayer->GetBaseSkillValue(SKILL_ENGINEERING) >= 260 && pPlayer->HasSpell(S_GNOMISH))
                {
                    if (!pPlayer->HasSpell(SPELL_TO_GADGET))
                    {
                        bCanLearn = true;
                        strGossipItem = GOSSIP_ITEM_JHORDY;
                    }
                    else if (pPlayer->HasSpell(SPELL_TO_GADGET))
                        uiNpcTextId = 7252;
                }
                break;
        }
    }

    if (bCanLearn)
    {
        if (pPlayer->HasItemCount(ITEM_GOBLIN_CARD, 1) || pPlayer->HasItemCount(ITEM_GNOMISH_CARD, 1))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, strGossipItem, pCreature->GetEntry(), GOSSIP_ACTION_INFO_DEF + 1);
    }

    pPlayer->SEND_GOSSIP_MENU(uiNpcTextId ? uiNpcTextId : pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_engineering_tele_trinket(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CLOSE_GOSSIP_MENU();

    if (uiSender != pCreature->GetEntry())
        return true;

    switch(uiSender)
    {
        case NPC_ZAP:
            pPlayer->CastSpell(pPlayer, SPELL_LEARN_TO_EVERLOOK, false);
            break;
        case NPC_JHORDY:
            pPlayer->CastSpell(pPlayer, SPELL_LEARN_TO_GADGET, false);
            break;
    }
    return true;
}

/*###
# start menues leatherworking
###*/

bool GossipHello_npc_prof_leather(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->HasSkill(SKILL_LEATHERWORKING) && pPlayer->GetBaseSkillValue(SKILL_LEATHERWORKING) >= 250 && pCreature->isTrainer())
    {
        switch (pCreature->GetEntry())
        {
            case 7866:                                      //Peter Galen
            case 7867:                                      //Thorkaf Dragoneye
                if (pPlayer->HasSpell(S_DRAGON))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
            case 7868:                                      //Sarah Tanner
            case 7869:                                      //Brumn Winterhoof
                if (pPlayer->HasSpell(S_ELEMENTAL))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
            case 7870:                                      //Caryssia Moonhunter
            case 7871:                                      //Se'Jib
                if (pPlayer->HasSpell(S_TRIBAL))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
                break;
        }
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_prof_leather(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_TRAIN:
            pPlayer->SEND_TRAINERLIST(pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*###
# start menues for GO (engineering and leatherworking)
###*/

/*bool GOUse_go_soothsaying_for_dummies(Player* pPlayer, GameObject* pGo)
{
    pPlayer->PlayerTalkClass->GetGossipMenu()->AddMenuItem(0,GOSSIP_LEARN_DRAGON, GOSSIP_SENDER_INFO, GOSSIP_ACTION_INFO_DEF, "", 0);

    pPlayer->SEND_GOSSIP_MENU(5584, pGo->GetObjectGuid());

    return true;
}*/

/*###
# AddSC
###*/

void AddSC_npc_professions()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "npc_prof_blacksmith";
    pNewscript->pGossipHello =  &GossipHello_npc_prof_blacksmith;
    pNewscript->pGossipSelect = &GossipSelect_npc_prof_blacksmith;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_prof_engineering";
    pNewscript->pGossipHello =  &GossipHello_npc_prof_engineering;
    pNewscript->pGossipSelect = &GossipSelect_npc_prof_engineering;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_engineering_tele_trinket";
    pNewscript->pGossipHello =  &GossipHello_npc_engineering_tele_trinket;
    pNewscript->pGossipSelect = &GossipSelect_npc_engineering_tele_trinket;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_prof_leather";
    pNewscript->pGossipHello =  &GossipHello_npc_prof_leather;
    pNewscript->pGossipSelect = &GossipSelect_npc_prof_leather;
    pNewscript->RegisterSelf();

    /*pNewscript = new Script;
    pNewscript->Name = "go_soothsaying_for_dummies";
    pNewscript->pGOUse =  &GOUse_go_soothsaying_for_dummies;
    //pNewscript->pGossipSelect = &GossipSelect_go_soothsaying_for_dummies;
    pNewscript->RegisterSelf();*/
}
