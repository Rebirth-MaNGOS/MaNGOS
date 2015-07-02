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
SDName: Silithus
SD%Complete: 100
SDComment: Abbysal Council support, Quest support: 1126, 7785, 8304, 8315, 8534, 8538, 8539.
SDCategory: Silithus
EndScriptData */

/* ContentData
boss_abyssal_council
go_wind_stone
go_hiveashi_pod
go_silithus_hive_crystal
npc_cenarion_scout
npc_highlord_demitrian
npcs_rutgar_and_frankal
EndContentData */

#include "precompiled.h"
#include "../../../../game/MotionMaster.h"
#include "../../../../game/TargetedMovementGenerator.h"
#include "escort_ai.h"
#include "TemporarySummon.h"

/*###
## boss_abyssal_council
###*/

enum eAbyssalHighCouncil
{
    FACTION_FRIENDLY                 = 35,

    // Abbysal Council members
    NPC_CRIMSON_TEMPLAR              = 15209,
    NPC_HOARY_TEMPLAR                = 15212,
    NPC_EARTHEN_TEMPLAR              = 15307,
    NPC_AZURE_TEMPLAR                = 15211,
    NPC_THE_DUKE_OF_CYNDERS          = 15206,
    NPC_THE_DUKE_OF_ZEPHYRS          = 15220,
    NPC_THE_DUKE_OF_SHARDS           = 15208,
    NPC_THE_DUKE_OF_FANTHOMS         = 15207,
    NPC_PRINCE_SKALDRENOX            = 15203,
    NPC_HIGH_MARSHAL_WHIRLAXIS       = 15204,
    NPC_BARON_KAZUM                  = 15205,
    NPC_LORD_SKWOL                   = 15305,

    // Crimson Templar
    SPELL_FIREBALL_VOLLEY           = 11989,
    SPELL_FLAME_BUFFET              = 16536,
    // Hoary Templar
    SPELL_LIGHTNING_SHIELD_TEMPLAR  = 19514,
    // Earthen Templar
    SPELL_ENTANGLING_ROOTS          = 22127,
    SPELL_KNOCK_AWAY_TEMPLAR        = 18813,
    // Azure Templar
    SPELL_FROST_NOVA                = 14907,
    SPELL_FROST_SHOCK               = 12548,
    // The Duke Of Cynders
    SPELL_BLAST_WAVE_DUKE           = 22424,
    SPELL_FIRE_BLAST                = 25028,
    SPELL_FLAMESTRIKE               = 18399,
    // The Duke of Zephyris
    SPELL_ENVELOPING_WINGS          = 15535,
    SPELL_FORKED_LIGHTNING          = 25034,
    SPELL_WING_FLAP                 = 12882,
    // The Duke of Shards
    SPELL_GROUND_TREMOR             = 6524,
    SPELL_STRIKE                    = 13446,
    SPELL_THUNDERCLAP               = 8078,
    // The Duke of Fanthoms
    SPELL_KNOCK_AWAY                = 18670,
    SPELL_KNOCKDOWN                 = 16790,
    SPELL_TRASH                     = 3391,
    // Skaldrenox
    SPELL_BLAST_WAVE_PRINCE         = 25049,
    SPELL_CLEAVE                    = 15284,
    SPELL_MARK_OF_FLAMES            = 25050,
    // High Marshal Whirlaxis
    SPELL_LIGHTNING_BREATH          = 20627,
    SPELL_LIGHTNING_SHIELD          = 25020,
    SPELL_UPDRAFT                   = 25060,
    // Baron Kazum
    SPELL_MASSIVE_TREMOR            = 19129,
    SPELL_MORTAL_STRIKE             = 17547,
    SPELL_STOMP                     = 25056,
    // Lord Skwol
    SPELL_VENOM_SPIT                = 25053,
};

struct MANGOS_DLL_DECL boss_abyssal_councilAI : public ScriptedAI
{
    boss_abyssal_councilAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bIsFighting;
    uint32 m_uiInitialTimer;
    uint32 m_uiRemoveTimer;
    uint32 m_uiSpell1Entry;
    uint32 m_uiSpell2Entry;
    uint32 m_uiSpell3Entry;
    uint32 m_uiSpell1Timer;
    uint32 m_uiSpell2Timer;
    uint32 m_uiSpell3Timer;

    ObjectGuid m_uiTargetGUID;

    void Reset()
    {
        m_bIsFighting = false;
        m_uiInitialTimer = 7000;
        m_uiRemoveTimer = 0;
        m_uiSpell1Timer = urand(1000,3000);
        m_uiSpell2Timer = urand(6000,8000);
        m_uiSpell3Timer = urand(11000,13000);

		m_uiTargetGUID.Clear();

        switch(m_creature->GetEntry())
        {
            case NPC_CRIMSON_TEMPLAR:
                m_uiSpell1Entry = SPELL_FIREBALL_VOLLEY;
                m_uiSpell2Entry = SPELL_FLAME_BUFFET;
                m_uiSpell3Entry = 0;
                break;
            case NPC_HOARY_TEMPLAR:
                m_uiSpell1Entry = 0;
                m_uiSpell2Entry = SPELL_LIGHTNING_SHIELD_TEMPLAR;
                m_uiSpell3Entry = 0;
                break;
            case NPC_EARTHEN_TEMPLAR:
                m_uiSpell1Entry = SPELL_ENTANGLING_ROOTS;
                m_uiSpell2Entry = SPELL_KNOCK_AWAY_TEMPLAR;
                m_uiSpell3Entry = 0;
                break;
            case NPC_AZURE_TEMPLAR:
                m_uiSpell1Entry = SPELL_FROST_NOVA;
                m_uiSpell2Entry = SPELL_FROST_SHOCK;
                m_uiSpell3Entry = 0;
                break;
            case NPC_THE_DUKE_OF_CYNDERS:
                m_uiSpell1Entry = SPELL_BLAST_WAVE_DUKE;
                m_uiSpell2Entry = SPELL_FIRE_BLAST;
                m_uiSpell3Entry = SPELL_FLAMESTRIKE;
                break;
            case NPC_THE_DUKE_OF_ZEPHYRS:
                m_uiSpell1Entry = SPELL_FORKED_LIGHTNING;
                m_uiSpell2Entry = SPELL_WING_FLAP;
                m_uiSpell3Entry = SPELL_ENVELOPING_WINGS;
                break;
            case NPC_THE_DUKE_OF_SHARDS:
                m_uiSpell1Entry = SPELL_THUNDERCLAP;
                m_uiSpell2Entry = SPELL_STRIKE;
                m_uiSpell3Entry = SPELL_GROUND_TREMOR;
                break;
            case NPC_THE_DUKE_OF_FANTHOMS:
                m_uiSpell1Entry = SPELL_KNOCKDOWN;
                m_uiSpell2Entry = SPELL_TRASH;
                m_uiSpell3Entry = SPELL_KNOCK_AWAY;
                break;
            case NPC_PRINCE_SKALDRENOX:
                m_uiSpell1Entry = SPELL_BLAST_WAVE_PRINCE;
                m_uiSpell2Entry = SPELL_CLEAVE;
                m_uiSpell3Entry = SPELL_MARK_OF_FLAMES;
                break;
            case NPC_HIGH_MARSHAL_WHIRLAXIS:
                m_uiSpell1Entry = SPELL_LIGHTNING_BREATH;
                m_uiSpell2Entry = SPELL_LIGHTNING_SHIELD;
                m_uiSpell3Entry = SPELL_UPDRAFT;
                break;
            case NPC_BARON_KAZUM:
                m_uiSpell1Entry = SPELL_MASSIVE_TREMOR;
                m_uiSpell2Entry = SPELL_MORTAL_STRIKE;
                m_uiSpell3Entry = SPELL_STOMP;
                break;
            case NPC_LORD_SKWOL:
                m_uiSpell1Entry = SPELL_TRASH;
                m_uiSpell2Entry = SPELL_VENOM_SPIT;
                m_uiSpell3Entry = 0;
                break;
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
    }

    void JustReachedHome()
    {
        m_uiRemoveTimer = 30*IN_MILLISECONDS;
    }

    void SetTarget(Player* pPlayer)
    {
        m_uiTargetGUID = pPlayer->GetObjectGuid();
    }

    void Execute(uint32 uiSpellEntry)
    {
        Unit* pTarget = NULL;
        switch(uiSpellEntry)
        {
            case SPELL_LIGHTNING_SHIELD_TEMPLAR:
            case SPELL_TRASH:
            case SPELL_LIGHTNING_SHIELD:
                pTarget = m_creature;
                break;
            case SPELL_FIREBALL_VOLLEY:
            case SPELL_FLAME_BUFFET:
            case SPELL_KNOCK_AWAY_TEMPLAR:
            case SPELL_FROST_NOVA:
            case SPELL_BLAST_WAVE_DUKE:
            case SPELL_FORKED_LIGHTNING:
            case SPELL_WING_FLAP:
            case SPELL_GROUND_TREMOR:
            case SPELL_STRIKE:
            case SPELL_THUNDERCLAP:
            case SPELL_KNOCK_AWAY:
            case SPELL_KNOCKDOWN:
            case SPELL_BLAST_WAVE_PRINCE:
            case SPELL_CLEAVE:
            case SPELL_UPDRAFT:
            case SPELL_MASSIVE_TREMOR:
            case SPELL_MORTAL_STRIKE:
            case SPELL_STOMP:
            case SPELL_VENOM_SPIT:
                pTarget = m_creature->getVictim();
                break;
            case SPELL_ENTANGLING_ROOTS:
            case SPELL_FROST_SHOCK:
            case SPELL_FIRE_BLAST:
            case SPELL_FLAMESTRIKE:
            case SPELL_ENVELOPING_WINGS:
            case SPELL_LIGHTNING_BREATH:
            case SPELL_MARK_OF_FLAMES:
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                break;
        }
        if (pTarget && uiSpellEntry)
            DoCastSpellIfCan(pTarget, uiSpellEntry);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Remove after 30s out of combat
        if (m_uiRemoveTimer)
        {
            if (m_uiRemoveTimer <= uiDiff)
            {
                m_uiRemoveTimer = 0;
                m_creature->RemoveFromWorld();
            }
            else
                m_uiRemoveTimer -= uiDiff;
        }

        // Fight will start 7s after spawning
        if (!m_bIsFighting)
        {
            if (m_uiInitialTimer <= uiDiff)
            {
                m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
                m_bIsFighting = true;
                if (Player* pTarget = m_creature->GetMap()->GetPlayer(m_uiTargetGUID))
                    m_creature->AI()->AttackStart(pTarget);
                else
                    m_creature->SetInCombatWithZone();
            }
            else
                m_uiInitialTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cancel removing when enter combat again
        if (m_uiRemoveTimer)
            m_uiRemoveTimer = 0;

        // Spells of Abbysal Council
        if (m_uiSpell1Timer <= uiDiff)
        {
            Execute(m_uiSpell1Entry);
            m_uiSpell1Timer = urand(3000,6000);
        }
        else
            m_uiSpell1Timer -= uiDiff;

        if (m_uiSpell2Timer <= uiDiff)
        {
            Execute(m_uiSpell2Entry);
            m_uiSpell2Timer = urand(7000,11000);
        }
        else
            m_uiSpell2Timer -= uiDiff;

        if (m_uiSpell3Timer <= uiDiff)
        {
            Execute(m_uiSpell3Entry);
            m_uiSpell3Timer = urand(12000,15000);
        }
        else
            m_uiSpell3Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_abyssal_council(Creature* pCreature)
{
    return new boss_abyssal_councilAI(pCreature);
}

/*###
## go_wind_stone
###*/

enum eWindStones
{
    ACCESS_DENIED                       = 0,
    ACCESS_LESSER_WIND_STONE            = 1,
    ACCESS_WIND_STONE                   = 2,
    ACCESS_GREATER_WIND_STONE           = 3,

    AURA_LESSER_WIND_STONE              = 24746,
    AURA_WIND_STONE                     = 24748,
    AURA_GREATER_WIND_STONE             = 24782,

    GO_LESSER_WIND_STONE_1              = 180456,
    GO_LESSER_WIND_STONE_2              = 180518,
    GO_LESSER_WIND_STONE_3              = 180529,
    GO_LESSER_WIND_STONE_4              = 180544,
    GO_LESSER_WIND_STONE_5              = 180549,
    GO_LESSER_WIND_STONE_6              = 180564,
    GO_WIND_STONE_1                     = 180461,
    GO_WIND_STONE_2                     = 180534,
    GO_WIND_STONE_3                     = 180554,
    GO_GREATER_WIND_STONE_1             = 180466,
    GO_GREATER_WIND_STONE_2             = 180539,
    GO_GREATER_WIND_STONE_3             = 180559,

    ITEM_CREST_OF_BECKONING_FIRE        = 20416,
    ITEM_CREST_OF_BECKONING_THUNDER     = 20418,
    ITEM_CREST_OF_BECKONING_STONE       = 20419,
    ITEM_CREST_OF_BECKONING_WATER       = 20420,
    ITEM_SIGNET_OF_BECKONING_FIRE       = 20432,
    ITEM_SIGNET_OF_BECKONING_THUNDER    = 20433,
    ITEM_SIGNET_OF_BECKONING_STONE      = 20435,
    ITEM_SIGNET_OF_BECKONING_WATER      = 20436,
    ITEM_SCEPTER_OF_BECKONING_FIRE      = 20447,
    ITEM_SCEPTER_OF_BECKONING_THUNDER   = 20448,
    ITEM_SCEPTER_OF_BECKONING_STONE     = 20449,
    ITEM_SCEPTER_OF_BECKONING_WATER     = 20450,

    SPELL_ABYSSAL_PUNISHMENT            = 24803,
    SPELL_ELEMENTAL_SPAWN_IN            = 25035,
    SPELL_SUMMON_TEMPLAR_RANDOM         = 24734,
    SPELL_SUMMON_TEMPLAR_FIRE           = 24744,
    SPELL_SUMMON_TEMPLAR_THUNDER        = 24756,
    SPELL_SUMMON_TEMPLAR_STONE          = 24758,
    SPELL_SUMMON_TEMPLAR_WATER          = 24760,
    SPELL_SUMMON_DUKE_RANDOM            = 24763,
    SPELL_SUMMON_DUKE_FIRE              = 24765,
    SPELL_SUMMON_DUKE_THUNDER           = 24768,
    SPELL_SUMMON_DUKE_STONE             = 24770,
    SPELL_SUMMON_DUKE_WATER             = 24772,
    SPELL_SUMMON_ROYAL_RANDOM           = 24784,
    SPELL_SUMMON_ROYAL_FIRE             = 24786,
    SPELL_SUMMON_ROYAL_THUNDER          = 24788,
    SPELL_SUMMON_ROYAL_STONE            = 24789,
    SPELL_SUMMON_ROYAL_WATER            = 24790,
};

const uint32 aWindStoneEntry[3][4] =
{
    {NPC_CRIMSON_TEMPLAR, NPC_HOARY_TEMPLAR, NPC_EARTHEN_TEMPLAR, NPC_EARTHEN_TEMPLAR},
    {NPC_THE_DUKE_OF_CYNDERS, NPC_THE_DUKE_OF_ZEPHYRS, NPC_THE_DUKE_OF_SHARDS, NPC_THE_DUKE_OF_FANTHOMS},
    {NPC_PRINCE_SKALDRENOX, NPC_HIGH_MARSHAL_WHIRLAXIS, NPC_BARON_KAZUM, NPC_LORD_SKWOL}
};

uint32 GetAccessToWindstone(GameObject* pGo, Player* pPlayer)
{
    switch(pGo->GetEntry())
    {
        case GO_LESSER_WIND_STONE_1:
        case GO_LESSER_WIND_STONE_2:
        case GO_LESSER_WIND_STONE_3:
        case GO_LESSER_WIND_STONE_4:
        case GO_LESSER_WIND_STONE_5:
        case GO_LESSER_WIND_STONE_6:
            if (pPlayer->HasAura(AURA_LESSER_WIND_STONE, EFFECT_INDEX_0))
                return ACCESS_LESSER_WIND_STONE;
            break;
        case GO_WIND_STONE_1:
        case GO_WIND_STONE_2:
        case GO_WIND_STONE_3:
            if (pPlayer->HasAura(AURA_LESSER_WIND_STONE, EFFECT_INDEX_0) && pPlayer->HasAura(AURA_WIND_STONE, EFFECT_INDEX_0))
                return ACCESS_WIND_STONE;
            break;
        case GO_GREATER_WIND_STONE_1:
        case GO_GREATER_WIND_STONE_2:
        case GO_GREATER_WIND_STONE_3:
            if (pPlayer->HasAura(AURA_LESSER_WIND_STONE, EFFECT_INDEX_0) && pPlayer->HasAura(AURA_WIND_STONE, EFFECT_INDEX_0) && pPlayer->HasAura(AURA_GREATER_WIND_STONE, EFFECT_INDEX_0))
                return ACCESS_GREATER_WIND_STONE;
            break;
    }
    return ACCESS_DENIED;
}

bool GossipHello_go_wind_stone(Player* pPlayer, GameObject* pGo)
{
    switch(GetAccessToWindstone(pGo, pPlayer))
    {
        case ACCESS_DENIED:
            pPlayer->CastSpell(pPlayer, SPELL_ABYSSAL_PUNISHMENT, true);
            break;
        case ACCESS_LESSER_WIND_STONE:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Summon an Abyssal Templar", 0, GOSSIP_ACTION_INFO_DEF + 1);

            if (pPlayer->HasItemCount(ITEM_CREST_OF_BECKONING_FIRE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Crimson Templar! I hold your crest! Heed my call!", ITEM_CREST_OF_BECKONING_FIRE, GOSSIP_ACTION_INFO_DEF + 2);
            if (pPlayer->HasItemCount(ITEM_CREST_OF_BECKONING_THUNDER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Hoary Templar! I hold your crest! Heed my call!", ITEM_CREST_OF_BECKONING_THUNDER, GOSSIP_ACTION_INFO_DEF + 3);
            if (pPlayer->HasItemCount(ITEM_CREST_OF_BECKONING_STONE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Earthen Templar! I hold your crest! Heed my call!", ITEM_CREST_OF_BECKONING_STONE, GOSSIP_ACTION_INFO_DEF + 4);
            if (pPlayer->HasItemCount(ITEM_CREST_OF_BECKONING_WATER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Azure Templar! I hold your crest! Heed my call!", ITEM_CREST_OF_BECKONING_WATER, GOSSIP_ACTION_INFO_DEF + 5);

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo), pGo->GetObjectGuid());
            break;
        case ACCESS_WIND_STONE:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Summon an Abyssal Duke", 0, GOSSIP_ACTION_INFO_DEF + 6);

            if (pPlayer->HasItemCount(ITEM_SIGNET_OF_BECKONING_FIRE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Duke of Cynders! I hold your signet! Heed my call!", ITEM_SIGNET_OF_BECKONING_FIRE, GOSSIP_ACTION_INFO_DEF + 7);
            if (pPlayer->HasItemCount(ITEM_SIGNET_OF_BECKONING_THUNDER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Duke of Zephyrs! I hold your signet! Heed my call!", ITEM_SIGNET_OF_BECKONING_THUNDER, GOSSIP_ACTION_INFO_DEF + 8);
            if (pPlayer->HasItemCount(ITEM_SIGNET_OF_BECKONING_STONE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Duke of Shards! I hold your signet! Heed my call!", ITEM_SIGNET_OF_BECKONING_STONE, GOSSIP_ACTION_INFO_DEF + 9);
            if (pPlayer->HasItemCount(ITEM_SIGNET_OF_BECKONING_WATER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Duke of Fathoms! I hold your signet! Heed my call!", ITEM_SIGNET_OF_BECKONING_WATER, GOSSIP_ACTION_INFO_DEF + 10);

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo), pGo->GetObjectGuid());
            break;
        case ACCESS_GREATER_WIND_STONE:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Summon an Abyssal Royal Lord", 0, GOSSIP_ACTION_INFO_DEF + 11);

            if (pPlayer->HasItemCount(ITEM_SCEPTER_OF_BECKONING_FIRE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Prince Skaldrenox! I hold your scepter! Heed my call!", ITEM_SCEPTER_OF_BECKONING_FIRE, GOSSIP_ACTION_INFO_DEF + 12);
            if (pPlayer->HasItemCount(ITEM_SCEPTER_OF_BECKONING_THUNDER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "High Marshal Whirlaxis! I hold your scepter! Heed my call!", ITEM_SCEPTER_OF_BECKONING_THUNDER, GOSSIP_ACTION_INFO_DEF + 13);
            if (pPlayer->HasItemCount(ITEM_SCEPTER_OF_BECKONING_STONE, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Baron Kazum! I hold your scepter! Heed my call!", ITEM_SCEPTER_OF_BECKONING_STONE, GOSSIP_ACTION_INFO_DEF + 14);
            if (pPlayer->HasItemCount(ITEM_SCEPTER_OF_BECKONING_WATER, 1))
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lord Skwol! I hold your scepter! Heed my call!", ITEM_SCEPTER_OF_BECKONING_WATER, GOSSIP_ACTION_INFO_DEF + 15);

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo), pGo->GetObjectGuid());
            break;
    }
    return true;
}

void RemoveTwilightCultistGear(Player* pPlayer, uint32 stone)
{
  // The single break in the switch is correct, don't add more.
  switch (stone)
  {
    case ACCESS_GREATER_WIND_STONE:
      pPlayer->DestroyItemCount(20451, 1, true, false);  // Remove the ring.
    case ACCESS_WIND_STONE:
      pPlayer->DestroyItemCount(20422, 1, true, false);  // Remove the medallion.
    case ACCESS_LESSER_WIND_STONE:
	for (short i = 0; i < 3; i++)
	  pPlayer->DestroyItemCount(20406 + i, 1, true, false);  // Remove the armour.

      break;
  }
}

bool GossipSelect_go_wind_stone(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    if (!GetAccessToWindstone(pGo, pPlayer) || (uiSender && !pPlayer->HasItemCount(uiSender, 1)))
    {
        pPlayer->CastSpell(pPlayer, SPELL_ABYSSAL_PUNISHMENT, true);
        pPlayer->CLOSE_GOSSIP_MENU();
        return true;
    }

    uint32 m_uiSummonEntry = 0;
    uint32 m_uiSpellEntry = 0;
    float fX, fY, fZ;
    pGo->GetPosition(fX, fY, fZ);

    switch(uiAction)
    {
        // Lesser Wind Stone
        case GOSSIP_ACTION_INFO_DEF + 1:
            m_uiSummonEntry = aWindStoneEntry[0][urand(0,3)];
            m_uiSpellEntry = SPELL_SUMMON_TEMPLAR_RANDOM;
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            m_uiSummonEntry = NPC_CRIMSON_TEMPLAR;
            m_uiSpellEntry = SPELL_SUMMON_TEMPLAR_FIRE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            m_uiSummonEntry = NPC_HOARY_TEMPLAR;
            m_uiSpellEntry = SPELL_SUMMON_TEMPLAR_THUNDER;
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            m_uiSummonEntry = NPC_EARTHEN_TEMPLAR;
            m_uiSpellEntry = SPELL_SUMMON_TEMPLAR_STONE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            m_uiSummonEntry = NPC_AZURE_TEMPLAR;
            m_uiSpellEntry = SPELL_SUMMON_TEMPLAR_WATER;
            break;
        // Wind Stone
        case GOSSIP_ACTION_INFO_DEF + 6:
            m_uiSummonEntry = aWindStoneEntry[1][urand(0,3)];
            m_uiSpellEntry = SPELL_SUMMON_DUKE_RANDOM;
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            m_uiSummonEntry = NPC_THE_DUKE_OF_CYNDERS;
            m_uiSpellEntry = SPELL_SUMMON_DUKE_FIRE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 8:
            m_uiSummonEntry = NPC_THE_DUKE_OF_ZEPHYRS;
            m_uiSpellEntry = SPELL_SUMMON_DUKE_THUNDER;
            break;
        case GOSSIP_ACTION_INFO_DEF + 9:
            m_uiSummonEntry = NPC_THE_DUKE_OF_SHARDS;
            m_uiSpellEntry = SPELL_SUMMON_DUKE_STONE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            m_uiSummonEntry = NPC_THE_DUKE_OF_FANTHOMS;
            m_uiSpellEntry = SPELL_SUMMON_DUKE_WATER;
            break;
        // Greater Wind Stone
        case GOSSIP_ACTION_INFO_DEF + 11:
            m_uiSummonEntry = aWindStoneEntry[2][urand(0,3)];
            m_uiSpellEntry = SPELL_SUMMON_ROYAL_RANDOM;
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            m_uiSummonEntry = NPC_PRINCE_SKALDRENOX;
            m_uiSpellEntry = SPELL_SUMMON_ROYAL_FIRE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            m_uiSummonEntry = NPC_HIGH_MARSHAL_WHIRLAXIS;
            m_uiSpellEntry = SPELL_SUMMON_ROYAL_THUNDER;
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            m_uiSummonEntry = NPC_BARON_KAZUM;
            m_uiSpellEntry = SPELL_SUMMON_ROYAL_STONE;
            break;
        case GOSSIP_ACTION_INFO_DEF + 15:
            m_uiSummonEntry = NPC_LORD_SKWOL;
            m_uiSpellEntry = SPELL_SUMMON_ROYAL_WATER;
            break;
    }

    uint32 stone = GetAccessToWindstone(pGo, pPlayer);
    if (m_uiSummonEntry && m_uiSpellEntry && stone != ACCESS_DENIED)
    {
        pPlayer->CastSpell(pPlayer, m_uiSpellEntry, false);
        pGo->RemoveFromWorld();
        switch(stone)
        {
            case ACCESS_LESSER_WIND_STONE:
                pGo->SetRespawnTime(7*MINUTE);
                break;
            case ACCESS_WIND_STONE:
                pGo->SetRespawnTime(15*MINUTE);
                break;
            case ACCESS_GREATER_WIND_STONE:
                pGo->SetRespawnTime(3*HOUR);
                break;
        }
        pGo->SaveRespawnTime();
        if (Creature* pCouncil = pGo->SummonCreature(m_uiSummonEntry, fX, fY, fZ, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 120000))
        {
            pCouncil->setFaction(FACTION_FRIENDLY);
            pCouncil->CastSpell(pCouncil, SPELL_ELEMENTAL_SPAWN_IN, true);
            if (boss_abyssal_councilAI* pCouncilAI = dynamic_cast<boss_abyssal_councilAI*>(pCouncil->AI()))
                pCouncilAI->SetTarget(pPlayer);
        }
        
        RemoveTwilightCultistGear(pPlayer, stone);
    }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

/*######
## go_hiveashi_pod
######*/

enum eHiveAshiPod
{
    NPC_HIVE_AMBUSHER       = 13301,
    QUEST_HIVE_IN_THE_TOWER = 1126,
};

bool GOUse_go_hiveashi_pod(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_HIVE_IN_THE_TOWER) == QUEST_STATUS_INCOMPLETE)
    {
        float fX, fY, fZ;
        pGo->GetPosition(fX, fY, fZ);
        for(uint8 i = 0; i < 2; ++i)
        {
            if (Creature* pAmbusher = pGo->SummonCreature(NPC_HIVE_AMBUSHER, fX+irand(-5,5), fY+irand(-5,5), fZ, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
                pAmbusher->AI()->AttackStart(pPlayer);
        }
    }
    pPlayer->SendLoot(pGo->GetObjectGuid(), LOOT_CORPSE);
    return true;
}

/*###
## npc_cenarion_scout
###*/

enum eCenarionScouts
{
    GOSSIP_CENARION_SCOUT_AZENEL    = 8063,
    GOSSIP_CENARION_SCOUT_JALIA     = 8064,
    GOSSIP_CENARION_SCOUT_LANDION   = 8065,

    NPC_CENARION_SCOUT_AZENEL       = 15610,
    NPC_CENARION_SCOUT_JALIA        = 15611,
    NPC_CENARION_SCOUT_LANDION      = 15609,

    QUEST_HIVEZORA_SCOUT_REPORT     = 8534,
    QUEST_HIVEASHI_SCOUT_REPORT     = 8739,
    QUEST_HIVEREGAL_SCOUT_REPORT    = 8738,

    SPELL_HIVEZORA_SCOUT_REPORT     = 25843,
    SPELL_HIVEASHI_SCOUT_REPORT     = 25845,
    SPELL_HIVEREGAL_SCOUT_REPORT    = 25847,
};

bool GossipHello_npc_cenarion_scout(Player* pPlayer, Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_CENARION_SCOUT_AZENEL:
            if (pPlayer->GetQuestStatus(QUEST_HIVEZORA_SCOUT_REPORT) == QUEST_STATUS_INCOMPLETE)
                pCreature->CastSpell(pPlayer, SPELL_HIVEZORA_SCOUT_REPORT, false);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_CENARION_SCOUT_AZENEL, pCreature->GetObjectGuid());
            break;
        case NPC_CENARION_SCOUT_JALIA:
            if (pPlayer->GetQuestStatus(QUEST_HIVEASHI_SCOUT_REPORT) == QUEST_STATUS_INCOMPLETE)
                pCreature->CastSpell(pPlayer, SPELL_HIVEASHI_SCOUT_REPORT, false);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_CENARION_SCOUT_JALIA, pCreature->GetObjectGuid());
            break;
        case NPC_CENARION_SCOUT_LANDION:
            if (pPlayer->GetQuestStatus(QUEST_HIVEREGAL_SCOUT_REPORT) == QUEST_STATUS_INCOMPLETE)
                pCreature->CastSpell(pPlayer, SPELL_HIVEREGAL_SCOUT_REPORT, false);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_CENARION_SCOUT_LANDION, pCreature->GetObjectGuid());
            break;
        default:
            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*###
## npc_highlord_demitrian
###*/

#define GOSSIP_ITEM_DEMITRIAN1 "What do you know of it?"
#define GOSSIP_ITEM_DEMITRIAN2 "I am listening , Demitrian."
#define GOSSIP_ITEM_DEMITRIAN3 "Continue, please."
#define GOSSIP_ITEM_DEMITRIAN4 "A battle?"
#define GOSSIP_ITEM_DEMITRIAN5 "<Nod>"
#define GOSSIP_ITEM_DEMITRIAN6 "Caught unaware? How?"
#define GOSSIP_ITEM_DEMITRIAN7 "So what did Ragnaros do next?"

enum
{
    QUEST_EXAMINE_THE_VESSEL        =   7785,
    ITEM_BINDINGS_WINDSEEKER_LEFT   =   18563,
    ITEM_BINDINGS_WINDSEEKER_RIGHT  =   18564,
    ITEM_VESSEL_OF_REBIRTH          =   19016,
    GOSSIP_TEXTID_DEMITRIAN1        =   6842,
    GOSSIP_TEXTID_DEMITRIAN2        =   6843,
    GOSSIP_TEXTID_DEMITRIAN3        =   6844,
    GOSSIP_TEXTID_DEMITRIAN4        =   6867,
    GOSSIP_TEXTID_DEMITRIAN5        =   6868,
    GOSSIP_TEXTID_DEMITRIAN6        =   6869,
    GOSSIP_TEXTID_DEMITRIAN7        =   6870
};

bool GossipHello_npc_highlord_demitrian(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_EXAMINE_THE_VESSEL) == QUEST_STATUS_NONE &&
        (pPlayer->HasItemCount(ITEM_BINDINGS_WINDSEEKER_LEFT,1,false) || pPlayer->HasItemCount(ITEM_BINDINGS_WINDSEEKER_RIGHT,1,false)))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_highlord_demitrian(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEMITRIAN7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEMITRIAN7, pCreature->GetObjectGuid());

            if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_VESSEL_OF_REBIRTH, 1))
                pPlayer->SendNewItem(pItem, 1, true, false);

            break;
    }
    return true;
}

/*###
## npcs_rutgar_and_frankal
###*/

//gossip item text best guess
#define GOSSIP_ITEM_SEEK1 "I seek information about Natalia"

#define GOSSIP_ITEM_RUTGAR2 "That sounds dangerous!"
#define GOSSIP_ITEM_RUTGAR3 "What did you do?"
#define GOSSIP_ITEM_RUTGAR4 "Who?"
#define GOSSIP_ITEM_RUTGAR5 "Women do that. What did she demand?"
#define GOSSIP_ITEM_RUTGAR6 "What do you mean?"
#define GOSSIP_ITEM_RUTGAR7 "What happened next?"

#define GOSSIP_ITEM_FRANKAL11 "Yes, please continue"
#define GOSSIP_ITEM_FRANKAL12 "What language?"
#define GOSSIP_ITEM_FRANKAL13 "The Priestess attacked you?!"
#define GOSSIP_ITEM_FRANKAL14 "I should ask the monkey about this"
#define GOSSIP_ITEM_FRANKAL15 "Then what..."

enum
{
    QUEST_DEAREST_NATALIA       =   8304,
    NPC_RUTGAR                  =   15170,
    NPC_FRANKAL                 =   15171,
    TRIGGER_RUTGAR              =   15222,
    TRIGGER_FRANKAL             =   15221,
    GOSSIP_TEXTID_RF            =   7754,
    GOSSIP_TEXTID_RUTGAR1       =   7755,
    GOSSIP_TEXTID_RUTGAR2       =   7756,
    GOSSIP_TEXTID_RUTGAR3       =   7757,
    GOSSIP_TEXTID_RUTGAR4       =   7758,
    GOSSIP_TEXTID_RUTGAR5       =   7759,
    GOSSIP_TEXTID_RUTGAR6       =   7760,
    GOSSIP_TEXTID_RUTGAR7       =   7761,
    GOSSIP_TEXTID_FRANKAL1      =   7762,
    GOSSIP_TEXTID_FRANKAL2      =   7763,
    GOSSIP_TEXTID_FRANKAL3      =   7764,
    GOSSIP_TEXTID_FRANKAL4      =   7765,
    GOSSIP_TEXTID_FRANKAL5      =   7766,
    GOSSIP_TEXTID_FRANKAL6      =   7767
};

bool GossipHello_npcs_rutgar_and_frankal(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_DEAREST_NATALIA) == QUEST_STATUS_INCOMPLETE &&
        pCreature->GetEntry() == NPC_RUTGAR &&
        !pPlayer->GetReqKillOrCastCurrentCount(QUEST_DEAREST_NATALIA, TRIGGER_RUTGAR))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SEEK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    if (pPlayer->GetQuestStatus(QUEST_DEAREST_NATALIA) == QUEST_STATUS_INCOMPLETE &&
        pCreature->GetEntry() == NPC_FRANKAL &&
        pPlayer->GetReqKillOrCastCurrentCount(QUEST_DEAREST_NATALIA, TRIGGER_RUTGAR))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SEEK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RF, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npcs_rutgar_and_frankal(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTGAR7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR6, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 6:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_RUTGAR7, pCreature->GetObjectGuid());
            //'kill' our trigger to update quest status
            pPlayer->KilledMonsterCredit(TRIGGER_RUTGAR, pCreature->GetObjectGuid());
            break;

        case GOSSIP_ACTION_INFO_DEF + 9:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL1, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL12, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL2, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 11:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL13, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL3, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL14, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL4, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FRANKAL15, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL5, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_FRANKAL6, pCreature->GetObjectGuid());
            //'kill' our trigger to update quest status
            pPlayer->KilledMonsterCredit(TRIGGER_FRANKAL, pCreature->GetObjectGuid());
            break;
    }
    return true;
}

/*######
## go_silithus_hive_crystal
######*/

enum eHiveCrystal
{
    ITEM_GEOLOGISTS_TRANSCRIPTION_KIT   = 20453,
    GO_HIVE_ASHI_GLYPHED_CRYSTAL        = 180454,
    GO_HIVE_ZORA_GLYPHED_CRYSTAL        = 180455,
    GO_HIVE_REGAL_GLYPHED_CRYSTAL       = 180453,
    SPELL_HIVE_ASHI_RUBBING             = 24806,
    SPELL_HIVE_ZORA_RUBBING             = 24805,
    SPELL_HIVE_REGAL_RUBBING            = 24807,
};

bool GOUse_go_silithus_hive_crystal(Player* pPlayer, GameObject* pGo)
{
    // Player must to have Geologist's Transcription Kit item (provided by taking quest).
    if (!pPlayer->HasItemCount(ITEM_GEOLOGISTS_TRANSCRIPTION_KIT, 1))
        return false;

    switch(pGo->GetEntry())
    {
        case GO_HIVE_ASHI_GLYPHED_CRYSTAL:    // Hive'Ashi
            pPlayer->CastSpell(pPlayer, SPELL_HIVE_ASHI_RUBBING, true);
            break;
        case GO_HIVE_ZORA_GLYPHED_CRYSTAL:    // Hive'Zora
            pPlayer->CastSpell(pPlayer, SPELL_HIVE_ZORA_RUBBING, true);
            break;
        case GO_HIVE_REGAL_GLYPHED_CRYSTAL:    // Hive'Regal
            pPlayer->CastSpell(pPlayer, SPELL_HIVE_REGAL_RUBBING, true);
            break;
    }
    return true;
}

enum InfantrySpells
{
	SPELL_KICK								= 15618,
	SPELL_INCAPACITATING_SHOUT				= 18328,
	SPELL_INFANTRY_MORTAL_STRIKE			= 19643,
	SPELL_AUTOSHOT							= 75

};

struct MANGOS_DLL_DECL npc_cenarion_hold_infantryAI : public ScriptedAI
{
    npc_cenarion_hold_infantryAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	uint32 m_uiShootTimer;
	uint32 m_uiKickTimer;
	uint32 m_uiShoutTimer;
	uint32 m_uiMortalStrikeTimer;

    void Reset()
    {
		m_uiShootTimer = 1000;
		m_uiKickTimer = 0;
		m_uiShoutTimer = urand(1000, 3000);
		m_uiMortalStrikeTimer = urand(4000, 6000);

    }

	void Aggro(Unit* /*who*/)
	{
		int say[] = { -1156, -1157, -1158 };
		m_creature->MonsterSay(say[urand(0,2)], 0);
	}

    void UpdateAI(const uint32 uiDiff)
    {

		if (m_creature->getVictim() && !m_creature->GetMotionMaster()->operator->()->IsReachable())
		{
			if (m_uiShootTimer <= uiDiff)
			{
				m_creature->CastSpell(m_creature->getVictim(), SPELL_AUTOSHOT, true);
				m_uiShootTimer = 1000;
			}	
			else
				m_uiShootTimer -= uiDiff;
			
			return;
		}

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			return;

		if (m_uiKickTimer <= uiDiff)
		{
			if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_KICK) == CAST_OK)
				m_uiKickTimer = urand(8000, 14000);
		}
		else
			m_uiKickTimer -= uiDiff;

		if (m_uiShoutTimer <= uiDiff)
		{
			if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INCAPACITATING_SHOUT) == CAST_OK)
				m_uiShoutTimer = urand(9000, 12000);
		}
		else
			m_uiShoutTimer -= uiDiff;

			
		if (m_uiMortalStrikeTimer <= uiDiff)
		{
			if (DoCastSpellIfCan(m_creature->getVictim(), InfantrySpells::SPELL_INFANTRY_MORTAL_STRIKE) == CAST_OK)
				m_uiMortalStrikeTimer = urand(7000, 9000);
		}
		else
			m_uiMortalStrikeTimer -= uiDiff;



        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_cenarion_hold_infantry(Creature* pCreature)
{
    return new npc_cenarion_hold_infantryAI(pCreature);
}

/*######
## npc_the_calling_event
######*/

enum TheCallingEvent
{
	NPC_ALUNTIR							= 15288,
	NPC_ARAKIS							= 15290,
	NPC_XIL_XIX 						= 15286,

	NPC_ROMAN_KHAN						= 14862,

	WAVE_ONE							= 1,
	WAVE_BOSS							= 2,
};

struct Loc
{
    float x, y, z;
};

static Loc aMobSpawnLoc[]= 
{
	{-7283.15f,863.34f,3.18f},
	{-7270.11f, 841.34f, 3.94f},
	{-7286.15f, 843.34f, 2.24f},
	{-7276.38f, 852.34f, 3.64f},
};

struct MANGOS_DLL_DECL npc_the_calling_eventAI : public ScriptedAI
{
    npc_the_calling_eventAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_uiEventTimer = 1000;
		m_uiPhase = 0;
        Reset();
    }

    uint8 m_uiKilledCreatures;
	uint8 m_uiPhase;

	uint32 m_uiEventTimer;

    void Reset()
    {
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);
    }

	void DoSummonWave(uint32 uiSummonId = 0)
    {
        if (uiSummonId == WAVE_ONE)
        {
            for (uint8 i = 0; i < 1; ++i)
            {
                m_creature->SummonCreature(NPC_ALUNTIR, aMobSpawnLoc[0].x, aMobSpawnLoc[0].y, aMobSpawnLoc[0].z, 5.48f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 600000);
				m_creature->SummonCreature(NPC_ARAKIS, aMobSpawnLoc[1].x, aMobSpawnLoc[1].y, aMobSpawnLoc[1].z, 2.23f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 600000);
				m_creature->SummonCreature(NPC_XIL_XIX, aMobSpawnLoc[2].x, aMobSpawnLoc[2].y, aMobSpawnLoc[2].z, 0.74f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 600000);
            }
        }
        if (uiSummonId == WAVE_BOSS)
        {
            for (uint8 i = 0; i < 1; ++i)
                m_creature->SummonCreature(NPC_ROMAN_KHAN, aMobSpawnLoc[3].x, aMobSpawnLoc[3].y, aMobSpawnLoc[3].z, 0.50f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1200000);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
		pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn
    }

	void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ALUNTIR || pSummoned->GetEntry() == NPC_ARAKIS || pSummoned->GetEntry() == NPC_XIL_XIX)
        {
            ++m_uiKilledCreatures;
		}

		// If first wave is done
        if (m_uiKilledCreatures >= 3)
			m_uiEventTimer = 10000;

		// if boss died, despawn dummy
		if (pSummoned->GetEntry() == NPC_ROMAN_KHAN)
			m_creature->ForcedDespawn();
	}

	void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiEventTimer)
		{
			if (m_uiEventTimer <= uiDiff)
			{
				switch (m_uiPhase)
				{
					case 0:
						DoSummonWave(WAVE_ONE);
						m_uiEventTimer = 0;
						break;
					case 1:
						DoSummonWave(WAVE_BOSS);
						m_uiEventTimer = 0;
						break;
				}
			++m_uiPhase;
			}
			else
			{
				m_uiEventTimer -= uiDiff;
			}
		}			
	}
};

CreatureAI* GetAI_npc_the_calling_event(Creature* pCreature)
{
    return new npc_the_calling_eventAI(pCreature);
}

/*####
# npc_geologist_larksbane
####*/

enum eLarksbane
{
	LARKSBANE_SAY1								= -1720088,
	LARKSBANE_SAY2								= -1720089,
	LARKSBANE_SAY3								= -1720090,
	LARKSBANE_SAY4								= -1720091,
	LARKSBANE_SAY5								= -1720092,
	LARKSBANE_SAY6								= -1720093,
	LARKSBANE_SAY7								= -1720094,
	LARKSBANE_SAY8								= -1720095,
	LARKSBANE_SAY9								= -1720096,
	LARKSBANE_SAY10								= -1720097,
	LARKSBANE_SAY11								= -1720098,
	LARKSBANE_SAY12								= -1720099,
	LARKSBANE_SAY13								= -1720100,
	LARKSBANE_SAY14								= -1720101,
	LARKSBANE_SAY15								= -1720102,
	LARKSBANE_SAY16								= -1720103,
	LARKSBANE_SAY17								= -1720104,
	LARKSBANE_SAY18								= -1720105,
	LARKSBANE_SAY19								= -1720106,
	LARKSBANE_SAY20								= -1720107,
	LARKSBANE_SAY21								= -1720108,
	LARKSBANE_SAY22								= -1720109,

	BARISTOLOTH_SAY								= -1720110,

	NPC_BARISTOLOTH								= 15180,
	NPC_BROOD_OF_NOZDORMU						= 15185,

	GO_GLYPHED_CRYSTAL							= 180514,

	QUEST_ID_THE_CALLING						= 8315,
};

struct MANGOS_DLL_DECL npc_geologist_larksbaneAI : public npc_escortAI
{
    npc_geologist_larksbaneAI(Creature* pCreature) : npc_escortAI(pCreature) 
	{ 
		Reset();
	}
	
	uint8 m_uiSpeechStep;
	uint32 m_uiSpeechTimer;
	bool m_bOutro;

    void Reset()
	{
        if (!m_creature->isActiveObject() && m_creature->isAlive())
            m_creature->SetActiveObjectState(true);

		m_bOutro = false;
		m_uiSpeechStep = 1;
		m_uiSpeechTimer = 0;
	}

	void WaypointReached(uint32 /*uiPointId*/)
    {
	}

	void JustStartedEscort()
    {
    }

	void StartOutro()
	{		
		m_bOutro = true; 
		m_uiSpeechTimer = 2000;
		m_uiSpeechStep = 1;
		m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
	}

	void Crystals(int action)
	{
		// Search for crystals and set them as active. Pref not all at the same time but for now all
		std::list<GameObject*> m_lCrystals;
		GetGameObjectListWithEntryInGrid(m_lCrystals, m_creature, GO_GLYPHED_CRYSTAL, 10.0f);
		for (std::list<GameObject*>::const_iterator itr = m_lCrystals.begin(); itr != m_lCrystals.end(); ++itr)
		{
			if (action == 0)
				(*itr)->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

			if (action == 1)
			{
			if ((*itr)->GetGoState() == GO_STATE_ACTIVE)
				continue;

			(*itr)->SetGoState(GO_STATE_ACTIVE);
			}
			if (action == 2)
			{
			if ((*itr)->GetGoState() == GO_STATE_ACTIVE)
				(*itr)->SetGoState(GO_STATE_READY);
			}
			if (action == 3)
				(*itr)->Delete();
		}
	}

	void JustSummoned(Creature* pSummoned)
    {
		if (pSummoned->GetEntry() == NPC_BROOD_OF_NOZDORMU)
		{
			pSummoned->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
			pSummoned->SetSplineFlags(SPLINEFLAG_FLYING);
			pSummoned->GetMotionMaster()->MovePoint(1,-7039.02f,891.77f,133.02f);
		}
	}
	
	void UpdateAI(const uint32 uiDiff)
    {
		npc_escortAI::UpdateAI(uiDiff);

		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
	}

	void UpdateEscortAI(const uint32 uiDiff)
    {
		if (m_uiSpeechTimer && m_bOutro)							// handle RP at quest end
		{
			if (!m_uiSpeechStep)
				return;
		
			if (m_uiSpeechTimer <= uiDiff)
            {		
				Creature* pBaristoloth = GetClosestCreatureWithEntry(m_creature, NPC_BARISTOLOTH, 30.0f);
                switch(m_uiSpeechStep)
                {
					case 1:
						m_creature->GetMotionMaster()->MovePoint(1, -6823.92f, 809.55f,51.66f);
						m_creature->SummonCreature(NPC_BROOD_OF_NOZDORMU, -6667.05f,793.94f,133.01f,2.89f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000,true);
						m_uiSpeechTimer = 5000;
						break;
					case 2:			// craft emote
						m_creature->HandleEmoteState(EMOTE_STATE_USESTANDING);
						m_uiSpeechTimer = 3000;
						break;
					case 3:			// spawn crystals
						m_creature->HandleEmoteState(EMOTE_STATE_NONE);	
						m_creature->SummonGameObject(GO_GLYPHED_CRYSTAL,600000,-6820.02f, 807.99f,51.66f,2.74f);
						m_creature->SummonGameObject(GO_GLYPHED_CRYSTAL,600000,-6822.19f, 812.35f,51.66f,4.52f);
						m_creature->SummonGameObject(GO_GLYPHED_CRYSTAL,600000,-6822.83f, 808.998f,51.66f,0.78f);

                        m_uiSpeechTimer = 100;
						break;
					case 4:			// set no interact on crystals
						Crystals(0);
						m_uiSpeechTimer = 3000;
						break;
					case 5:
						Crystals(1);
						m_uiSpeechTimer = 5000;
						break;
					case 6:
						DoScriptText(LARKSBANE_SAY2, m_creature);
						m_uiSpeechTimer = 12000;
						break;
					case 7:
						DoScriptText(LARKSBANE_SAY3, m_creature);
						m_uiSpeechTimer = 24000;
						break;
					case 8:
						DoScriptText(LARKSBANE_SAY4, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 9:
						DoScriptText(LARKSBANE_SAY5, m_creature);
						m_uiSpeechTimer = 24000;
						break;
					case 10:
						DoScriptText(LARKSBANE_SAY6, m_creature);
						m_uiSpeechTimer = 28000;
						break;
					case 11:		
						DoScriptText(LARKSBANE_SAY7, m_creature);
						m_uiSpeechTimer = 12000;
						break;
					case 12:
						m_creature->GenericTextEmote("Geologist Larksbane regains her composure.", NULL, false);
						m_uiSpeechTimer = 10000;
						break;
					case 13:
						DoScriptText(LARKSBANE_SAY8, m_creature);
						m_uiSpeechTimer = 8000;
						break;
					case 14:
						DoScriptText(LARKSBANE_SAY9, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 15:
						DoScriptText(LARKSBANE_SAY10, m_creature);
						m_uiSpeechTimer = 12000;
						break;
					case 16:
						DoScriptText(LARKSBANE_SAY11, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 17:
						DoScriptText(LARKSBANE_SAY12, m_creature);
						m_uiSpeechTimer = 24000;
						break;
					case 18:
						DoScriptText(LARKSBANE_SAY13, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 19:
						DoScriptText(LARKSBANE_SAY14, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 20:
						DoScriptText(LARKSBANE_SAY15, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 21:
						DoScriptText(LARKSBANE_SAY16, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 22:
						DoScriptText(LARKSBANE_SAY17, m_creature);
						m_uiSpeechTimer = 30000;
						break;
					case 23:
						DoScriptText(LARKSBANE_SAY18, m_creature);
						m_uiSpeechTimer = 14000;
						break;
					case 24:
						DoScriptText(LARKSBANE_SAY19, m_creature);
						m_uiSpeechTimer = 8000;
						break;
					case 25:
						DoScriptText(LARKSBANE_SAY20, m_creature);
						m_uiSpeechTimer = 20000;
						break;
					case 26:
						DoScriptText(LARKSBANE_SAY21, m_creature);
						m_uiSpeechTimer = 16000;
						break;
					case 27:
						DoScriptText(LARKSBANE_SAY22, m_creature);
						m_uiSpeechTimer = 8000;
						break;
					case 28:			// stop crystal glow and spawn the dragon that flies over them
						Crystals(2);
						m_creature->SummonCreature(NPC_BROOD_OF_NOZDORMU, -6667.05f,793.94f,133.01f,2.89f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000,true);
						m_uiSpeechTimer = 3000;
						break;
					case 29:
						m_creature->GenericTextEmote("Baristolth of the Shifting Sands shifts uncomfortably.", NULL, false);
						m_uiSpeechTimer = 8000;
						break;
					case 30:
						if (pBaristoloth)
							DoScriptText(BARISTOLOTH_SAY, pBaristoloth);
						m_uiSpeechTimer = 5000;
						break;
					case 31:			// despawn crystals
						Crystals(3);
						m_uiSpeechTimer = 2000;
						break;
					case 32:
						m_creature->GetMotionMaster()->MoveTargetedHome();
						m_uiSpeechTimer = 3000;
						break;
					case 33:						
						m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);					
						m_bOutro = false;
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
	}
};

CreatureAI* GetAI_npc_geologist_larksbane(Creature* pCreature)
{
    return new npc_geologist_larksbaneAI(pCreature);
}

bool OnQuestRewarded_npc_geologist_larksbane(Player* /*pPlayer*/, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_ID_THE_CALLING)
    {
		DoScriptText(LARKSBANE_SAY1, pCreature);
		if (npc_geologist_larksbaneAI* pEscortAI = dynamic_cast<npc_geologist_larksbaneAI*>(pCreature->AI()))
			pEscortAI->StartOutro();
	}
	return true;
}

/*####
# boss_roman_khan
####*/

enum eRomanKhan
{
	SPELL_WILT				= 23772,
	SPELL_SYSTEM_SHOCK		= 23774,
	//SPELL_EXPLODE			= 0,		// find out what spell this should be!
	SPELL_ARCANE_ERUPTION	= 25672,   // from moam for now
	SPELL_DRAIN_MANA         = 25671, // also from moam, trigger by wilt so he gets mana
};

struct MANGOS_DLL_DECL boss_roman_khanAI : public ScriptedAI
{
    boss_roman_khanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiWiltTimer;
    uint32 m_uiSystemShockTimer;
	uint32 m_uiCheckoutManaTimer;

    void Reset()
    {
        m_uiWiltTimer = urand(6000,11000);			// 5-10 sec on retail video
        m_uiSystemShockTimer = urand(9000,15000);
		m_uiCheckoutManaTimer = 15000;
		m_creature->SetPower(POWER_MANA, 0);
        m_creature->SetMaxPower(POWER_MANA, 0);
    }

	void Aggro(Unit* /*pWho*/)
    {
        m_creature->SetMaxPower(POWER_MANA, m_creature->GetCreatureInfo()->maxmana);
    }

	void KilledUnit(Unit* victim)
    {
        
        if (victim->GetTypeId() != TYPEID_PLAYER)
            return;

        // heal for 2% everytime a player dies
		m_creature->SetHealth(m_creature->GetHealth()+m_creature->GetMaxHealth()*0.02);
	}

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_WILT && pTarget->GetTypeId() == TYPEID_PLAYER)
			m_creature->CastSpell(pTarget,SPELL_DRAIN_MANA, true);				//added mana drain to wilt, probably should be in core but oh well
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // 100% mana -> explode        
		if (m_uiCheckoutManaTimer <= uiDiff)
        {
			m_uiCheckoutManaTimer = 2000;
			
			if (m_creature->GetPower(POWER_MANA) == m_creature->GetMaxPower(POWER_MANA))
				DoCastSpellIfCan(m_creature, SPELL_ARCANE_ERUPTION);
        } 
		else
			m_uiCheckoutManaTimer -= uiDiff;

        // Wilt
        if (m_uiWiltTimer <= uiDiff)
        {			
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WILT);
            m_uiWiltTimer = urand(5000,10000);
        }
        else
            m_uiWiltTimer -= uiDiff;

        // System Shock
        if (m_uiSystemShockTimer <= uiDiff)			// cast time depends on how much mana the boss has, about 3 sec?
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            float percent = m_creature->GetPower(POWER_MANA) * 100 / m_creature->GetMaxPower(POWER_MANA);
			if (percent <= 25.0f)
            {
				m_creature->CastSpell(pTarget, SPELL_SYSTEM_SHOCK, true);
				m_uiSystemShockTimer = urand(25000,30000);
            }
			else if (percent > 25.0f && percent <= 50.0f)
            {
				m_creature->CastSpell(pTarget, SPELL_SYSTEM_SHOCK, true);
				m_uiSystemShockTimer = urand(20000,25000);
            }
			else if (percent > 50.0f && percent <= 75.0f)
            {
				m_creature->CastSpell(pTarget, SPELL_SYSTEM_SHOCK, true);
				m_uiSystemShockTimer = urand(15000,20000);
            }
			else if (percent > 75.0f)
            {
				m_creature->CastSpell(pTarget, SPELL_SYSTEM_SHOCK, true);
				m_uiSystemShockTimer = urand(10000,15000);
            }
        }
        else
            m_uiSystemShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_roman_khan(Creature* pCreature)
{
    return new boss_roman_khanAI(pCreature);
}

/*####
# npc_mistress_natalia_maralith
####*/

enum eNatalia
{
	SPELL_BLACKOUT			= 15269,
	SPELL_PSYCHIC_SCREAM	= 13704,
	SPELL_SHADOW_WORD_PAIN	= 11639,
	SPELL_MIND_FLAY			= 16568,
	SPELL_DOMINATE_MIND		= 20740,
	SPELL_HEALING_TOUCH		= 27527,

	NATALIA_SAY_AGGRO		= -1720119,
	NATALIA_SAY_DEATH		= -1720120,
};

struct MANGOS_DLL_DECL npc_mistress_natalia_maralithAI : public ScriptedAI
{
    npc_mistress_natalia_maralithAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiPsychicScreamTimer;
    uint32 m_uiShadowWordPainTimer;
	uint32 m_uiMindFlayTimer;
	uint32 m_uiDominateMindTimer;
	uint32 m_uiHealingTouchTimer;

    void Reset()
    {
        m_uiPsychicScreamTimer = urand(6000,15000);
        m_uiShadowWordPainTimer = urand(5000,10000);
		m_uiMindFlayTimer = urand(12000,15000);
		m_uiDominateMindTimer = urand(14000,22000);
		m_uiHealingTouchTimer = 1000;
    }

	void Aggro(Unit* /*pWho*/)
    {
        int say = urand(0,2);
		if (say == 1)
			DoScriptText(NATALIA_SAY_AGGRO, m_creature);
    }

	  void JustDied(Unit* /*pKiller*/)
    {
		int say2 = urand(0,2);
		if (say2 == 1)
			DoScriptText(NATALIA_SAY_DEATH, m_creature);    
    } 

	void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if ((pSpell->Id == SPELL_SHADOW_WORD_PAIN || pSpell->Id == SPELL_MIND_FLAY) && pTarget->GetTypeId() == TYPEID_PLAYER)
		{
			int blackoutProcc = urand(1,10);
			if (blackoutProcc == 1)
				m_creature->CastSpell(pTarget,SPELL_BLACKOUT, true);				//has to manually procc blackout
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Psychic Scream
        if (m_uiPsychicScreamTimer <= uiDiff)
        {			
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PSYCHIC_SCREAM);
            m_uiPsychicScreamTimer = urand(10000,20000);
        }
        else
            m_uiPsychicScreamTimer -= uiDiff;

        // Shadow Word Pain
        if (m_uiShadowWordPainTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            
			m_creature->CastSpell(pTarget, SPELL_SHADOW_WORD_PAIN, false);
			m_uiShadowWordPainTimer = urand(15000,19000);          
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

		// Mind flay
        if (m_uiMindFlayTimer <= uiDiff)
        {			
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_FLAY);
            m_uiMindFlayTimer = urand(8000,17000);
        }
        else
            m_uiMindFlayTimer -= uiDiff;

		// Dominate mind
        if (m_uiDominateMindTimer <= uiDiff)
        {
			Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            
			m_creature->CastSpell(pTarget, SPELL_DOMINATE_MIND, false);
			m_uiDominateMindTimer = urand(20000,30000);          
        }
        else
            m_uiDominateMindTimer -= uiDiff;

		if(HealthBelowPct(50))
		{
			// Healing touch
			if (m_uiHealingTouchTimer <= uiDiff)
			{			
				DoCastSpellIfCan(m_creature, SPELL_HEALING_TOUCH);
				m_uiHealingTouchTimer = urand(10000,15000);
			}
			else
				m_uiHealingTouchTimer -= uiDiff;
		}

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_mistress_natalia_maralith(Creature* pCreature)
{
    return new npc_mistress_natalia_maralithAI(pCreature);
}

/********************************************************
 * Scripting for the quest "A Pawn on the Eternal Board *
********************************************************/

enum PawnValues
{
    MUSIC = 8493,
    Q_A_PAWN_ON_THE_ETERNAL_BOARD = 8519,
    MERITHRAS_WAKE_NPC = 15427,
    ANACHRONOS_TRIGGER = 15454,
    ANUBI_CONQUEROR = 15424,
    QIRAJI_TANK = 15422,
    QIRAJI_DRONE = 15421,
    QIRAJI_WASP = 15414,
    KALDOREI_INFANTRY = 15423,
    ANACHRONOS_THE_ANCIENT = 15381,
    FANDRAL_STAGHELM = 15382,
    MERITHRA = 15378,
    CAELESTRASZ = 15379,
    ARYGOS = 15380,
    SPELL_ARYGOS = 25149,
    SPELL_MERITHRA = 25145,
    SPELL_MERITHRA_VISUAL = 25172,
    SPELL_CAELESTRASZ = 25150,
    SPELL_TIME_STOP = 25158,
    SPELL_TIME_STOP_VISUAL = 25171,
    SPELL_GREEN_DRAGON = 25105, 
    SPELL_RED_DRAGON = 25106,
    SPELL_BLUE_DRAGON = 25107,
    SPELL_BRONZE_DRAGON = 25108,
    SPELL_STAGHELM_BREAK = 25182,
    SPELL_CALL_ANCIENTS = 25167,
    SPELL_CALL_GLYPS_OF_WARDING = 25166,
    SPELL_CALL_PRISMATIC_BARRIER = 25159,
    AQ_RUNE = 180898,
    AQ_ROOT = 180899,
    AQ_ANCIENT_DOOR = 180904
};

enum PawnTalks
{
    TALK = -1720152,
    TALK1 = -1720153,
    TALK2 = -1720154,
    TALK3 = -1720155,
    TALK4 = -1720156,
    TALK5 = -1720157,
    TALK6 = -1720158,
    TALK7 = -1720159,
    TALK8 = -1720160,
    TALK9 = -1720161,
    TALK10 = -1720163,
    TALK11 = -1720162,
    TALK12 = -1720164,
    TALK13 = -1720165,
    TALK14 = -1720166,
    TALK15 = -1720167,
    TALK16 = -1720168,
    TALK17 = -1720169,
    TALK18 = -1720170,
    TALK19 = -1720171,
    TALK20 = -1720172,
    TALK21 = -1720173,
    TALK22 = -1720174,
    TALK23 = -1720175
};

enum Emotes
{
    NOD = 67,
};

struct MANGOS_DLL_DECL npc_anachronos_triggerAI : public ScriptedAI
{
    npc_anachronos_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiEventTimer;
    uint32 m_uiEventStage;

    std::vector<ObjectGuid> m_elfs;
    std::vector<ObjectGuid> m_bugs;

    ObjectGuid m_Anachronos;
    ObjectGuid m_Fandral;
    ObjectGuid m_Merithra;
    ObjectGuid m_Caelestrasz;
    ObjectGuid m_Arygos;
    ObjectGuid m_Door;
    ObjectGuid m_Roots;
    ObjectGuid m_Runes;
    ObjectGuid m_Player;

    const float m_BreathSpot[3] = { -8060.f, 1534.f, 2.62f };

    void Reset()
    {
        m_uiEventTimer = 4000;
        m_uiEventStage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiEventTimer > uiDiff)
        {
            m_uiEventTimer -= uiDiff;
            return;
        }

        switch (m_uiEventStage)
        {
            case 0:
                {

                    // Stage 0 - Spawn the three NPCs, the Guards and the door objects.
                    // Start playing the event music.

                    GameObject* pOSummon = m_creature->SummonGameObject(AQ_ANCIENT_DOOR, 0, -8124.28f, 1524.88f, 17.19f, -0.023624, GO_STATE_ACTIVE);
                    m_Door = pOSummon->GetObjectGuid();

                    pOSummon = m_creature->SummonGameObject(AQ_RUNE, 0, -8124.28f, 1524.88f, 17.19f, -0.023624, GO_STATE_ACTIVE);
                    m_Runes = pOSummon->GetObjectGuid();

                    // Play the background music.
                    m_creature->PlayDirectSound(MUSIC, nullptr);

                    Creature* pSummon = m_creature->SummonCreature(ANACHRONOS_THE_ANCIENT, -8028.55f, 1539.27f, 2.61f, 4.1091f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSummon->setFaction(1601);
                        m_Anachronos = pSummon->GetObjectGuid();
                    }

                    pSummon = m_creature->SummonCreature(FANDRAL_STAGHELM, -8028.79f, 1536.57f, 2.61f, 2.662f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSummon->setFaction(1601);
                        m_Fandral = pSummon->GetObjectGuid();
                    }

                    pSummon = m_creature->SummonCreature(MERITHRA, -8032.65f, 1538.06f, 2.61f, 5.923f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSummon->setFaction(1601);
                        m_Merithra = pSummon->GetObjectGuid();
                    }

                    pSummon = m_creature->SummonCreature(CAELESTRASZ, -8031.27f, 1535.84f, 2.61f, 0.629f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSummon->setFaction(1601);
                        m_Caelestrasz = pSummon->GetObjectGuid();
                    }

                    pSummon = m_creature->SummonCreature(ARYGOS, -8031.26f, 1539.59f, 2.61f, 5.427f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSummon->setFaction(1601);
                        m_Arygos = pSummon->GetObjectGuid();
                    }

                    pSummon = m_creature->SummonCreature(ANUBI_CONQUEROR, -8091.91f, 1503.81f, 2.63f, 1.414f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->GetMotionMaster()->MoveRandom();
                        pSummon->setFaction(310);
                        m_bugs.push_back(pSummon->GetObjectGuid());
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }

                    pSummon = m_creature->SummonCreature(ANUBI_CONQUEROR, -8105.31f, 1547.41f, 3.88f, 4.79f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->GetMotionMaster()->MoveRandom();
                        pSummon->setFaction(310);
                        m_bugs.push_back(pSummon->GetObjectGuid());
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }

                    pSummon = m_creature->SummonCreature(ANUBI_CONQUEROR, -8091.35f, 1544.08f, 2.61f, 4.50f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->GetMotionMaster()->MoveRandom();
                        pSummon->setFaction(310);
                        m_bugs.push_back(pSummon->GetObjectGuid());
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage += 100;
                    break;
                }
                case 100: // Anachronus first talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        DoScriptText(TALK, pAnachronos);
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 200:
                {
                    for (ObjectGuid guid : m_bugs)
                    {
                        Creature* pCreature = m_creature->GetMap()->GetCreature(guid);
                        if (pCreature)
                            pCreature->SetIgnoreNonCombatFlags(true);
                    }

                    SpawnBugs();

                    for (short i = 0; i < 12; i++)
                    {
                        float x = m_creature->GetPositionX() + frand(0, 8) * cosf(frand(0, 6));
                        float y = m_creature->GetPositionY() + frand(0, 8) * sinf(frand(0, 6));

                        Creature* pSummon = m_creature->SummonCreature(KALDOREI_INFANTRY, x, y, 5.f, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        if (pSummon)
                        {
                            //pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pSummon->setFaction(35);
                            m_elfs.push_back(pSummon->GetObjectGuid());
                            pSummon->SetIgnoreNonCombatFlags(true);
                        }
                    }

                    AddThreat();

                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pFandral && pAnachronos)
                    {
                        pFandral->SetFacingToObject(pAnachronos);
                    }

                    m_uiEventTimer = 2000;
                    m_uiEventStage += 100;
                    break;
                }
                case 300: // Fandral - start talking
                {

                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        DoScriptText(TALK1, pFandral);

                    m_uiEventTimer = 2000;
                    m_uiEventStage += 1;
                    break;
                }
                case 301:
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                        pMerithra->GenericTextEmote("Mirithra of the Dream glances at her compatriots.", nullptr, true);

                    m_uiEventTimer = 3000;
                    m_uiEventStage = 400;
                    break;
                }
                case 400: // Fandral - turn towards Merithra
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pFandral && pMerithra)
                    {
                        pFandral->SetFacingToObject(pMerithra);
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 500: // Merithra - first talk
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                        DoScriptText(TALK2, pMerithra);

                    m_uiEventTimer = 4000;
                    m_uiEventStage += 1;
                    break;
                }
                case 501:
                {
                    Creature* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                    {
                        pArygos->HandleEmote(EMOTE_ONESHOT_YES);
                        pArygos->GenericTextEmote("Arygos nods knowingly.", nullptr, true);
                    }

                    m_uiEventTimer = 2000;
                    m_uiEventStage = 600;
                    break;
                }
                case 600: // Caelestrasz - first talk
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pFandral && pCaelestrasz)
                    {
                        pCaelestrasz->SetFacingToObject(pFandral);
                        DoScriptText(TALK3, pCaelestrasz);
                    }

                    m_uiEventTimer = 10000;
                    m_uiEventStage += 100;
                    break;
                }
                case 700: // Merithra - second talk
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                        DoScriptText(TALK4, pMerithra);

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 800: // Merithra - runs away
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                    {
                        pMerithra->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pMerithra->GetMotionMaster()->MovePoint(0, m_BreathSpot[0], m_BreathSpot[1], m_BreathSpot[2], true);
                    }

                    m_uiEventTimer = 11000;
                    m_uiEventStage += 100;
                    break;
                }
                case 900: // Merithra - turns into a dragon
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                    {
                        DoScriptText(TALK5, pMerithra);
                        pMerithra->CastSpell(pMerithra, SPELL_GREEN_DRAGON, false);
                    }

                    m_uiEventTimer = 9000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1000: // Merithra - breathes on monsters, Arygos - first talk
                {
                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                    {
                        pMerithra->CastSpell(m_creature, SPELL_MERITHRA, true);
                    }

                    Unit* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                        DoScriptText(TALK6, pArygos);

                    m_uiEventTimer = 2300;
                    m_uiEventStage += 100;
                    break;
                }
                case 1100: // Cast the Merithra aura on all of the mobs.
                {
                    Creature* pTarget = m_creature->GetMap()->GetCreature(m_bugs[m_bugs.size() - 1]);
                    if (pTarget)
                    {
                        for (ObjectGuid guid : m_bugs)
                        {
                            Creature* pTarget = m_creature->GetMap()->GetCreature(guid);
                            if (pTarget)
                                pTarget->CastSpell(pTarget, SPELL_MERITHRA_VISUAL, true);
                        }
                    }


                    Creature* pMerithra = m_creature->GetMap()->GetCreature(m_Merithra);
                    if (pMerithra)
                    {
                        pMerithra->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pMerithra->SetSplineFlags(SPLINEFLAG_FLYING);
                        pMerithra->GetMotionMaster()->MovePoint(1, -8130.f, 1525.f, 47.f, false);
                    }

                    m_uiEventTimer = 2500;
                    m_uiEventStage += 100;
                    break;
                }
                case 1200: // Arygos - runs over
                {
                    Creature* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                    {
                        pArygos->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pArygos->GetMotionMaster()->MovePoint(0, m_BreathSpot[0], m_BreathSpot[1], m_BreathSpot[2], true);
                    }

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1300: // Arygos - screams
                {
                    Unit* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                        DoScriptText(TALK7, pArygos);

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                }
                case 1400: // Arygos - turns into a dragon
                {
                    Unit* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                        pArygos->CastSpell(pArygos, SPELL_BLUE_DRAGON, false);

                    m_uiEventTimer = 7000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1500: // Arygos -- breathes on the monsters
                {
                    Unit* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                    {
                        pArygos->CastSpell(m_creature, SPELL_ARYGOS, true);
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1600: // Arygos flies away
                {
                    Creature* pArygos = m_creature->GetMap()->GetCreature(m_Arygos);
                    if (pArygos)
                    {
                        pArygos->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pArygos->SetSplineFlags(SPLINEFLAG_FLYING);
                        pArygos->GetMotionMaster()->MovePoint(1, -8130.f, 1525.f, 47.f, false);
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1700: // Caelestrasz - talks
                {
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                        DoScriptText(TALK8, pCaelestrasz);

                    SpawnBugs();
                    AddThreat();

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1800: // Caelestrasz - runs off
                {
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                    {
                        pCaelestrasz->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pCaelestrasz->GetMotionMaster()->MovePoint(0, m_BreathSpot[0], m_BreathSpot[1], m_BreathSpot[2], true);
                    }

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 1900: // Caelestrasz - screams
                {
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                        DoScriptText(TALK9, pCaelestrasz);

                    m_uiEventTimer = 2000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2000: // Caelestrasz turns into a dragon
                {
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                        pCaelestrasz->CastSpell(pCaelestrasz, SPELL_RED_DRAGON, false);

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2100: // Caelestrasz - breathes
                {
                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                    {
                        pCaelestrasz->CastSpell(m_creature, SPELL_CAELESTRASZ, true);
                    }

                    m_uiEventTimer = 4000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2200: // Caelestrasz - flies away, Anachronos - speaks
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK10, pAnachronos);

                    Creature* pCaelestrasz = m_creature->GetMap()->GetCreature(m_Caelestrasz);
                    if (pCaelestrasz)
                    {
                        pCaelestrasz->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pCaelestrasz->SetSplineFlags(SPLINEFLAG_FLYING);
                        pCaelestrasz->GetMotionMaster()->MovePoint(1, -8130.f, 1525.f, 47.f, false);
                    }

                    m_uiEventTimer = 5000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2300: // Straghelm - talk
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        DoScriptText(TALK11, pFandral);

                    auto itr = m_bugs.begin();
                    while (itr != m_bugs.end())
                    {
                        TemporarySummon* pBug = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(*itr));
                        if (pBug && pBug->isDead())
                        {
                            pBug->UnSummon();

                            itr = m_bugs.erase(itr);
                            continue;
                        }
                        ++itr;
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2400: // Anachronos - talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK12, pAnachronos);

                    m_uiEventTimer = 500;
                    m_uiEventStage += 100;
                    break;
                }
                case 2500: // Anachronos - run
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pAnachronos->GetMotionMaster()->MovePoint(0, -8105.f, 1523.f, 2.61f, true);
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2600: // Staghelm - run
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pFandral->GetMotionMaster()->MovePoint(0, -8102.f, 1523.f, 2.61, true);
                    }

                    m_uiEventTimer = 10000;
                    m_uiEventStage += 100;
                    break;
                }
                case 2700:
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        pAnachronos->CastSpell(pAnachronos, SPELL_TIME_STOP, false);

                    for (ObjectGuid guid : m_bugs)
                    {
                        Creature* pBug = m_creature->GetMap()->GetCreature(guid);
                        if (pBug)
                           pBug->CastSpell(pBug, SPELL_TIME_STOP_VISUAL, true); 
                    }

                    for (ObjectGuid guid : m_elfs)
                    {
                        Creature* pElf = m_creature->GetMap()->GetCreature(guid);
                        if (pElf)
                           pElf->CastSpell(pElf, SPELL_TIME_STOP_VISUAL, true); 
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 1;
                    break;
                }
                case 2701:
                {
                    for (ObjectGuid guid : m_bugs)
                    {
                        Creature* pBug = m_creature->GetMap()->GetCreature(guid);
                        if (pBug)
                        {
                           pBug->getThreatManager().clearReferences();
                           pBug->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                           pBug->CombatStop(true);
                           pBug->GetMotionMaster()->MoveIdle();
                           pBug->GetMotionMaster()->MovePoint(0, pBug->GetPositionX(), pBug->GetPositionY(), pBug->GetPositionZ(), false);
                           pBug->SetIgnoreNonCombatFlags(false);
                        }
                    }

                    for (ObjectGuid guid : m_elfs)
                    {
                        Creature* pElf = m_creature->GetMap()->GetCreature(guid);
                        if (pElf)
                        {
                           pElf->getThreatManager().clearReferences();
                           pElf->CombatStop(true);
                           pElf->GetMotionMaster()->MovePoint(0, pElf->GetPositionX(), pElf->GetPositionY(), pElf->GetPositionZ(), false);
                        }
                    }

                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        DoScriptText(TALK13, pAnachronos);
                        pAnachronos->CastSpell(pAnachronos, SPELL_CALL_PRISMATIC_BARRIER, false);
                    }
                    
                    m_uiEventTimer = 10000;
                    m_uiEventStage += 1;
                    break;

                }
                case 2702: // Make door spawn.
                {

                    GameObject* pDoor = m_creature->GetMap()->GetGameObject(m_Door);
                    if (pDoor)
                    {
                        pDoor->SetGoState(GO_STATE_READY);
                        pDoor->UpdateVisibilityAndView();
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage = 2800;
                    break;
                }
                case 2800: // Anachronos - use magic
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->CastSpell(pAnachronos, SPELL_CALL_GLYPS_OF_WARDING, false);
                    }


                    m_uiEventTimer = 3000;
                    m_uiEventStage += 1;
                    break;
                }
                case 2801:
                {
                    GameObject* pGlyphs = m_creature->GetMap()->GetGameObject(m_Runes);
                    if (pGlyphs)
                    {
                        pGlyphs->SetGoState(GO_STATE_READY);
                        pGlyphs->UpdateVisibilityAndView();
                    }

                    m_uiEventTimer = 4000;
                    m_uiEventStage = 2900;
                    break;
                }
                case 2900: // Anachronos - talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK14, pAnachronos);

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 100;
                    break;
                }
                case 3000: // Straghelm - talk, magic
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->CastSpell(pFandral, SPELL_CALL_ANCIENTS, false);
                        DoScriptText(TALK15, pFandral);
                    }
                    

                    m_uiEventTimer = 3000;
                    m_uiEventStage += 1;
                    break;
                }
                case 3001:
                {
                    // Restart the background music.
                    m_creature->PlayDirectSound(MUSIC, nullptr);

                    m_uiEventTimer = 4000;
                    m_uiEventStage += 1;
                    break;
                }
                case 3002: // Staghelm - kneel
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->GenericTextEmote("Fandral Staghelm falls to one knee - exhausted.", nullptr, false);
                        pFandral->SetStandState(UNIT_STAND_STATE_KNEEL);
                    }

                    GameObject* pOSummon = m_creature->SummonGameObject(AQ_ROOT, 0, -8124.28f, 1524.88f, 17.19f, -0.023624, GO_STATE_ACTIVE);
                    if (pOSummon)
                    {
                        m_Roots = pOSummon->GetObjectGuid();
                        pOSummon->UpdateVisibilityAndView();
                        pOSummon->SetGoState(GO_STATE_READY);
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage = 3100;
                    break;
                }
                case 3100:
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pFandral && pAnachronos)
                        pAnachronos->SetFacingToObject(pFandral);

                    m_uiEventTimer = 1000;
                    m_uiEventStage = 3200;
                    break;

                }
                case 3200: // Anachronos - talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK16, pAnachronos);

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 3300: // Anachronos - talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK17, pAnachronos);

                    m_uiEventTimer = 4000;
                    m_uiEventStage += 100;
                    break;
                }
                case 3400: // Anachronos - talk
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK18, pAnachronos);

                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        pFandral->SetStandState(UNIT_STAND_STATE_STAND);

                    m_uiEventTimer = 9000;
                    m_uiEventStage += 1;
                    break;
                }
                case 3401:
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        pAnachronos->GenericTextEmote("Anachronos the Ancient hands the Scepter of the Shifting Sands to Fandral Staghelm.", nullptr, false);

                    m_uiEventTimer = 1000;
                    m_uiEventStage = 3402;
                    break;
                }
                case 3402:
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        pAnachronos->HandleEmote(EMOTE_ONESHOT_BEG);

                    m_uiEventTimer = 7000;
                    m_uiEventStage = 3500;
                    break;
                }
                case 3500: // Staghelm - talk
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        DoScriptText(TALK19, pFandral);

                    m_uiEventTimer = 8000;
                    m_uiEventStage += 100;
                    break;
                }
                case 3600: // Staghelm - talk
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        DoScriptText(TALK20, pFandral);

                    m_uiEventTimer = 10000;
                    m_uiEventStage += 100;
                }
                case 3700: // Staghelm - throws thing on ground, glass breaking sound
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->GenericTextEmote("Fandral Staghelm hurls the Scepter of the Shifting Sands into the barrier, shattering it.", nullptr, false);
                        pFandral->CastSpell(-8114.f, 1517.f, 3.8f, SPELL_STAGHELM_BREAK, false);
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage += 1;
                    break;
                }
                case 3701:
                {
                    m_creature->PlayDirectSound(8392, nullptr);
                    m_uiEventTimer = 500;
                    m_uiEventStage = 3800;
                    break;

                }
                case 3800: // Staghelm - walks, Anachronos - speaks
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        DoScriptText(TALK21, pAnachronos);

                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->SetSplineFlags(SPLINEFLAG_WALKMODE);
                        pFandral->GetMotionMaster()->MovePoint(0, -8105.5f, 1521.4f, 2.61f, true);
                    }
                    

                    m_uiEventTimer = 2000;
                    m_uiEventStage += 100;
                    break;
                }
                case 3900: // Fandral Walk back
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->SetSplineFlags(SPLINEFLAG_WALKMODE);
                        pFandral->GetMotionMaster()->MovePoint(0, -8100.f, 1522.f, 2.64f, true);
                    }

                    m_uiEventTimer = 2800;
                    m_uiEventStage += 100;
                    break;
                }
                case 4000:
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pFandral && pAnachronos)
                        pFandral->SetFacingToObject(pAnachronos);

                    m_uiEventTimer = 1200;
                    m_uiEventStage += 100;
                    break;
                }
                case 4100: // Staghelm - talk, Monsters - despawn
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                        DoScriptText(TALK22, pFandral);

                    for (ObjectGuid guid : m_bugs)
                    {
                        TemporarySummon* pBug = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(guid));
                        if (pBug)
                            pBug->UnSummon();
                    }

                    for (ObjectGuid guid : m_elfs)
                    {
                        TemporarySummon* pElf = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(guid));
                        if (pElf)
                            pElf->UnSummon();
                    }

                    m_uiEventTimer = 7000;
                    m_uiEventStage += 100;
                    break;
                }
                case 4200: // Staghelm - walks off
                {
                    Creature* pFandral = m_creature->GetMap()->GetCreature(m_Fandral);
                    if (pFandral)
                    {
                        pFandral->SetSplineFlags(SPLINEFLAG_WALKMODE);
                        pFandral->GetMotionMaster()->MovePoint(1, -8071.f, 1524.f, 2.61f, true);
                    }

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 1;
                    break;
                }
                case 4201: // Anachronos - Shakes his head
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->GenericTextEmote("Anachronos the Ancient shakes his head in disappointment.", nullptr, false);
                        pAnachronos->HandleEmote(EMOTE_ONESHOT_NO);
                    }

                    m_uiEventTimer = 3000;
                    m_uiEventStage = 4202;
                    break;
                }
                case 4202: // Anachronos - walks to pick up the thingie.
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->SetSplineFlags(SPLINEFLAG_WALKMODE);
                        pAnachronos->GetMotionMaster()->MovePoint(0, -8114.f, 1517.f, 3.1f, true);
                    }

                    m_uiEventTimer = 1000;
                    m_uiEventStage = 4300;
                    break;
                }
                case 4300: // Anachronos - picks up thingie, Staghelm - despawn
                {
                    TemporarySummon* pFandral = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(m_Fandral));
                    if (pFandral)
                        pFandral->UnSummon();

                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->GenericTextEmote("Anachronos the Ancient kneels down to pick up the fragments of the shattered scepter.", nullptr, false);
                        pAnachronos->SetStandState(UNIT_STAND_STATE_KNEEL);
                    }
                    
                    m_uiEventTimer = 3000;
                    m_uiEventStage += 1;
                    break;
                }
                case 4301: // Anachronos - walks in front of port
                {

                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->SetStandState(UNIT_STAND_STATE_STAND);
                        pAnachronos->GetMotionMaster()->MovePoint(0, -8113.f, 1525.f, 3.03f, true);
                    }

                    m_uiEventTimer = 4000;
                    m_uiEventStage += 1;
                    break;
                }
                case 4302: // Anachronos - walks more in front of port
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        pAnachronos->GetMotionMaster()->MovePoint(0, -8114.f, 1533.f, 3.26f, true);

                    m_uiEventTimer = 5000;
                    m_uiEventStage = 4400;
                    break;
                }
                case 4400: // Anachronos - talks, Complete the quest for the player.
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        DoScriptText(TALK23, pAnachronos);

                        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_Player);
                        if (pPlayer)
                        {
                            pAnachronos->SetFacingToObject(pPlayer);
                            if (pPlayer->isAlive() && pPlayer->IsWithinDistInMap(pAnachronos, 100.f, false))
                                pPlayer->CompleteQuest(Q_A_PAWN_ON_THE_ETERNAL_BOARD);
                        }
                    }

                    m_uiEventTimer = 7000;
                    m_uiEventStage += 100;
                    break;
                }
                case 4500: // Anachronos - says "well done" in audio
                {
                    GameObject* pDoor = dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(m_Door));
                    if (pDoor)
                        pDoor->Delete();

                    GameObject* pRoots = dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(m_Roots));
                    if (pRoots)
                        pRoots->Delete();

                    GameObject* pRunes = dynamic_cast<TemporaryGameObject*>(m_creature->GetMap()->GetGameObject(m_Runes));
                    if (pRunes)
                        pRunes->Delete();

                    m_uiEventTimer = 1000;
                    m_uiEventStage += 100;
                }
                case 4600: // Anachronos - runs off
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pAnachronos->GetMotionMaster()->MovePoint(0, -8059.f, 1483.f, 2.62f, true);
                    }

                    m_uiEventTimer = 9000;
                    m_uiEventStage += 100;
                    break;
                }
                case 4700: // Anachronos - turns into a dragon
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                        pAnachronos->CastSpell(pAnachronos, SPELL_BRONZE_DRAGON, false);

                    m_uiEventTimer = 6000;
                    m_uiEventStage += 100;
                    break;
                }
                case 4800: // Anachronos - flies off
                {
                    Creature* pAnachronos = m_creature->GetMap()->GetCreature(m_Anachronos);
                    if (pAnachronos)
                    {
                        pAnachronos->SetSplineFlags(SPLINEFLAG_FLYING);
                        pAnachronos->GetMotionMaster()->MovePoint(1, -7961.f, 1466.f, 42.f, false);
                    }

                    m_uiEventTimer = 12000;
                    m_uiEventStage += 100;
                    break;
                }
                case 4900: // Anachronos - despawn
                {
                    TemporarySummon* pAnachronos = dynamic_cast<TemporarySummon*>(m_creature->GetMap()->GetCreature(m_Anachronos));
                    if (pAnachronos)
                        pAnachronos->UnSummon();
                }
        }
    }

    void SpawnBugs()
    {
        for (short i = 0; i < 5; i++)
        {
            float x = m_creature->GetPositionX() + frand(0, 8) * cosf(frand(0, 6));
            float y = m_creature->GetPositionY() + frand(0, 8) * sinf(frand(0, 6));

            Creature* pSummon = m_creature->SummonCreature(QIRAJI_TANK, x, y, 3.f, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            if (pSummon)
            {
                pSummon->GetMotionMaster()->MoveRandom();
                pSummon->setFaction(310);
                m_bugs.push_back(pSummon->GetObjectGuid());
                pSummon->SetIgnoreNonCombatFlags(true);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }

        for (short i = 0; i < 8; i++)
        {
            float x = m_creature->GetPositionX() + frand(0, 8) * cosf(frand(0, 6));
            float y = m_creature->GetPositionY() + frand(0, 8) * sinf(frand(0, 6));

            Creature* pSummon = m_creature->SummonCreature(QIRAJI_DRONE, x, y, 3.f, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            if (pSummon)
            {
                pSummon->GetMotionMaster()->MoveRandom();
                pSummon->setFaction(310);
                m_bugs.push_back(pSummon->GetObjectGuid());
                pSummon->SetIgnoreNonCombatFlags(true);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }

        for (short i = 0; i < 11; i++)
        {
            float x = m_creature->GetPositionX() + frand(0, 8) * cosf(frand(0, 6));
            float y = m_creature->GetPositionY() + frand(0, 8) * sinf(frand(0, 6));

            Creature* pSummon = m_creature->SummonCreature(QIRAJI_WASP, x, y, 3.f, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            if (pSummon)
            {
                pSummon->GetMotionMaster()->MoveRandom();
                pSummon->setFaction(310);
                m_bugs.push_back(pSummon->GetObjectGuid());
                pSummon->SetIgnoreNonCombatFlags(true);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }
    }

    void AddThreat()
    {
        for (ObjectGuid guid : m_elfs)
        {
            Unit* pElf = m_creature->GetMap()->GetCreature(guid);
            if (pElf)
            {
                // Shuffle the target list so they don't all attack the same creature.
                std::random_shuffle(m_bugs.begin(), m_bugs.end());
                for (ObjectGuid guid : m_bugs)
                {
                    Unit* pBug = m_creature->GetMap()->GetCreature(guid);
                    if (pBug)
                    {
                        pElf->Attack(pBug, true);
                        pElf->getThreatManager().addThreatDirectly(pBug, 10000000.f);
                    }
                }
            }
        }

        for (ObjectGuid guid : m_bugs)
        {
            Unit* pBug = m_creature->GetMap()->GetCreature(guid);
            if (pBug)
            {
                // Shuffle the target list so they don't all attack the same creature.
                std::random_shuffle(m_elfs.begin(), m_elfs.end());
                for (ObjectGuid guid : m_elfs)
                {
                    Unit* pElf = m_creature->GetMap()->GetCreature(guid);
                    if (pElf)
                    {
                        pBug->Attack(pElf, true);
                        pBug->getThreatManager().addThreatDirectly(pElf, 10000000.f);
                    }
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_anachronos_trigger(Creature* pCreature)
{
    return new npc_anachronos_triggerAI(pCreature);
}

bool QuestAccept_crystalline_tear(Player* pPlayer, GameObject* pGO, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == Q_A_PAWN_ON_THE_ETERNAL_BOARD)
    {
        // Don't allow two RPs to run at the same time.
        Creature* pCreature = GetClosestCreatureWithEntry(pGO, ANACHRONOS_TRIGGER, 100.f);
        if (pCreature)
            return true;

        Creature* pSummon = pGO->SummonCreature(ANACHRONOS_TRIGGER, -8087.f, 1522.f, 3.f, 0.f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000, true);
        if (pSummon)
        {
            npc_anachronos_triggerAI* pAI = dynamic_cast<npc_anachronos_triggerAI*>(pSummon->AI());
            if (pAI)
                pAI->m_Player = pPlayer->GetObjectGuid();
        }
    }

    return false;
}


struct MANGOS_DLL_DECL npc_dragon_flightAI : public ScriptedAI
{
    npc_dragon_flightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
    }

    void MovementInform(uint32 /*uiMovementType*/, uint32 uiPointId)
    {
        if (uiPointId == 1)
        {
            // Make the dragon fly to the despawn point.
            m_creature->GetMotionMaster()->MovePoint(2, -8201.f, 1511.f, 34.f, false);
        }
        else if (uiPointId == 2)
        {
            TemporarySummon* pCreature = dynamic_cast<TemporarySummon*>(m_creature);
            if (pCreature)
                pCreature->UnSummon();
        }
    }
};

struct MANGOS_DLL_DECL npc_merithraAI : public npc_dragon_flightAI 
{
    npc_merithraAI(Creature* pCreature) : npc_dragon_flightAI(pCreature) {}

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_merithra(Creature* pCreature)
{
    return new npc_merithraAI(pCreature);
}

struct MANGOS_DLL_DECL npc_arygosAI: public npc_dragon_flightAI 
{
    npc_arygosAI(Creature* pCreature) : npc_dragon_flightAI(pCreature) {}

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/) override final
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_arygos(Creature* pCreature)
{
    return new npc_arygosAI(pCreature);
}

struct MANGOS_DLL_DECL npc_caelestraszAI: public npc_dragon_flightAI 
{
    npc_caelestraszAI(Creature* pCreature) : npc_dragon_flightAI(pCreature) {}

    void Reset()
    {
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_caelestrasz(Creature* pCreature)
{
    return new npc_caelestraszAI(pCreature);
}

void AddSC_silithus()
{
    Script* pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_abyssal_council";
    pNewscript->GetAI = &GetAI_boss_abyssal_council;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_cenarion_hold_infantry";
    pNewscript->GetAI = &GetAI_npc_cenarion_hold_infantry;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_wind_stone";
    pNewscript->pGossipHelloGO = &GossipHello_go_wind_stone;
    pNewscript->pGossipSelectGO = &GossipSelect_go_wind_stone;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_hiveashi_pod";
    pNewscript->pGOUse = &GOUse_go_hiveashi_pod;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_silithus_hive_crystal";
    pNewscript->pGOUse = &GOUse_go_silithus_hive_crystal;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_cenarion_scout";
    pNewscript->pGossipHello = &GossipHello_npc_cenarion_scout;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_highlord_demitrian";
    pNewscript->pGossipHello = &GossipHello_npc_highlord_demitrian;
    pNewscript->pGossipSelect = &GossipSelect_npc_highlord_demitrian;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npcs_rutgar_and_frankal";
    pNewscript->pGossipHello = &GossipHello_npcs_rutgar_and_frankal;
    pNewscript->pGossipSelect = &GossipSelect_npcs_rutgar_and_frankal;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_the_calling_event";
    pNewscript->GetAI = &GetAI_npc_the_calling_event;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_geologist_larksbane";
    pNewscript->GetAI = &GetAI_npc_geologist_larksbane;
    pNewscript->pQuestRewardedNPC = &OnQuestRewarded_npc_geologist_larksbane;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "boss_roman_khan";
    pNewscript->GetAI = &GetAI_boss_roman_khan;
    pNewscript->RegisterSelf();

	pNewscript = new Script;
    pNewscript->Name = "npc_mistress_natalia_maralith";
    pNewscript->GetAI = &GetAI_npc_mistress_natalia_maralith;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "go_crystalline_tear";
    pNewscript->pQuestAcceptGO = &QuestAccept_crystalline_tear;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_anachronos_trigger";
    pNewscript->GetAI = &GetAI_npc_anachronos_trigger;
    pNewscript->RegisterSelf();
    
    pNewscript = new Script;
    pNewscript->Name = "npc_merithra";
    pNewscript->GetAI = &GetAI_npc_merithra;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_arygos";
    pNewscript->GetAI = &GetAI_npc_arygos;
    pNewscript->RegisterSelf();

    pNewscript = new Script;
    pNewscript->Name = "npc_caelestrasz";
    pNewscript->GetAI = &GetAI_npc_caelestrasz;
    pNewscript->RegisterSelf();
}
