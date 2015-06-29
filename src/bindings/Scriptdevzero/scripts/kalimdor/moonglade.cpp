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

	SPELL_LONGSHAKE = 24203,
    SPELL_SHORT_SHAKE = 24204,

    SPELL_MALFURION_SHADE           = 24999,
    SPELL_SPAWN	                    = 17321,
    
    QUEST_WAKING_LEGENDS            = 8447,
    QUEST_THE_NIGHTMARE_MANIFESTS   = 8736,

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

    void SummonedCreatureDespawn(Creature* /*pSummoned*/)
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->UpdateVisibilityAndView();
    }

    void SummonedCreatureJustDied(Creature* /*pSummoned*/)
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->UpdateVisibilityAndView();
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (!GetPlayerForEscort())
                return;

            // Declaration
            Creature* pMalfurion = NULL;

            if (m_uiLastReachedWPoint == 7 && m_uiEventPhase > 4 && m_uiEventPhase < 17)
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
                        m_uiEventTimer = 3000;
                        break;
					case 3:
						// Shake the ground
						DoCast(m_creature, SPELL_LONGSHAKE, true);
						m_uiEventTimer = 7000;
						break;
                    case 4:
                        if (Creature* pSummon = m_creature->SummonCreature(NPC_MALFURION_STORMRAGE, 7729.68f, -2314.69f, 453.67f, 0.63f, TEMPSUMMON_TIMED_DESPAWN, 78000+1000))
                        {
                            m_uiMalfurionGUID = pSummon->GetObjectGuid();
							// keep him up in the air
							pSummon->SendMonsterMove(pSummon->GetPositionX(),pSummon->GetPositionY(),pSummon->GetPositionZ()+2.0f, SPLINETYPE_NORMAL, SPLINEFLAG_FLYING, 1000);
                            // Dark visual across the character
                            pSummon->CastSpell(pSummon, SPELL_MALFURION_SHADE, false);
							pSummon->CastSpell(pSummon, SPELL_SPAWN, true);
                        }						
                        m_uiEventTimer = 100;
                        break;
					case 5:
						if (pMalfurion)
							pMalfurion->SetFacingTo(0.63f);							
						m_uiEventTimer = 1000;
                        break;
					case 6:
						if (pMalfurion)
							pMalfurion->HandleEmote(EMOTE_ONESHOT_ROAR);
						m_uiEventTimer = 2000;
                        break;
                    case 7:
                        // Remulos says: Malfuron!                        
                        DoScriptText(SAY_REMULOS_3, m_creature);
                        m_uiEventTimer = 4000;
                        break;
                    case 8:
                        DoScriptText(SAY_MALFURION_1, pMalfurion);
                        m_uiEventTimer = 5000;
                        break;
                    case 9:
                        DoScriptText(SAY_REMULOS_4, m_creature);
                        m_uiEventTimer = 7000;
                        break;
					case 10:
                        DoScriptText(SAY_MALFURION_2, pMalfurion);
                        m_uiEventTimer = 11000;
                        break;
                    case 11:
                        DoScriptText(SAY_REMULOS_5, m_creature);
                        m_uiEventTimer = 6000;
                        break;
                    case 12:
                        DoScriptText(SAY_MALFURION_3, pMalfurion);
						// Shake the ground
						DoCast(m_creature, SPELL_SHORT_SHAKE, true);
                        m_uiEventTimer = 6000;
                        break;
                    case 13:
                        DoScriptText(SAY_REMULOS_6, m_creature);
                        m_uiEventTimer = 10000;
                        break;
                    case 14:
                        DoScriptText(SAY_MALFURION_4, pMalfurion);
                        m_uiEventTimer = 20000;
                        break;
                    case 15:
                        DoScriptText(SAY_MALFURION_5, pMalfurion);
                        m_uiEventTimer = 4000;
                        break;
                    case 16:
                        DoScriptText(SAY_REMULOS_7, m_creature);
                        m_uiEventTimer = 4000;
                        break;
                    case 17:
                        DoScriptText(SAY_REMULOS_8, m_creature);
                        m_uiEventTimer = 3000;
                        break;
                    case 18:
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

/*##################
## BOSS ERANIKUS
#############*/

enum eranikus
{
    NIGHTMARE_PHANTASM = 15629,
    NPC_REMULOS        = 11890,
    SPELL_LEVEL_UP      = 24312,
    BOSS_ERANIKUS       = 15491,
    SPELL_SHADOWBOLT_VOLLEY = 27646,
    SPELL_MASS_HEAL = 25839,
    NPC_TYRANDE = 15633,
    NPC_MOON_PRIESTESS = 15634,

   ERANIKUS_AQ_YELL_1 = -1720130,
   ERANIKUS_AQ_YELL_2 = -1720131,
   ERANIKUS_AQ_YELL_3 = -1720132,
   ERANIKUS_AQ_YELL_4 = -1720133,
   ERANIKUS_AQ_YELL_5 = -1720134,
   ERANIKUS_AQ_YELL_6 = -1720135,
   ERANIKUS_AQ_YELL_7 = -1720136,
   ERANIKUS_AQ_YELL_8 = -1720137,
   ERANIKUS_AQ_YELL_10 = -1720138,
   ERANIKUS_AQ_YELL_11 = -1720139,

   ERANIKUS_AQ_SAYL_1 = -1720140,
   ERANIKUS_AQ_SAYL_2 = -1720141,
   ERANIKUS_AQ_SAYL_3 = -1720142,
   ERANIKUS_AQ_SAYL_4 = -1720143,

   TYRANDE_AQ_YELL_1 = -1720145,
   TYRANDE_AQ_YELL_2 = -1720147,
   TYRANDE_AQ_YELL_3 = -1720148,
   TYRANDE_AQ_YELL_4 = -1720150,

   TYRANDE_AQ_SAY_1 = -1720146,
   TYRANDE_AQ_SAY_2 = -1720149,

};

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ;
};

static const SpawnLocation aEranikusShades[7] =
{
    {7883.7f, -2581.3f, 486.95f},                       // Shade spawn location 1
    {7888.2f, -2560.0f, 486.92f},                       // Shade spawn location 2
    {7896.87f, -2573.236f, 487.75f},                    // Shade spawn location 3
    {7885.69f, -2569.0f, 486.96f},                      // Shade spawn location 4
    {7909.58f, -2559.56f, 487.73f},                     // Shade spawn location 5
    {7924.96f, -2573.28f, 499.63f},                     // Hover spot
    {7899.9f, -2565.54f, 487.89f},                      // Eranikus landing spot                  
};

struct MANGOS_DLL_DECL boss_eranikus_tyrant_of_the_dream : public ScriptedAI
{
    boss_eranikus_tyrant_of_the_dream(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        start_moving = false;
        Reset();
    }
    
    bool start_moving;
    bool tyrande_summoned;
    bool isFriendly;
    bool firstSummon;
    uint32 m_uiYellCount;
    uint32 m_uiLanding_timer;
    uint32 m_uilandEmoteTimer;
    uint32 m_uiPhantasmCounter;
    uint32 m_uiSummonTimer;
    uint32 m_yellTimer;
    uint32 m_endRpTimer;
    uint32 m_uiSayCount;
    std::vector<Creature*> m_vecPhantasmList;
    Creature *tyrande;

    void Reset()
    {
        tyrande_summoned = false;
        isFriendly = false;
        firstSummon = true;
        m_uiYellCount = 0;
        m_uiLanding_timer = 0;
        m_uilandEmoteTimer = 0;
        m_uiPhantasmCounter = 0;
        m_uiSummonTimer = 0;
        m_yellTimer = 0;
        m_endRpTimer = 0;
        m_uiSayCount = 0;

        if(start_moving)
        {
            m_creature->ForcedDespawn();
            m_vecPhantasmList.clear();
        }
    }

    void MoveToLand()
    {
        start_moving = true;
        m_creature->MonsterMove(aEranikusShades[5].m_fX, aEranikusShades[5].m_fY, aEranikusShades[5].m_fZ, 6500);
        m_uiSummonTimer = 6500;
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            EmpowerPhantasms();
        }
    }

    void EmpowerPhantasms()
    {
        for(Creature* phant : m_vecPhantasmList)
        {
            if(phant != nullptr && phant->isAlive())
            {
                phant->AI()->DoCastSpellIfCan(phant, SPELL_LEVEL_UP, CAST_TRIGGERED);
                phant->SetLevel(phant->getLevel() + 1);
                phant->GenericTextEmote("Nightmare Phantasm drinks in the suffering of the fallen.", nullptr);
            }
        }
    }

    void DespawnShades()
    {
        for(Creature* phant : m_vecPhantasmList)
        {
            if(phant != nullptr && phant->isAlive())
            {
                phant->ForcedDespawn();
            }
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if(pSummoned && pSummoned->GetEntry() == NIGHTMARE_PHANTASM)
        {
            if(firstSummon)
            {
                --m_uiPhantasmCounter;
            }

            if(m_uiPhantasmCounter == 0)
            {
                DoScriptText(ERANIKUS_AQ_YELL_6, m_creature);
                m_uiLanding_timer = 1000;
                m_uiSummonTimer = 10000;
                firstSummon = false;
            }
        }
    }
    
    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiLanding_timer)
        {
            if(m_uiLanding_timer <= uiDiff)
            {
                m_uiLanding_timer = 0;
                m_creature->HandleEmote(EMOTE_ONESHOT_LAND);
                m_creature->SetHover(false);
                m_creature->SetSplineFlags(SPLINEFLAG_NONE);
                m_uilandEmoteTimer = 3000;
            }
            else
                m_uiLanding_timer -= uiDiff;
        }

        if(m_uilandEmoteTimer)
        {
            if(m_uilandEmoteTimer <= uiDiff)
            {
                m_uilandEmoteTimer = 0;
                m_creature->setFaction(14);
                AttackStart(m_creature->GetClosestCreatureWithEntry(m_creature, NPC_REMULOS, 100.0f));
            }
            else
                m_uilandEmoteTimer -= uiDiff;
        }
        
        if(m_uiSummonTimer)
        {
            if(m_uiSummonTimer <= uiDiff)
            {
                if(m_uiPhantasmCounter == 30)
                    m_uiSummonTimer = 0;
                else
                    m_uiSummonTimer = 60000;

                if(firstSummon)
                {
                    m_creature->SetHover(true);
                    DoScriptText(ERANIKUS_AQ_YELL_4, m_creature);
                    m_uiSummonTimer = 0;
                    m_yellTimer = 15000;
                }

                for(int i = 0; i < 5; ++i)
                {
                    Creature *shade = m_creature->SummonCreature(NIGHTMARE_PHANTASM, aEranikusShades[i].m_fX, aEranikusShades[i].m_fY, aEranikusShades[i].m_fZ, 0, TEMPSUMMON_CORPSE_DESPAWN, 60000);
                    ++m_uiPhantasmCounter;

                    if(shade)
                    {
                        shade->AI()->AttackStart(m_creature->GetClosestCreatureWithEntry(m_creature, NPC_REMULOS, 500.0f));
                        m_vecPhantasmList.push_back(shade);
                    }
                }
            }
            else
                m_uiSummonTimer -= uiDiff;
        }

        if(m_yellTimer)
        {
            if(m_yellTimer <= uiDiff)
            {
                DoScriptText(ERANIKUS_AQ_YELL_5, m_creature);
                m_yellTimer = 0;
            }
            else
                m_yellTimer -= uiDiff;
        }

        if(m_endRpTimer)
        {
            if(m_endRpTimer <= uiDiff)
            {
                switch(m_uiSayCount)
                {
                case 1:
                    m_creature->SetDisplayId(6984);
                    m_creature->UpdateVisibilityAndView();
                    m_endRpTimer = 5000;
                    break;
                case 2:
                    DoScriptText(ERANIKUS_AQ_SAYL_1, m_creature);
                    m_endRpTimer = 5000;                    
                    break;
                case 3:
                    DoScriptText(ERANIKUS_AQ_SAYL_2, m_creature);
                    m_endRpTimer = 5000;
                    break;
                case 4:
                    DoScriptText(ERANIKUS_AQ_SAYL_3, m_creature);
                    m_endRpTimer = 5000;
                    break;
                case 5:
                    DoScriptText(ERANIKUS_AQ_SAYL_4, m_creature);
                    m_endRpTimer = 0;
                    break;
                } 

                ++m_uiSayCount;
            }
            else
                m_endRpTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->GetHealthPercent() <= 80.0f && !tyrande_summoned)
        {
            float x = 0, y = 0, z = 0;
            m_creature->GetClosePoint(x, y, z, 2.0f, 5.0f);
            tyrande = m_creature->SummonCreature(NPC_TYRANDE, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); 
            
            if(tyrande)
            {
                tyrande->AI()->AttackStart(m_creature);
                DoScriptText(TYRANDE_AQ_YELL_1, tyrande);
            }

            tyrande_summoned = true;
        }

        if(m_creature->GetHealthPercent() <= 85.0f && m_uiYellCount == 0)
        {
            DoScriptText(ERANIKUS_AQ_YELL_7, m_creature);
            ++m_uiYellCount;
        }
        else if(m_creature->GetHealthPercent() <= 70.0f && m_uiYellCount == 1)
        {
            if(tyrande)
                DoScriptText(TYRANDE_AQ_YELL_2, tyrande);

            ++m_uiYellCount;
        }
        else if(m_creature->GetHealthPercent() <= 65.0f && m_uiYellCount == 2)
        {
            if(tyrande)
                DoScriptText(TYRANDE_AQ_YELL_3, tyrande);
            ++m_uiYellCount;
        }
        else if(m_creature->GetHealthPercent() <= 60.0f && m_uiYellCount == 3)
        {
            if(tyrande)
                DoScriptText(TYRANDE_AQ_SAY_2, tyrande);

            DoScriptText(ERANIKUS_AQ_YELL_8, m_creature);
            ++m_uiYellCount;
        }
        else if(m_creature->GetHealthPercent() <= 45.0f && m_uiYellCount == 4)
        {
            DoScriptText(ERANIKUS_AQ_YELL_10, m_creature);
            ++m_uiYellCount;
        }
        else if(m_creature->GetHealthPercent() <= 30.0f && !isFriendly)
        {
            DoScriptText(ERANIKUS_AQ_YELL_11, m_creature);

            m_creature->GenericTextEmote("Eranikus, Tyrant of the Dream is wholly consumed by the Light of Elune. Tranquility sets in over the Moonglade.", nullptr);

            if(tyrande)
            {
                tyrande->GenericTextEmote("Tyrande falls to one knee.", nullptr);
                DoScriptText(TYRANDE_AQ_YELL_4, tyrande);
            }

            m_creature->HandleEmoteState(EMOTE_STATE_DEAD);

            DespawnShades();
            m_uiSummonTimer = 0;
            m_creature->CombatStop();
            m_creature->setFaction(635);
            isFriendly = true;
            m_endRpTimer = 5000;
            m_uiSayCount = 1;
        }

        DoMeleeAttackIfReady();
    }
};
    
CreatureAI* GetAI_boss_eranikus_tyrant_of_the_dream(Creature* pCreature)
{
    return new boss_eranikus_tyrant_of_the_dream(pCreature);
}

/*##################
## MOB NIGHTMARE PHANTASM
#############*/

struct MANGOS_DLL_DECL mob_nightmare_phantasm : public ScriptedAI
{
    mob_nightmare_phantasm(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiShadowVolleyTimer;

    void Reset()
    {
        m_uiShadowVolleyTimer = urand(5000, 20000);
    }

    void KilledUnit(Unit* pVictim)
    {
        if(pVictim && pVictim->GetTypeId() == TYPEID_PLAYER)
        {
            Creature* eranikus = m_creature->GetClosestCreatureWithEntry(m_creature, BOSS_ERANIKUS, 100.0f);

            if(eranikus)
            {
                if(boss_eranikus_tyrant_of_the_dream* eranikusAi = dynamic_cast<boss_eranikus_tyrant_of_the_dream*>(eranikus->AI()))
                {
                    eranikusAi->EmpowerPhantasms();
                }
            }  
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiShadowVolleyTimer)
        {
            if(m_uiShadowVolleyTimer <= uiDiff)
            {
                m_uiShadowVolleyTimer = urand(5000, 20000);

                DoCast(m_creature, SPELL_SHADOWBOLT_VOLLEY, true);
            }
            else
                m_uiShadowVolleyTimer -= uiDiff;
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nightmare_phantasm(Creature* pCreature)
{
    return new mob_nightmare_phantasm(pCreature);
}

struct MANGOS_DLL_DECL npc_tyrande : public ScriptedAI
{
    npc_tyrande(Creature* pCreature) : ScriptedAI(pCreature) {
        m_uiHealTimer = urand(5000, 20000);
        m_spawnedPriests = false;
        Reset();
    }

    bool m_spawnedPriests;
    uint32 m_uiHealTimer;
    std::vector<Creature*> priests;

    void Reset()
    {   
    }

    void UpdateAI(const uint32 uiDiff)
    {


        if(!m_spawnedPriests)
        {
            for(int i = 0; i < 5; ++i)
            {
                float x = 0, y = 0, z = 0;
                m_creature->GetClosePoint(x, y, z, 2.0f, 5.0f);
                Creature *priest = m_creature->SummonCreature(NPC_MOON_PRIESTESS, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

                if(priest)
                {
                    if(Creature *eranikus = GetClosestCreatureWithEntry(m_creature, BOSS_ERANIKUS, 500.0f))
                        priest->AI()->AttackStart(eranikus);

                    priests.push_back(priest);
                }
            }

            DoScriptText(TYRANDE_AQ_SAY_1, m_creature);

            m_spawnedPriests = true;
        }

        if(m_uiHealTimer)
        {
            if(m_uiHealTimer <= uiDiff)
            {
                m_uiHealTimer = urand(5000, 20000);

                DoCast(m_creature, SPELL_MASS_HEAL, true);
            }
            else
                m_uiHealTimer -= uiDiff;
        }
   
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim())
        {
            if(m_creature->getVictim()->GetEntry() == BOSS_ERANIKUS)
            {
                if(m_creature->getVictim()->getFaction() == 635)
                    m_creature->CombatStop();

                for(Creature* prst : priests)
                {
                    if(prst && prst->isAlive())
                        prst->CombatStop();
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tyrande(Creature* pCreature)
{
    return new npc_tyrande(pCreature);
}

/*##################
## NPC REMOLUS AQ ESCORT
#############*/

enum remulos_AQ
{
    REMULOS_AQ_SAY_1 = -1720122,
    REMULOS_AQ_SAY_2 = -1720123,
    REMULOS_AQ_SAY_3 = -1720124,
    REMULOS_AQ_SAY_4 = -1720125,
    REMULOS_AQ_SAY_5 = -1720126,
    REMULOS_AQ_SAY_6 = -1720127,
    REMULOS_AQ_SAY_7 = -1720128,
    REMULOS_AQ_SAY_8 = -1720129,
    REMULOS_AQ_SAY_9 = -1720144,
    REMULOS_AQ_SAY_10 = -1720151,
};

struct MANGOS_DLL_DECL npc_keeper_remulosAI_AQ : public npc_escortAI
{
    npc_keeper_remulosAI_AQ(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint8  m_uiEventPhase;
    uint32 m_uiEventTimer;
    uint32 m_uiLastReachedWPoint;

    Creature* eranikus;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING) || HasEscortState(STATE_ESCORT_PAUSED))
            return;

        m_uiEventPhase = 0;
        m_uiEventTimer = 0;
        m_uiLastReachedWPoint = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        m_uiLastReachedWPoint = uiPointId;

        switch(uiPointId)
        {
        case 18:
            SetEscortPaused(true);
            m_uiEventPhase = 2;
            m_uiEventTimer = 2000;
            break;
        case 24:
            SetEscortPaused(true);
            break;
        }
    }

    void StartEscort(bool run, const Player* pPlayer, const Quest* pQuest, bool instantrespawn)
    {    
        m_uiEventPhase = 1;
        m_uiEventTimer = 5000;
        Start(run, pPlayer, pQuest, instantrespawn);
        m_creature->SetRespawnEnabled(false);

        if(Player *player = GetPlayerForEscort())
            DoScriptText(REMULOS_AQ_SAY_1, m_creature, player);

        SetEscortPaused(true);
    }  

    void JustDied(Unit* /*pKiller*/)
    {
        if(eranikus)
        {
            eranikus->ForcedDespawn();
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer)
        {
            if (!GetPlayerForEscort())
                return;

            if (m_uiEventTimer <= uiDiff)
            {
                switch(m_uiEventPhase)
                {
                    case 1:
                        DoScriptText(REMULOS_AQ_SAY_2, m_creature);
                        SetEscortPaused(false);
                        m_uiEventTimer = 0;
                        break;
                    case 2:
                        if(Player *pPlayer = GetPlayerForEscort())
                            DoScriptText(REMULOS_AQ_SAY_3, m_creature, pPlayer);
                        m_uiEventTimer = 8000;
                        break;
					case 3:
                        DoScriptText(REMULOS_AQ_SAY_4, m_creature);
                        m_uiEventTimer = 8000;
						break;
                    case 4:
                        DoScriptText(REMULOS_AQ_SAY_5, m_creature);
                        m_uiEventTimer = 3000;
                        break;
					case 5:
						DoCastSpellIfCan(m_creature, 25813);
						m_uiEventTimer = 10000;
                        break;
					case 6:
                        eranikus = m_creature->SummonCreature(15491, 7857, -2691, 491, 0.69, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000, true);
                        eranikus->setFaction(35);
                        eranikus->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        eranikus->SetHover(true);
                        eranikus->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND/* | UNIT_BYTE1_FLAG_UNK_2*/);
                        eranikus->SetSplineFlags(SPLINEFLAG_FLYING);
                        eranikus->GenericTextEmote("Eranikus, Tyrant of the Dream, has entered our world.", nullptr);
                        m_uiEventTimer = 2000;
                        break;
                    case 7:
                        DoScriptText(ERANIKUS_AQ_YELL_1, eranikus);
                        m_uiEventTimer = 10000;
                        break;
                    case 8:
                        DoScriptText(REMULOS_AQ_SAY_6, m_creature);
                        m_uiEventTimer = 5000;
                        break;
                    case 9:
                        eranikus->GenericTextEmote("Eranikus, Tyrant of the Dream lets loose a sinister laugh.", nullptr);
                        DoScriptText(ERANIKUS_AQ_YELL_2, eranikus);
                        m_uiEventTimer = 10000;
                        break;
					case 10:
                        DoScriptText(REMULOS_AQ_SAY_7, m_creature);
                        m_uiEventTimer = 10000;
                        break;
                    case 11:
                        DoScriptText(ERANIKUS_AQ_YELL_3, eranikus);
                        m_uiEventTimer = 10000;
                        break;
                    case 12:
                        eranikus->GenericTextEmote("Eranikus, Tyrant of the Dream roars furiously.", nullptr);
                        if(Player *pPlayer = GetPlayerForEscort())
                            DoScriptText(REMULOS_AQ_SAY_8, m_creature, pPlayer);
                        m_uiEventTimer = 1000;
                        break;
                    case 13:
                        SetEscortPaused(false);
                        m_uiEventTimer = 10000;
                        break;
                    case 14:
                        DoScriptText(REMULOS_AQ_SAY_10, m_creature);
                        if (boss_eranikus_tyrant_of_the_dream* eranikusAi = dynamic_cast<boss_eranikus_tyrant_of_the_dream*>(eranikus->AI()))
                        {
                            eranikusAi->MoveToLand();
                        }

                        m_uiEventTimer = 0;

                        break;
                    case 15:
                        DoScriptText(REMULOS_AQ_SAY_9, m_creature);
                        
                        if(Player *pPlayer = GetPlayerForEscort())
                            pPlayer->GroupEventHappens(8736, m_creature);

                        m_uiEventTimer = 5000;
                        break;
                    case 16:
                        SetEscortPaused(false);
                        m_uiEventTimer = 0;
                        break;
                    case 17:
                        break;
                    case 18:
                        break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim())
        {
            if(m_creature->getVictim()->GetEntry() == BOSS_ERANIKUS)
            {
                if(m_creature->getVictim()->getFaction() == 635)
                    m_creature->CombatStop();

                m_uiEventTimer = 30000;
                m_uiEventPhase = 15;
            }
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_keeper_remulos_AQ(Creature* pCreature)
{
    return new npc_keeper_remulosAI_AQ(pCreature);
}

bool QuestAccept_npc_keeper_remulos(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WAKING_LEGENDS)
    {
        if (npc_keeper_remulosAI* pEscortAI = dynamic_cast<npc_keeper_remulosAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_REMULOS_1, pCreature, pPlayer);
            pEscortAI->Start(false, pPlayer, pQuest, true);
        }
    }
    else if(pQuest->GetQuestId() == QUEST_THE_NIGHTMARE_MANIFESTS)
    {
        Creature *remuloCopy = pCreature->SummonCreature(11890, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), pCreature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0, true);
        pCreature->SetVisibility(VISIBILITY_OFF);
        pCreature->UpdateVisibilityAndView();
        if (npc_keeper_remulosAI_AQ* pEscortAI = dynamic_cast<npc_keeper_remulosAI_AQ*>(remuloCopy->AI()))
        {
            pEscortAI->StartEscort(true, pPlayer, pQuest, false);
        }
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

    pNewScript = new Script;
    pNewScript->Name = "npc_keeper_remulos_AQ";
    pNewScript->GetAI = &GetAI_npc_keeper_remulos_AQ;
    pNewScript->RegisterSelf();
    
    pNewScript = new Script;
    pNewScript->Name = "boss_eranikus_tyrant_of_the_dream";
    pNewScript->GetAI = &GetAI_boss_eranikus_tyrant_of_the_dream;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nightmare_phantasm";
    pNewScript->GetAI = &GetAI_mob_nightmare_phantasm;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tyrande";
    pNewScript->GetAI = &GetAI_npc_tyrande;
    pNewScript->RegisterSelf();
}
