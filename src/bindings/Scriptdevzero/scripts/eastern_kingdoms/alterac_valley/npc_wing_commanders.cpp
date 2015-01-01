#include "precompiled.h"
#include "patrol_ai.h"
#include "BattleGroundAV.h"
#include "Language.h"

void GiveItem(uint32 itemID, Player* player)
{
    ItemPosCountVec dest;
    if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemID, 1) == EQUIP_ERR_OK)
    {
        Item* item = player->StoreNewItem( dest, itemID, true, Item::GenerateItemRandomPropertyId(itemID));
        player->SendNewItem(item, 1, true, false);
    }
    else
        player->GetSession()->SendNotification(LANG_NOT_FREE_TRADE_SLOTS);
}

enum
{
    SPELL_REND								= 16509,
    SPELL_STRIKE							= 15580,

    GOSSIP_EASTERN_BEACON					= 6107,
    GOSSIP_WESTERN_BEACON					= 6110,
    GOSSIP_GIVE_BEACON						= -3000006,
    GOSSIP_LAUNCH_AIRSTRIKE					= -3000007
};


enum
{
    SPELL_SUNDER_ARMOR						= 15572,

    MULVERICK_BEACON						= 17323,

    GOSSIP_MULVERICK_SAY_RESCUE				= 6101,
    GOSSIP_MULVERICK_RESPONSE_RESCUE		= -3000003
};


struct MANGOS_DLL_DECL npc_wing_commander_mulverickAI : public npc_patrolAI
{
    uint32 m_uiStrikeTimer;
    uint32 m_uiSunderArmorTimer;

    bool m_bSaved;

    npc_wing_commander_mulverickAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiStrikeTimer = urand(4100, 9300);
        m_uiSunderArmorTimer = urand(5000, 9000);
    }

    void FinishedPatrol()
    {
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

			m_creature->SetFacingTo(0.852f);

            m_bSaved = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(10200, 17900);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);

            m_uiSunderArmorTimer = urand(5000, 9000);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_mulverick(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1: // Start rescue.
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_mulverickAI* pAI = dynamic_cast<npc_wing_commander_mulverickAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(MULVERICK_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(5);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_mulverick(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    npc_wing_commander_mulverickAI *pAI = dynamic_cast<npc_wing_commander_mulverickAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 3) >= 30)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_H) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_WESTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_MULVERICK_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_MULVERICK_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;

}

bool QuestRewarded_npc_wing_commander_mulverick(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 3) >= 30)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}


CreatureAI* GetAI_npc_wing_commander_mulverick(Creature* pCreature)
{
    return new npc_wing_commander_mulverickAI(pCreature);
}


enum
{
    SPELL_GREATER_INVISIBILITY		= 16380,

    ICHMAN_BEACON					= 17505,

    GOSSIP_ICHMAN_SAY_RESCUE		= 6178,
    GOSSIP_ICHMAN_RESPONSE_RESCUE	= -3000005
};


struct MANGOS_DLL_DECL npc_wing_commander_ichmanAI : public npc_patrolAI
{

    uint32 m_uiRendTimer;
    uint32 m_uiStrikeTimer;

    bool m_bSaved;

    npc_wing_commander_ichmanAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        m_creature->CastSpell(m_creature, SPELL_GREATER_INVISIBILITY, true);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiRendTimer = urand(9000, 13000);
        m_uiStrikeTimer = urand(4000, 6000);
    }

    void Aggro(Unit* /*attacker*/)
    {
        if (m_creature->HasAura(SPELL_GREATER_INVISIBILITY))
            m_creature->RemoveAura(SPELL_GREATER_INVISIBILITY, EFFECT_INDEX_0);
    }

    void FinishedPatrol()
    {
        m_bSaved = true;

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRendTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);

            m_uiRendTimer = urand(18000, 34000);
        }
        else
            m_uiRendTimer -= uiDiff;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);

            m_uiStrikeTimer = urand(7000, 9000);
        }
        else
            m_uiStrikeTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_ichman(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_ichmanAI* pAI = dynamic_cast<npc_wing_commander_ichmanAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(ICHMAN_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(4);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_ichman(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    npc_wing_commander_ichmanAI *pAI = dynamic_cast<npc_wing_commander_ichmanAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 3) >= 30)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_A) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_WESTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ICHMAN_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_ICHMAN_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;
}

bool QuestRewarded_npc_wing_commander_ichman(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 3) >= 30)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}

CreatureAI* GetAI_npc_wing_commander_ichman(Creature* pCreature)
{
    return new npc_wing_commander_ichmanAI(pCreature);
}

enum
{
    SLIDORE_BEACON					= 17507,

    GOSSIP_SLIDORE_SAY_RESCUE		= 6180,
    GOSSIP_SLIDORE_RESPONSE_RESCUE	= -3000004
};

struct MANGOS_DLL_DECL npc_wing_commander_slidoreAI : public npc_patrolAI
{
    uint32 m_uiRendTimer;
    uint32 m_uiStrikeTimer;

    bool m_bSaved;

    npc_wing_commander_slidoreAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiRendTimer = urand(9000, 13000);
        m_uiStrikeTimer = urand(4000, 6000);
    }

    void FinishedPatrol()
    {
        m_bSaved = true;

		m_creature->SetFacingTo(4.704f);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRendTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);

            m_uiRendTimer = urand(18000, 34000);
        }
        else
            m_uiRendTimer -= uiDiff;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);

            m_uiStrikeTimer = urand(7000, 9000);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_slidore(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_slidoreAI* pAI = dynamic_cast<npc_wing_commander_slidoreAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(SLIDORE_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(0);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_slidore(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    npc_wing_commander_slidoreAI *pAI = dynamic_cast<npc_wing_commander_slidoreAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 1) >= 120)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_A) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_EASTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_SLIDORE_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_SLIDORE_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;
}

bool QuestRewarded_npc_wing_commander_slidore(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 1) >= 120)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}


CreatureAI* GetAI_npc_wing_commander_slidore(Creature* pCreature)
{
    return new npc_wing_commander_slidoreAI(pCreature);
}


enum
{
    GUSE_BEACON					= 17324,

    GOSSIP_GUSE_SAY_RESCUE		= 6095,
    GOSSIP_GUSE_RESPONSE_RESCUE = -3000001

};


struct MANGOS_DLL_DECL npc_wing_commander_guseAI : public npc_patrolAI
{
    uint32 m_uiStrikeTimer;
    uint32 m_uiSunderArmorTimer;

    bool m_bSaved;

    npc_wing_commander_guseAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        m_creature->GetMotionMaster()->MoveTaxiFlight(18, 0);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiStrikeTimer = urand(4100, 9300);
        m_uiSunderArmorTimer = urand(5000, 9000);
    }

    void FinishedPatrol()
    {
        m_bSaved = true;

		m_creature->SetFacingTo(1.830f);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(10200, 17900);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);

            m_uiSunderArmorTimer = urand(5000, 9000);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_guse(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_guseAI* pAI = dynamic_cast<npc_wing_commander_guseAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(GUSE_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(1);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_guse(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    npc_wing_commander_guseAI *pAI = dynamic_cast<npc_wing_commander_guseAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 1) >= 120)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_H) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_EASTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GUSE_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_GUSE_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;
}

bool QuestRewarded_npc_wing_commander_guse(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 1) >= 120)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}

CreatureAI* GetAI_npc_wing_commander_guse(Creature* pCreature)
{
    return new npc_wing_commander_guseAI(pCreature);
}

enum
{
    VIPORE_BEACON					= 17506,

    GOSSIP_VIPORE_SAY_RESCUE		= 6179,
    GOSSIP_VIPORE_RESPONSE_RESCUE	= -3000005
};

struct MANGOS_DLL_DECL npc_wing_commander_viporeAI : public npc_patrolAI
{
    uint32 m_uiRendTimer;
    uint32 m_uiStrikeTimer;

    bool m_bSaved;

    npc_wing_commander_viporeAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_SIT);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiRendTimer = urand(9000, 13000);
        m_uiStrikeTimer = urand(4000, 6000);
    }

    void FinishedPatrol()
    {
        m_bSaved = true;

		m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiRendTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_REND);

            m_uiRendTimer = urand(18000, 34000);
        }
        else
            m_uiRendTimer -= uiDiff;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);

            m_uiStrikeTimer = urand(7000, 9000);
        }
        else
            m_uiStrikeTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_vipore(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_viporeAI* pAI = dynamic_cast<npc_wing_commander_viporeAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->SetStandState(UNIT_STAND_STATE_STAND);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetDisplayId(633);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(VIPORE_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(2);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_vipore(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    npc_wing_commander_viporeAI *pAI = dynamic_cast<npc_wing_commander_viporeAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 2) >= 60)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_A) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_WESTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_VIPORE_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_VIPORE_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;
}

bool QuestRewarded_npc_wing_commander_vipore(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_ALLIANCE, 2) >= 60)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}

CreatureAI* GetAI_npc_wing_commander_vipore(Creature* pCreature)
{
    return new npc_wing_commander_viporeAI(pCreature);
}


enum
{
    JEZTOR_BEACON					= 17325,

    GOSSIP_JEZTOR_SAY_RESCUE		= 6096,
    GOSSIP_JEZTOR_RESPONSE_RESCUE	= -3000002
};


struct MANGOS_DLL_DECL npc_wing_commander_jeztorAI : public npc_patrolAI
{
    uint32 m_uiStrikeTimer;
    uint32 m_uiSunderArmorTimer;

    bool m_bSaved;

    npc_wing_commander_jeztorAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, true)
    {
        m_bSaved = false;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetStandState(UNIT_STAND_STATE_SIT);

        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiStrikeTimer = urand(4100, 9300);
        m_uiSunderArmorTimer = urand(5000, 9000);
    }

    void FinishedPatrol()
    {
        m_bSaved = true;

		m_creature->SetFacingTo(6.248f);

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStrikeTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrikeTimer = urand(10200, 17900);
        }
        else
            m_uiStrikeTimer -= uiDiff;

        if (m_uiSunderArmorTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);

            m_uiSunderArmorTimer = urand(5000, 9000);
        }
        else
            m_uiSunderArmorTimer -= uiDiff;


        DoMeleeAttackIfReady();
    }

};

bool GossipSelect_npc_wing_commander_jeztor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        npc_wing_commander_jeztorAI* pAI = dynamic_cast<npc_wing_commander_jeztorAI*>(pCreature->AI());

        if (pAI)
            pAI->StartPatrol(0);

        pCreature->SetStandState(UNIT_STAND_STATE_STAND);
		pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 2: // Give a beacon.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        GiveItem(JEZTOR_BEACON, pPlayer);
        break;
    }
    case GOSSIP_ACTION_INFO_DEF + 3: // Launch the airstrike.
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

        if (pAV)
            pAV->SetAirstrikeLaunched(3);

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetVisibility(VISIBILITY_OFF);

        break;
    }
    }

    return true;
}

bool GossipHello_npc_wing_commander_jeztor(Player* pPlayer, Creature* pCreature)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    npc_wing_commander_jeztorAI *pAI = dynamic_cast<npc_wing_commander_jeztorAI*>(pCreature->AI());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 2) >= 60)
    {
        if (pCreature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

        if (pPlayer->GetReputationRank(BG_AV_FACTION_H) >= REP_HONORED)
        {
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_GIVE_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_LAUNCH_AIRSTRIKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        }

        pPlayer->SEND_GOSSIP_MENU(GOSSIP_WESTERN_BEACON, pCreature->GetObjectGuid());

        return true;
    }
    else if (pAI && !pAI->m_bSaved)
    {
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_JEZTOR_RESPONSE_RESCUE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_JEZTOR_SAY_RESCUE, pCreature->GetObjectGuid());

        return true;
    }

    return false;
}

bool QuestRewarded_npc_wing_commander_jeztor(Player *pPlayer, Creature *pCreature, const Quest* /*pQuest*/)
{
    BattleGroundAV *pBG = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());

    if (pBG && pBG->GetWingCommanderQuestStatus(BG_AV_TEAM_HORDE, 2) >= 60)
    {
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    return false;
}

CreatureAI* GetAI_npc_wing_commander_jeztor(Creature* pCreature)
{
    return new npc_wing_commander_jeztorAI(pCreature);
}

/*##################
 * Beacon handling #
 * ################*/

bool ProcessEventId_event_av_beacon(uint32 uiEventId, Object* pSource, Object* /*pTarget*/, bool /*bIsStart*/)
{
    Player *player = dynamic_cast<Player*>(pSource);
    if (player)
    {
        BattleGroundAV *pAV = dynamic_cast<BattleGroundAV*>(player->GetBattleGround());
        if (pAV)
            pAV->m_vecBeaconEventQueue.push_back(uiEventId);
    }

    return true;
}

bool GOUse_beacon (Player* pPlayer, GameObject* pGameobject)
{
    BattleGroundAV* pAV = dynamic_cast<BattleGroundAV*>(pPlayer->GetBattleGround());
    uint32 required_team;
    
    switch (pGameobject->GetEntry())
    {
      case BG_AV_BEACON_MULVERICK:
      case BG_AV_BEACON_GUSE:
      case BG_AV_BEACON_JEZTOR:
	required_team = ALLIANCE;
	break;
      default:
	required_team = HORDE;
	break;
    }
    
    // Do not allow a player to disable a beacon from its own faction.
    if (pPlayer->GetTeam() != required_team)
      return true;
      
    
    if (pAV)
        pAV->m_BeaconTimerMap[pGameobject->GetEntry()] = 0;

    return false;
}

/*################
 * Bird handling #
 * #############*/


/* War Gryphon master class with abilities. Inherit from this class to customize the different paths. */

// TODO: Add correct spell timers.

enum BirdValues
{
    SPELL_BIRD_FIREBALL				= 22088,
    SPELL_BIRD_FIREBALL_VOLLEY		= 15285, // Targets all nearby enemies.
    SPELL_BIRD_STUN_BOMB			= 21188,

	BIRD_WAYPOINT_CIRCLE			= 5000
};

struct MANGOS_DLL_DECL npc_airstrike_birdAI : public npc_patrolAI
{
    uint32 m_uiFireballTimer, m_uiFirebollVolleyTimer, m_uiStunBombTimer;
	bool m_bAttackMode;
	float m_fCircleAngle; // Where on the circle around the bird's victim it currently is.
	Player* m_pAttackingPlayer;

    npc_airstrike_birdAI(Creature* pCreature) : npc_patrolAI(pCreature, 0.f, false) 
    {
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
		SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
		npc_patrolAI::Reset();

        m_uiFireballTimer = 3000;			// These are just place holder timers.
        m_uiFirebollVolleyTimer = 20000;
        m_uiStunBombTimer = 20000;

		m_pAttackingPlayer = nullptr;
		m_bAttackMode = false;

		m_fCircleAngle = 0.f;
	}

    void UpdateAI(const uint32 uiDiff)
    {
		if (m_bAttackMode)
		{
			if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			{
				if (m_pAttackingPlayer) // If we cannot find a target through the normal targetting process we see if we can get the attacking target.
				{
					if (!m_pAttackingPlayer->isInAccessablePlaceFor(m_creature) || !m_pAttackingPlayer->isAlive())
					{
						m_pAttackingPlayer = nullptr;
						Reset();
						return;
					} 
					else
					{
						m_creature->Attack(m_pAttackingPlayer, false);

						// Calculate a point on a circle with the diameter 20 yd centered around the victim.
						if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != MovementGeneratorType::POINT_MOTION_TYPE)
							m_creature->GetMotionMaster()->MovePoint(BIRD_WAYPOINT_CIRCLE, m_pAttackingPlayer->GetPositionX() + 10.f * cos(m_fCircleAngle), 
																	 m_pAttackingPlayer->GetPositionY() + 10.f * sin(m_fCircleAngle), m_pAttackingPlayer->GetPositionZ() + 10.f, false);
					}
				}
				else
					return;
			}

			if (m_uiFireballTimer <= uiDiff)
			{
				DoCastSpellIfCan(m_creature->getVictim(), SPELL_BIRD_FIREBALL, true);

				m_uiFireballTimer = 3000; // Place holder timer.
			}
			else
				m_uiFireballTimer -= uiDiff;
		}
		else
		{
			if (m_uiFirebollVolleyTimer <= uiDiff)
			{
				m_creature->CastSpell(m_creature, SPELL_BIRD_FIREBALL_VOLLEY, true);

				m_uiFirebollVolleyTimer = 20000; // Place holder timer.
			}
			else
				m_uiFirebollVolleyTimer -= uiDiff;

			if (m_uiStunBombTimer <= uiDiff)
			{
				Player* player_target = GetRandomPlayerInCurrentMap(100.f);

				if (player_target && player_target->IsHostileTo(m_creature))
				{
					m_creature->CastSpell(player_target, SPELL_BIRD_STUN_BOMB, true);

					m_uiStunBombTimer = 20000; // Place holder timer.
				}

			}
			else
				m_uiStunBombTimer -= uiDiff;
		}
	}

	void DamageTaken(Unit* pDealer, uint32& /*uiDamage*/)
	{
		if (!m_bAttackMode)
		{
			m_pAttackingPlayer = dynamic_cast<Player*>(pDealer);
			if (!m_pAttackingPlayer)
				return;

			m_bAttackMode = true;

			m_creature->GetMotionMaster()->Clear(true);

			// Calculate the first point on a circle with the diameter 20 yd centered around the victim.
			m_creature->GetMotionMaster()->MovePoint(BIRD_WAYPOINT_CIRCLE, pDealer->GetPositionX() + 10.f * cos(m_fCircleAngle), pDealer->GetPositionY() + 10.f * sin(m_fCircleAngle), pDealer->GetPositionZ() + 10.f, false);
		}
	}

	void MovementInform(uint32 uiMovementType, uint32 uiPointID)
	{
		if (uiPointID == BIRD_WAYPOINT_CIRCLE)
		{
			Unit* target = nullptr;

			if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
			{
				if (m_pAttackingPlayer) // If we cannot find a target through the normal targetting process we see if we can get the attacking target.
				{
					if (!m_pAttackingPlayer->isInAccessablePlaceFor(m_creature) || !m_pAttackingPlayer->isAlive())
					{
						m_pAttackingPlayer = nullptr;
						Reset();
						return;
					} 
					else
					{
						target = m_pAttackingPlayer;
					}
				}
				else
				{
					Reset();
					return;
				}

			}
			else
				target = m_creature->getVictim();

			m_fCircleAngle += m_fCircleAngle > 2 * PI ? -2.f * PI : PI / 7.f;

			// Move to the next point on the circle.
			m_creature->GetMotionMaster()->MovePoint(BIRD_WAYPOINT_CIRCLE, target->GetPositionX() + 10.f * cos(m_fCircleAngle), 
															target->GetPositionY() + 10.f * sin(m_fCircleAngle), target->GetPositionZ() + 10.f, false);
			
		}
		else
			npc_patrolAI::MovementInform(uiMovementType, uiPointID);
		
	}

};

struct MANGOS_DLL_DECL npc_mulvericks_war_riderAI : public npc_airstrike_birdAI
{
    npc_mulvericks_war_riderAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(105);
	}
};

CreatureAI* GetAI_npc_mulvericks_war_rider(Creature* pCreature)
{
    return new npc_mulvericks_war_riderAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ichmans_gryphonAI : public npc_airstrike_birdAI
{
    npc_ichmans_gryphonAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(60);
	}
};

CreatureAI* GetAI_npc_ichmans_gryphon (Creature* pCreature)
{
    return new npc_ichmans_gryphonAI(pCreature);
}

struct MANGOS_DLL_DECL npc_slidores_gryphonAI : public npc_airstrike_birdAI
{
    npc_slidores_gryphonAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(39);
	}
};

CreatureAI* GetAI_npc_slidores_gryphon (Creature* pCreature)
{
    return new npc_slidores_gryphonAI(pCreature);
}

struct MANGOS_DLL_DECL npc_guses_war_riderAI : public npc_airstrike_birdAI
{
    npc_guses_war_riderAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(70);
	}
};

CreatureAI* GetAI_npc_guses_war_rider (Creature* pCreature)
{
    return new npc_guses_war_riderAI(pCreature);
}

struct MANGOS_DLL_DECL npc_vipores_gryphonAI : public npc_airstrike_birdAI
{
    npc_vipores_gryphonAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(49);
	}

};

CreatureAI* GetAI_npc_vipores_gryphon (Creature* pCreature)
{
    return new npc_vipores_gryphonAI(pCreature);
}

struct MANGOS_DLL_DECL npc_jeztors_war_riderAI : public npc_airstrike_birdAI
{
    npc_jeztors_war_riderAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(96);
	}
};

CreatureAI* GetAI_npc_jeztors_war_rider (Creature* pCreature)
{
    return new npc_jeztors_war_riderAI(pCreature);
}

struct MANGOS_DLL_DECL npc_mulvericks_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_mulvericks_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_mulvericks_beacon_bird (Creature* pCreature)
{
    return new npc_mulvericks_beacon_birdAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ichmans_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_ichmans_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_ichmans_beacon_bird (Creature* pCreature)
{
    return new npc_ichmans_beacon_birdAI(pCreature);
}

struct MANGOS_DLL_DECL npc_slidores_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_slidores_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_slidores_beacon_bird (Creature* pCreature)
{
    return new npc_slidores_beacon_birdAI(pCreature);
}

struct MANGOS_DLL_DECL npc_guses_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_guses_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_guses_beacon_bird (Creature* pCreature)
{
    return new npc_guses_beacon_birdAI(pCreature);
}

struct MANGOS_DLL_DECL npc_vipores_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_vipores_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_vipores_beacon_bird (Creature* pCreature)
{
    return new npc_vipores_beacon_birdAI(pCreature);
}

struct MANGOS_DLL_DECL npc_jeztors_beacon_birdAI : public npc_airstrike_birdAI
{
    npc_jeztors_beacon_birdAI(Creature* pCreature) : npc_airstrike_birdAI(pCreature) {}

	void JustRespawned()
	{
		StartPatrol(1);
	}
};

CreatureAI* GetAI_npc_jeztors_beacon_bird (Creature* pCreature)
{
    return new npc_jeztors_beacon_birdAI(pCreature);
}

void AddSC_npc_wing_commanders()
{
    Script* pScript = new Script;

	// Wing Commander AIs
    pScript->GetAI = &GetAI_npc_wing_commander_mulverick;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_mulverick;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_mulverick;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_mulverick;
    pScript->Name = "wing_commander_mulverick";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_wing_commander_ichman;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_ichman;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_ichman;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_ichman;
    pScript->Name = "wing_commander_ichman";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_wing_commander_slidore;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_slidore;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_slidore;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_slidore;
    pScript->Name = "wing_commander_slidore";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_wing_commander_guse;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_guse;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_guse;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_guse;
    pScript->Name = "wing_commander_guse";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_wing_commander_vipore;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_vipore;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_vipore;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_vipore;
    pScript->Name = "wing_commander_vipore";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_wing_commander_jeztor;
    pScript->pGossipHello = &GossipHello_npc_wing_commander_jeztor;
    pScript->pGossipSelect = &GossipSelect_npc_wing_commander_jeztor;
    pScript->pQuestRewardedNPC = &QuestRewarded_npc_wing_commander_jeztor;
    pScript->Name = "wing_commander_jeztor";
    pScript->RegisterSelf();

	// Beacon Handling
    pScript = new Script;
    pScript->pProcessEventId = &ProcessEventId_event_av_beacon;
    pScript->Name = "event_av_beacon";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->pGOUse = &GOUse_beacon;
    pScript->Name = "go_use_av_beacon";
    pScript->RegisterSelf();

	// Bird AIs
    pScript = new Script;
    pScript->GetAI = &GetAI_npc_mulvericks_war_rider;
    pScript->Name = "npc_mulvericks_war_rider";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_ichmans_gryphon;
    pScript->Name = "npc_ichmans_gryphon";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_slidores_gryphon;
    pScript->Name = "npc_slidores_gryphon";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_guses_war_rider;
    pScript->Name = "npc_guses_war_rider";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_vipores_gryphon;
    pScript->Name = "npc_vipores_gryphon";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_jeztors_war_rider;
    pScript->Name = "npc_jeztors_war_rider";
    pScript->RegisterSelf();

	pScript = new Script;
    pScript->GetAI = &GetAI_npc_mulvericks_beacon_bird;
    pScript->Name = "npc_mulvericks_beacon_bird";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_ichmans_beacon_bird;
    pScript->Name = "npc_ichmans_beacon_bird";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_slidores_beacon_bird;
    pScript->Name = "npc_slidores_beacon_bird";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_guses_beacon_bird;
    pScript->Name = "npc_guses_beacon_bird";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_vipores_beacon_bird;
    pScript->Name = "npc_vipores_beacon_bird";
    pScript->RegisterSelf();

    pScript = new Script;
    pScript->GetAI = &GetAI_npc_jeztors_beacon_bird;
    pScript->Name = "npc_jeztors_beacon_bird";
    pScript->RegisterSelf();
}
