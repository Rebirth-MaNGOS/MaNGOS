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
SDName: Moonglade
SD%Complete: 100
SDComment: Quest support: 30, 272, 5929, 5930, 8447. Special Flight Paths for Druid class.
SDCategory: Moonglade
EndScriptData */

/* ContentData
npc_bunthen_plainswind
npc_clintar_dw_spirit
npc_great_bear_spirit
npc_silva_filnaveth
npc_keeper_remulos
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"

/*######
## npc_bunthen_plainswind
######*/

enum
{
    QUEST_SEA_LION_HORDE        = 30,
    QUEST_SEA_LION_ALLY         = 272,
    TAXI_PATH_ID_ALLY           = 315,
    TAXI_PATH_ID_HORDE          = 316
};

#define GOSSIP_ITEM_THUNDER     "I'd like to fly to Thunder Bluff."
#define GOSSIP_ITEM_AQ_END      "Do you know where I can find Half Pendant of Aquatic Endurance?"

bool GossipHello_npc_bunthen_plainswind(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->getClass() != CLASS_DRUID)
        pPlayer->SEND_GOSSIP_MENU(4916, pCreature->GetObjectGuid());
    else if (pPlayer->GetTeam() != HORDE)
    {
        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_ALLY) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_END, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        pPlayer->SEND_GOSSIP_MENU(4917, pCreature->GetObjectGuid());
    }
    else if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == HORDE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THUNDER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_HORDE) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_END, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        pPlayer->SEND_GOSSIP_MENU(4918, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_bunthen_plainswind(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CLOSE_GOSSIP_MENU();

            if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == HORDE)
                pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_HORDE);

            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->SEND_GOSSIP_MENU(5373, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(5376, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## npc_great_bear_spirit
######*/

#define GOSSIP_BEAR1 "What do you represent, spirit?"
#define GOSSIP_BEAR2 "I seek to understand the importance of strength of the body."
#define GOSSIP_BEAR3 "I seek to understand the importance of strength of the heart."
#define GOSSIP_BEAR4 "I have heard your words, Great Bear Spirit, and I understand. I now seek your blessings to fully learn the way of the Claw."

bool GossipHello_npc_great_bear_spirit(Player* pPlayer, Creature* pCreature)
{
    //ally or horde quest
    if (pPlayer->GetQuestStatus(5929) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(5930) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(4719, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(4718, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_great_bear_spirit(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(4721, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(4733, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(4734, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(4735, pCreature->GetObjectGuid());
            if (pPlayer->GetQuestStatus(5929)==QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(5929);
            if (pPlayer->GetQuestStatus(5930)==QUEST_STATUS_INCOMPLETE)
                pPlayer->AreaExploredOrEventHappens(5930);
            break;
    }
    return true;
}

/*######
## npc_silva_filnaveth
######*/

#define GOSSIP_ITEM_RUTHERAN    "I'd like to fly to Rut'theran Village."
#define GOSSIP_ITEM_AQ_AGI      "Do you know where I can find Half Pendant of Aquatic Agility?"

bool GossipHello_npc_silva_filnaveth(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->getClass() != CLASS_DRUID)
        pPlayer->SEND_GOSSIP_MENU(4913, pCreature->GetObjectGuid());
    else if (pPlayer->GetTeam() != ALLIANCE)
    {
        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_HORDE) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_AGI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        pPlayer->SEND_GOSSIP_MENU(4915, pCreature->GetObjectGuid());
    }
    else if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == ALLIANCE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTHERAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_ALLY) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_AGI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        pPlayer->SEND_GOSSIP_MENU(4914, pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_silva_filnaveth(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->CLOSE_GOSSIP_MENU();

            if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == ALLIANCE)
                pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_ALLY);

            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->SEND_GOSSIP_MENU(5374, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->SEND_GOSSIP_MENU(5375, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## npc_keeper_remulos
######*/

enum eKeeperRemulos
{
    SPELL_CONJURE_DREAM_RIFT        = 25813,
    SPELL_HEALING_TOUCH             = 23381,
    SPELL_REGROWTH                  = 20665,
    SPELL_REJUVENATION              = 20664,
    SPELL_STARFIRE                  = 21668,
    SPELL_THROW_NIGHTMARE_OBJECT    = 25004,
    SPELL_TRANQUILITY               = 25817,

    SPELL_MALFURION_SHADE           = 24999,
    SPELL_LEVITATE                  = 1706,
    
    QUEST_WAKING_LEGENDS            = 8447,

    NPC_MALFURION_STORMRAGE         = 15362,

    SAY_REMULOS_1                   = -1000653,     // escort start
    SAY_REMULOS_2                   = -1000654,
    SAY_REMULOS_3                   = -1000655,
    SAY_MALFURION_1                 = -1000656,
    SAY_REMULOS_4                   = -1000657,
    SAY_MALFURION_2                 = -1000658,
    SAY_REMULOS_5                   = -1000659,
    SAY_MALFURION_3                 = -1000660,
    SAY_REMULOS_6                   = -1000661,
    SAY_MALFURION_4                 = -1000662,
    SAY_MALFURION_5                 = -1000663,
    SAY_REMULOS_7                   = -1000664,
    SAY_REMULOS_8                   = -1000665,
};

struct MANGOS_DLL_DECL npc_keeper_remulosAI : public npc_escortAI
{
    npc_keeper_remulosAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint32 m_uiConjureDreamRiftTimer;
    uint32 m_uiHealignTouchTimer;
    uint32 m_uiRegrowthTimer;
    uint32 m_uiRejuvenationTimer;
    uint32 m_uiStarfireTimer;
    uint32 m_uiThrowNightmareObjectTimer;
    uint32 m_uiTranquilityTimer;

    uint8  m_uiEventPhase;
    uint32 m_uiEventTimer;

    uint8  m_uiLastReachedWPoint;

    ObjectGuid m_uiMalfurionGUID;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) || HasEscortState(STATE_ESCORT_PAUSED))
            return;

        m_uiConjureDreamRiftTimer = 0;
        m_uiHealignTouchTimer = 0;
        m_uiRegrowthTimer = 0;
        m_uiRejuvenationTimer = 0;
        m_uiStarfireTimer = 0;
        m_uiThrowNightmareObjectTimer = 0;
        m_uiTranquilityTimer = 0;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;

        m_uiLastReachedWPoint = 0;

		m_uiMalfurionGUID.Clear();
    }

    void WaypointReached(uint32 uiPointId)
    {
        m_uiLastReachedWPoint = uiPointId;

        if (uiPointId == 7)
        {
            SetEscortPaused(true);
            m_uiEventPhase = 1;
            m_uiEventTimer = 2000;
        }
        else if (uiPointId == 17)
        {
            if (GetPlayerForEscort())
                GetPlayerForEscort()->GroupEventHappens(QUEST_WAKING_LEGENDS, m_creature);
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (!GetPlayerForEscort())
                return;

            // Declaration
            Creature* pMalfurion = NULL;

            if (m_uiLastReachedWPoint == 7 && m_uiEventPhase > 3 && m_uiEventPhase < 14)
            {
                pMalfurion = m_creature->GetMap()->GetCreature(m_uiMalfurionGUID);
                if (!pMalfurion)
                {
                    error_log("SD0: Moonglad: Keeper Remulos escort unable to select pointer of Malfurion Stormrage.");
                    return;
                }
            }

            if (m_uiEventTimer <= uiDiff)
            {
                switch(m_uiEventPhase)
                {
                    case 1:
                        DoScriptText(SAY_REMULOS_2, m_creature, GetPlayerForEscort());
                        m_uiEventTimer = 3000;
                        break;
                    case 2:
                        DoCastSpellIfCan(m_creature, SPELL_THROW_NIGHTMARE_OBJECT);
                        m_uiEventTimer = 7000;
                        break;
                    case 3:
                        if (Creature* pSummon = m_creature->SummonCreature(NPC_MALFURION_STORMRAGE, 7729.68f, -2314.69f, 453.67f, 0.63f, TEMPSUMMON_TIMED_DESPAWN, 78000+1000))
                        {
                            m_uiMalfurionGUID = pSummon->GetObjectGuid();
                            // Dark visual across the character
                            pSummon->CastSpell(pSummon, SPELL_MALFURION_SHADE, false);
                        }
                        m_uiEventTimer = 1000;
                        break;
                    case 4:
                        // Remulos says: Malfuron!
                        pMalfurion->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        DoScriptText(SAY_REMULOS_3, m_creature);
                        m_uiEventTimer = 4000;
                        break;
                    case 5:
                        DoScriptText(SAY_MALFURION_1, pMalfurion);
                        m_uiEventTimer = 5000;
                        break;
                    case 6:
                        DoScriptText(SAY_REMULOS_4, m_creature);
                        m_uiEventTimer = 7000;
                        break;
                    case 7:
                        DoScriptText(SAY_MALFURION_2, pMalfurion);
                        m_uiEventTimer = 11000;
                        break;
                    case 8:
                        DoScriptText(SAY_REMULOS_5, m_creature);
                        m_uiEventTimer = 6000;
                        break;
                    case 9:
                        DoScriptText(SAY_MALFURION_3, pMalfurion);
                        m_uiEventTimer = 6000;
                        break;
                    case 10:
                        DoScriptText(SAY_REMULOS_6, m_creature);
                        m_uiEventTimer = 10000;
                        break;
                    case 11:
                        DoScriptText(SAY_MALFURION_4, pMalfurion);
                        m_uiEventTimer = 20000;
                        break;
                    case 12:
                        DoScriptText(SAY_MALFURION_5, pMalfurion);
                        m_uiEventTimer = 4000;
                        break;
                    case 13:
                        DoScriptText(SAY_REMULOS_7, m_creature);
                        m_uiEventTimer = 4000;
                        break;
                    case 14:
                        DoScriptText(SAY_REMULOS_8, m_creature);
                        m_uiEventTimer = 3000;
                        break;
                    case 15:
                        m_uiEventPhase = 0;
                        m_uiEventTimer = 0;
                        SetEscortPaused(false);
                        break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_keeper_remulos(Creature* pCreature)
{
    return new npc_keeper_remulosAI(pCreature);
}

bool QuestAccept_npc_keeper_remulos(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WAKING_LEGENDS)
        if (npc_keeper_remulosAI* pEscortAI = dynamic_cast<npc_keeper_remulosAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_REMULOS_1, pCreature, pPlayer);
            pEscortAI->Start(true, pPlayer, pQuest, true);
        }
    return true;
}

void AddSC_moonglade()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_bunthen_plainswind";
    pNewScript->pGossipHello =  &GossipHello_npc_bunthen_plainswind;
    pNewScript->pGossipSelect = &GossipSelect_npc_bunthen_plainswind;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_great_bear_spirit";
    pNewScript->pGossipHello =  &GossipHello_npc_great_bear_spirit;
    pNewScript->pGossipSelect = &GossipSelect_npc_great_bear_spirit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_silva_filnaveth";
    pNewScript->pGossipHello =  &GossipHello_npc_silva_filnaveth;
    pNewScript->pGossipSelect = &GossipSelect_npc_silva_filnaveth;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_keeper_remulos";
    pNewScript->GetAI = &GetAI_npc_keeper_remulos;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_keeper_remulos;
    pNewScript->RegisterSelf();
}
