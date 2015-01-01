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
SDComment: Abbysal Council support, Quest support: 1126, 7785, 8304, 8534, 8538, 8539.
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
}
