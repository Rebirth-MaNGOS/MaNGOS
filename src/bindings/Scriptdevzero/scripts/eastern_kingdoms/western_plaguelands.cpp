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
SDName: Western_Plaguelands
SD%Complete: 90
SDComment: Quest support: 5097,5098,5216,5219,5222,5225,5229,5231,5233,5235,5944. To obtain Vitreous Focuser (could use more spesifics about gossip items)
SDCategory: Western Plaguelands
EndScriptData */

/* ContentData
npcs_andorhal_tower
npcs_dithers_and_arbington
npc_eva_sarkhoff
npc_myranda_hag
npc_the_scourge_cauldron
npc_highlord_taelan_fordring
mob_grand_inquisitor_isillien
mob_crimson_elite
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npcs_andorhal_tower 
######*/

enum eAndorhalTower
{
    GO_BEACON_TORCH         = 176093
};

struct MANGOS_DLL_DECL npc_andorhal_towerAI : public Scripted_NoMovementAI
{
    npc_andorhal_towerAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {}
    
    void Reset() {}

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (GetClosestGameObjectWithEntry(m_creature, GO_BEACON_TORCH, 10.0f))
            ((Player*)pWho)->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid());
    }
};

CreatureAI* GetAI_npc_andorhal_tower(Creature* pCreature)
{
    return new npc_andorhal_towerAI (pCreature);
}

/*######
## npcs_dithers_and_arbington
######*/

bool GossipHello_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestRewardStatus(5237) || pPlayer->GetQuestRewardStatus(5238))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Felstone Field Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Dalson's Tears Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Writhing Haunt Cauldron need?",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What does the Gahrron's Withering Cauldron need?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
        pPlayer->SEND_GOSSIP_MENU(3985, pCreature->GetObjectGuid());
    }else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(3980, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(3981, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(3982, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks, i need a Vitreous Focuser", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(3983, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, 17529, false);
            break;
    }
    return true;
}

/*######
## npc_eva_sarkhoff
######*/

enum eEvaSarkhoff
{
    QUEST_KIRTONOS_THE_HERALD       = 5384,
    ITEM_SPECTRAL_ESSENCE           = 13544,
    SPELL_SUMMON_SPECTRAL_ESSENCE   = 17672
};

bool GossipHello_npc_eva_sarkhoff(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestRewardStatus(QUEST_KIRTONOS_THE_HERALD) && !pPlayer->HasItemCount(ITEM_SPECTRAL_ESSENCE, 1, true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I lost my Spectral Essence. Can you give me new one, pls?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_eva_sarkhoff(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_SUMMON_SPECTRAL_ESSENCE, true);
    }
    return true;
}

/*######
## npc_myranda_the_hag
######*/

enum
{
    QUEST_SUBTERFUGE        = 5862,
    QUEST_IN_DREAMS         = 5944,
    SPELL_SCARLET_ILLUSION  = 17961
};

#define GOSSIP_ITEM_ILLUSION    "I am ready for the illusion, Myranda."

bool GossipHello_npc_myranda_the_hag(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_SUBTERFUGE) == QUEST_STATUS_COMPLETE &&
        !pPlayer->GetQuestRewardStatus(QUEST_IN_DREAMS) && !pPlayer->HasAura(SPELL_SCARLET_ILLUSION))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ILLUSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(4773, pCreature->GetObjectGuid());
        return true;
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_myranda_the_hag(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_SCARLET_ILLUSION, false);
    }
    return true;
}

/*######
## npc_the_scourge_cauldron
######*/

struct MANGOS_DLL_DECL npc_the_scourge_cauldronAI : public ScriptedAI
{
    npc_the_scourge_cauldronAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	ObjectGuid CauldronLordGUID;

    void Reset() 
	{
		CauldronLordGUID.Clear();
	}

    void DoDie()
    {
        //summoner dies here
        m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        //override any database `spawntimesecs` to prevent duplicated summons
        uint32 rTime = m_creature->GetRespawnDelay();
        if (rTime<600)
            m_creature->SetRespawnDelay(600);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;

		if (CauldronLordGUID)
		{
			Creature* lord = m_creature->GetMap()->GetCreature(CauldronLordGUID);
			if (lord && lord->IsInWorld() && lord->isAlive())
				return;
		}

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            switch(m_creature->GetAreaId())
            {
                case 199:                                   //felstone
                    if (((Player*)who)->GetQuestStatus(5216) == QUEST_STATUS_INCOMPLETE ||
                        ((Player*)who)->GetQuestStatus(5229) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (Creature* lord = m_creature->SummonCreature(11075, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000))
							CauldronLordGUID = lord->GetObjectGuid();
                        DoDie();
                    }
                    break;
                case 200:                                   //dalson
                    if (((Player*)who)->GetQuestStatus(5219) == QUEST_STATUS_INCOMPLETE ||
                        ((Player*)who)->GetQuestStatus(5231) == QUEST_STATUS_INCOMPLETE)
                    {
                        m_creature->SummonCreature(11077, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        DoDie();
                    }
                    break;
                case 201:                                   //gahrron
                    if (((Player*)who)->GetQuestStatus(5225) == QUEST_STATUS_INCOMPLETE ||
                        ((Player*)who)->GetQuestStatus(5235) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (m_creature->SummonCreature(11078, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
                        DoDie();
                    }
                    break;
                case 202:                                   //writhing
                    if (((Player*)who)->GetQuestStatus(5222) == QUEST_STATUS_INCOMPLETE ||
                        ((Player*)who)->GetQuestStatus(5233) == QUEST_STATUS_INCOMPLETE)
                    {
                        if (m_creature->SummonCreature(11076, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,600000))
                        DoDie();
                    }
                    break;
            }
        }
    }
};
CreatureAI* GetAI_npc_the_scourge_cauldron(Creature* pCreature)
{
    return new npc_the_scourge_cauldronAI(pCreature);
}

/*######
## go_loose_dirt_mound
######*/

enum eLooseDirtMound
{
	QUEST_LOOSE_DIRT_MOUND   = 5781,
	NPC_MERCUTIO_FILTHGORGER = 11886,
	NPC_CRYPT_ROBBER         = 11887
};

bool GOUse_go_loose_dirt_mound(Player* pPlayer, GameObject* /*pGo*/)
{
	if (pPlayer->GetQuestStatus(QUEST_LOOSE_DIRT_MOUND))
	{
		// Mercutio Filthgorger
		if (Creature* Mercutio = pPlayer->SummonCreature(NPC_MERCUTIO_FILTHGORGER, 1599.30f, -3286.00f, 92.00f, 3.00f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
			Mercutio->AI()->AttackStart(pPlayer);

		// Crypt Robbers
		if (Creature* Raider1 = pPlayer->SummonCreature(NPC_CRYPT_ROBBER, 1603.24f, -3282.58f, 91.36f, 3.00f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
			Raider1->AI()->AttackStart(pPlayer);
		if (Creature* Raider2 = pPlayer->SummonCreature(NPC_CRYPT_ROBBER, 1602.26f, -3289.04f, 92.57f, 3.00f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
			Raider2->AI()->AttackStart(pPlayer);
		if (Creature* Raider3 = pPlayer->SummonCreature(NPC_CRYPT_ROBBER, 1604.27f, -3287.05f, 92.11f, 3.00f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
			Raider3->AI()->AttackStart(pPlayer);
	}
	return false;
}

/*######
## npc_highlord_taelan_fordring
######*/

enum eHighLordTaelanFordring
{
    SPELL_CRUSADER_STRIKE         = 14518,
    SPELL_DEVOTION_AURA           = 17232,
    SPELL_HOLY_CLEAVE             = 18819,
    SPELL_HOLY_LIGHT              = 15493,
    SPELL_HOLY_STRIKE             = 17143,
    SPELL_LAY_ON_HANDS            = 17233,

    SPELL_MOUNT                   = 470,        // Taelan's horse
    SPELL_MOUNT2                  = 458,        // Tirion's horse
    SPELL_TAELAN_DEATH            = 18969,
    SPELL_TAELANS_SUFFERING       = 18810,

    QUEST_SCARLET_SUBTERFUGE      = 5862,

    NPC_SCARLET_CAVALIER          = 1836,
    NPC_GRAND_INQUISITOR_ISILLIEN = 1840,
    NPC_TIRION_FORDRING           = 1855,
    NPC_CRIMSON_ELITE             = 12128,
    
    FACTION_FRIENDLY              = 35,
    FACTION_FFP                   = 250,         // Friendly with players, but not with enviromental
    J_SUMMONED_DESPAWN_TIME       = 5*MINUTE*IN_MILLISECONDS,
    MAX_CREATURES                 = 7,           // This definition contains all summoned creatures which must be despawned at end of event

    SAY_GUARD1                    = -1000553,
    SAY_GUARD2                    = -1000554,
    SAY_GUARD3                    = -1000555,
    SAY_GUARD4                    = -1000556,
    SAY_GUARD5                    = -1000557,
    SAY_GUARD6                    = -1000558,
    SAY_GUARD7                    = -1000559,
    YELL_NOOO                     = -1000560,

    YELL_ISILLIEN                 = -1000561,
    YELL_TAELAN                   = -1000562,
    SAY_TAELAN_STRANGER           = -1000563,
    SAY_ISILLIEN1                 = -1000564,
    SAY_ISILLIEN2                 = -1000565,
    SAY_ISILLIEN3                 = -1000566,
    SAY_ISILLIEN4                 = -1000567,
    SAY_TIRION1                   = -1000568,
    SAY_ISILLIEN5                 = -1000569,
    SAY_TIRION2                   = -1000570,
    SAY_ISILLIEN6                 = -1000571,
    SAY_TIRION3                   = -1000572,
    SAY_TIRION4                   = -1000573,
    SAY_TIRION5                   = -1000574,
    SAY_TIRION6                   = -1000575,
    SAY_TIRION7                   = -1000576
};

struct MANGOS_DLL_DECL npc_highlord_taelan_fordringAI : public npc_escortAI
{
    npc_highlord_taelan_fordringAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}
    
    uint32 m_uiCrusaderStrikeTimer;
    uint32 m_uiHolyCleaveTimer;
    uint32 m_uiHolyLightTimer;
    uint32 m_uiHolyStrikeTimer;
    uint32 m_uiMountTimer;

    uint32 EventPhase;
    uint32 EventTimer;
    uint32 DeathTimer;

    bool bCanMount;
    bool bUsedLayOnHands;
    bool CanWalk;

    std::list<Creature*> lTaelansGuards;
    Creature* pTowerCreatures[MAX_CREATURES];

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiCrusaderStrikeTimer = 3000;
        m_uiHolyCleaveTimer = 3000;
        m_uiHolyLightTimer = 10000;
        m_uiHolyStrikeTimer = 5000;
        m_uiMountTimer = 0;

        EventPhase = 0;
        EventTimer = 0;
        DeathTimer = 0;

        bCanMount = false;
        bUsedLayOnHands = false;
        CanWalk = true;

        lTaelansGuards.clear();

        for(uint32 itr = 0; itr < MAX_CREATURES; ++itr)
            pTowerCreatures[itr] = 0;

        DoCastSpellIfCan(m_creature, SPELL_DEVOTION_AURA, CAST_AURA_NOT_PRESENT);

        DismountUnitIfCan();
    }
    
    void Aggro(Unit *)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            DismountUnitIfCan();
        DoCastSpellIfCan(m_creature, SPELL_DEVOTION_AURA, CAST_AURA_NOT_PRESENT);
    }

    void JustDied(Unit* /*pKiller*/)
    {
        //DoCastSpellIfCan(m_creature, SPELL_TAELAN_DEATH, CAST_TRIGGERED);
    }

	 void SummonedCreatureDespawn(Creature* pCreature)
	 {
		 // Clean up pointers in the array to avoid errors.
		 for (int i = 0; i < MAX_CREATURES; i++)
		 {
			 if (pTowerCreatures[i] == pCreature)
			 {
				 pTowerCreatures[i] = nullptr;
				 break;
			 }
		 }
	 }


    void WaypointStart(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                m_creature->setFaction(FACTION_FFP);
                break;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 19:
                bCanMount = true;
                SetRun(true);
                break;
            case 34:
                for(uint8 i = 0; i < 4; ++i)
                    if (Creature* pSummoned = m_creature->SummonCreature(NPC_CRIMSON_ELITE, 2710.37f+i, -1702.92f+i, 124.98f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME))
                    {
                        pSummoned->GetMotionMaster()->MovePoint(0, 2761.21f, -1630.92f, 127.96f);
                        CreatureCreatePos pos(pSummoned->GetMap(), 2761.21f, -1630.92f, 127.96f, 0.0f);
                        pSummoned->SetSummonPoint(pos);
                    }
                break;
            case 45:
                CanWalk = false;
                bCanMount = false;
                DismountUnitIfCan();
                m_creature->SetFacingTo(5.21f);
                EventPhase = 1;
                EventTimer = 1;
                break;
        }
    }

    void DismountUnitIfCan(Unit* pUnit = 0)
    {
        if (!pUnit)
            pUnit = m_creature;

        if (pUnit && pUnit->isAlive() && pUnit->IsMounted())
        {
            pUnit->Unmount();
            pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }
    }

    void SpellHit(Unit *, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_TAELAN_DEATH && pTowerCreatures[0])
        {
            for(uint8 itr = 0; itr != 6; ++itr)
                if (pTowerCreatures[itr] && pTowerCreatures[itr]->isAlive())
                    if (pTowerCreatures[itr]->getVictim() && pTowerCreatures[itr]->getVictim()->GetEntry() == m_creature->GetEntry())
                        pTowerCreatures[itr]->AI()->ResetToHome();

            m_creature->setFaction(FACTION_FRIENDLY);
            ResetToHome();
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_CAN_NOT_MOVE);
            m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

            pTowerCreatures[0]->SetFacingToObject(m_creature);
            CreatureCreatePos pos(pTowerCreatures[0]->GetMap(), pTowerCreatures[0]->GetPositionX(), pTowerCreatures[0]->GetPositionY(), pTowerCreatures[0]->GetPositionZ(), pTowerCreatures[0]->GetOrientation());
            pTowerCreatures[0]->SetSummonPoint(pos);
            pTowerCreatures[0]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pTowerCreatures[0]->AI()->ResetToHome();
            EventTimer = 1500;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        // Taelan's Death
        if ((((m_creature->GetHealth() - uiDamage)*100)/m_creature->GetMaxHealth()) <= 50)
	{
	    if (pTowerCreatures[0])
            {
                uiDamage = 0;
                pTowerCreatures[0]->InterruptNonMeleeSpells(false);
                pTowerCreatures[0]->CastSpell(m_creature, SPELL_TAELAN_DEATH, true);
            }
            else
                debug_log("SD0: Can't select Isillien in quest \"In Dreams!\" (phase Taelan's death)");
	} 
    }

    void DoStartEvent()
    {
        // This will start "NOOOOOOOOOOOO!" event
        EventPhase = 100;
        EventTimer = 1;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanWalk)
            npc_escortAI::UpdateAI(uiDiff);

        if (EventTimer)
        {
            if (EventTimer <= uiDiff)
            {
                switch(EventPhase)
                {
                    case 1:
                        m_creature->setFaction(FACTION_FRIENDLY);
                        if ((pTowerCreatures[0] = m_creature->SummonCreature(NPC_GRAND_INQUISITOR_ISILLIEN, 2689.30f, -1936.92f, 72.13f, 2.10f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME)))
                        {
                            pTowerCreatures[0]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            DoScriptText(YELL_ISILLIEN, pTowerCreatures[0]);
                        }
                        EventTimer = 3000;
                        break;
                    case 2:
                        DoScriptText(YELL_TAELAN, m_creature);
                        EventTimer = 4000;
                        break;
                    case 3:
                        DoScriptText(SAY_TAELAN_STRANGER, m_creature);
                        if (pTowerCreatures[0])
                        {
                            pTowerCreatures[0]->GetMotionMaster()->MovePoint(0, 2677.28f, -1918.11f, 68.15f);
                            CreatureCreatePos pos(pTowerCreatures[0]->GetMap(), 2677.28f, -1918.11f, 68.15f, 2.10f);
                            pTowerCreatures[0]->SetSummonPoint(pos);
                        }
                        if ((pTowerCreatures[1] = m_creature->SummonCreature(NPC_CRIMSON_ELITE, 2689.90f, -1937.98f, 72.13f, 2.10f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME)))
                        {
                            pTowerCreatures[1]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTowerCreatures[1]->GetMotionMaster()->MovePoint(0, 2673.22f, -1920.70f, 68.21f);
                            CreatureCreatePos pos(pTowerCreatures[1]->GetMap(), 2673.22f, -1920.70f, 68.21f, 2.10f);
                            pTowerCreatures[1]->SetSummonPoint(pos);
                        }
                        if ((pTowerCreatures[2] = m_creature->SummonCreature(NPC_CRIMSON_ELITE, 2689.90f, -1937.98f, 72.13f, 2.10f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME)))
                        {
                            pTowerCreatures[2]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTowerCreatures[2]->GetMotionMaster()->MovePoint(0, 2682.19f, -1914.96f, 68.04f);
                            CreatureCreatePos pos(pTowerCreatures[2]->GetMap(), 2682.19f, -1914.96f, 68.04f, 2.10f);
                            pTowerCreatures[2]->SetSummonPoint(pos);
                        }
                        EventTimer = 8500;
                        break;
                    case 4:
                        if (pTowerCreatures[0])
                        {
                            pTowerCreatures[0]->HandleEmote(EMOTE_ONESHOT_TALK);
                            DoScriptText(SAY_ISILLIEN1, pTowerCreatures[0]);
                        }
                        EventTimer = 35000;
                        break;
                    case 5:
                        if (pTowerCreatures[0])
                            DoScriptText(SAY_ISILLIEN2, pTowerCreatures[0]);
                        EventTimer = 4000;
                        break;
                    case 6:
                        m_creature->setFaction(FACTION_FFP);
                        if (pTowerCreatures[0])
                        {
                            pTowerCreatures[0]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTowerCreatures[0]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);

                            if (pTowerCreatures[0]->AI())
                                pTowerCreatures[0]->AI()->AttackStart(m_creature);

                            AttackStart(pTowerCreatures[0]);
                        }
                        for(uint8 itr = 1; itr < 3; ++itr)
                            if (pTowerCreatures[itr])
                            {
                                if (pTowerCreatures[itr]->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                                    pTowerCreatures[itr]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                if (!pTowerCreatures[itr]->getVictim() && pTowerCreatures[itr]->AI())
                                    pTowerCreatures[itr]->AI()->AttackStart(m_creature);
                            }
                        for(uint8 itr = 3; itr < 6; ++itr)
                            if ((pTowerCreatures[itr] = m_creature->SummonCreature(NPC_CRIMSON_ELITE, 2699.69f + rand()%3, -1862.04f + rand()%3, 67.51f, 4.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME)))
                            {
                                if (pTowerCreatures[itr]->AI())
                                    pTowerCreatures[itr]->AI()->AttackStart(m_creature);

                                switch(itr)
                                {
                                    case 3:
                                    {
                                        CreatureCreatePos pos(pTowerCreatures[itr]->GetMap(), 2669.17f, -1895.37f, 66.99f, 4.26f);
                                        pTowerCreatures[itr]->SetSummonPoint(pos);
                                        break;
                                    }
                                    case 4:
                                    {
                                        CreatureCreatePos pos(pTowerCreatures[itr]->GetMap(), 2659.75f, -1901.16f, 68.04f, 6.23f);
                                        pTowerCreatures[itr]->SetSummonPoint(pos);
                                        break;
                                    }
                                    case 5:
                                    {
                                        CreatureCreatePos pos(pTowerCreatures[itr]->GetMap(), 2676.84f, -1903.70f, 66.02f, 3.36f);
                                        pTowerCreatures[itr]->SetSummonPoint(pos);
                                        break;
                                    }
                                }
                            }
                        EventTimer = 0;
                        break;
                    case 7:
                        if (pTowerCreatures[0])
                            DoScriptText(SAY_ISILLIEN3, pTowerCreatures[0]);
                        EventTimer = 2500;
                        break;
                    case 8:
                        if (pTowerCreatures[0])
                            DoScriptText(SAY_ISILLIEN4, pTowerCreatures[0]);
                        EventTimer = 8000;
                        break;
                    case 9:
                        if (pTowerCreatures[0] && GetPlayerForEscort())
                        {
                            pTowerCreatures[0]->SetTargetGuid(GetPlayerForEscort()->GetObjectGuid());
                            pTowerCreatures[0]->HandleEmote(EMOTE_ONESHOT_POINT);
                        }
                        EventTimer = 4000;
                        break;
                    case 10:
                        // Isillien start attacking escorter player
                        if (pTowerCreatures[0] && pTowerCreatures[0]->isAlive() && GetPlayerForEscort() && GetPlayerForEscort()->isAlive())
                        {
                            CreatureCreatePos pos(pTowerCreatures[0]->GetMap(), 2677.28f, -1918.11f, 68.15f, 2.10f);
                            pTowerCreatures[0]->SetSummonPoint(pos);
                            pTowerCreatures[0]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_ATTACKABLE_1);
                            //pTowerCreatures[0]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);

                            if (pTowerCreatures[0]->AI())
                                pTowerCreatures[0]->AI()->AttackStart(GetPlayerForEscort());
                        }
                        EventTimer = 11000;
                        break;
                    case 11:
                        if ((pTowerCreatures[6] = m_creature->SummonCreature(NPC_TIRION_FORDRING, 2634.07f, -1914.61f, 72.23f, 0.35f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, J_SUMMONED_DESPAWN_TIME)))
                        {
                            pTowerCreatures[6]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTowerCreatures[6]->RemoveSplineFlag(SPLINEFLAG_WALKMODE);      // Tirion run
                            pTowerCreatures[6]->InterruptNonMeleeSpells(false);
                            pTowerCreatures[6]->CastSpell(pTowerCreatures[6], SPELL_MOUNT2, true);
                        }
                        EventTimer = 2000;
                        break;
                    case 12:
                        if (pTowerCreatures[6])
                        {
                            pTowerCreatures[6]->StopMoving();
                            pTowerCreatures[6]->GetMotionMaster()->Clear();
                            pTowerCreatures[6]->GetMotionMaster()->MovePoint(0, 2652.45f, -1908.32f, 69.64f);
                        }
                        EventTimer = 2000;
                        break;
                    case 13:
                        if (pTowerCreatures[6])
                        {
                            pTowerCreatures[6]->StopMoving();
                            //DismountUnitIfCan(pTowerCreatures[6]);
                            if (pTowerCreatures[6]->IsMounted())
                            {
                                pTowerCreatures[6]->Unmount();
                                pTowerCreatures[6]->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
                            }
                            pTowerCreatures[6]->SetFacingToObject(m_creature);
                            pTowerCreatures[6]->GetMotionMaster()->Clear();
                            pTowerCreatures[6]->GetMotionMaster()->MoveChase(m_creature, 3.0f, pTowerCreatures[6]->GetOrientation());
                            CreatureCreatePos pos(pTowerCreatures[6]->GetMap(), m_creature->GetPositionX()-2, m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);
                            pTowerCreatures[6]->SetSummonPoint(pos);
                            EventTimer = 2500;
                        }
                        break;
                    case 14:
                        if (pTowerCreatures[6])
                        {
                            if (pTowerCreatures[6]->IsWithinDist(m_creature, 4.0f))
                            {
                                CreatureCreatePos pos(pTowerCreatures[6]->GetMap(), pTowerCreatures[6]->GetPositionX(), pTowerCreatures[6]->GetPositionY(), pTowerCreatures[6]->GetPositionZ(), pTowerCreatures[6]->GetOrientation());
                                pTowerCreatures[6]->SetSummonPoint(pos);
                            }
                            else
                            {
                                pTowerCreatures[6]->Relocate(m_creature->GetPositionX()-2, m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);
                                --EventPhase;
                                EventTimer = 1;
                                break;
                            }
                            pTowerCreatures[6]->SetFacingToObject(m_creature);
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        EventTimer = 4000;
                        break;
                    case 15:
                        if (pTowerCreatures[6])
                        {
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_STAND);
                            DoScriptText(SAY_TIRION1, pTowerCreatures[6]);
                        }
                        EventTimer = 10000;
                        break;
                    case 16:
                        if (pTowerCreatures[0] && pTowerCreatures[0]->isAlive())
                            DoScriptText(SAY_ISILLIEN5, pTowerCreatures[0]);
                        else if (pTowerCreatures[0] && pTowerCreatures[0]->isDead())
                        {
                            EventPhase = 20;
                            EventTimer = 1;
                            break;
                        }
                        EventTimer = 14000;
                        break;
                    case 17:
                        if (pTowerCreatures[0] && pTowerCreatures[0]->isAlive() && pTowerCreatures[6] && pTowerCreatures[6]->isAlive())
                        {
                            DoScriptText(SAY_TIRION2, pTowerCreatures[6]);
                            pTowerCreatures[6]->SetFacingToObject(pTowerCreatures[0]);
                            pTowerCreatures[6]->HandleEmote(EMOTE_ONESHOT_POINT);
                        }
                        EventTimer = 10000;
                        break;
                    case 18:
                        if (pTowerCreatures[0] && pTowerCreatures[0]->isAlive())
                            DoScriptText(SAY_ISILLIEN6, pTowerCreatures[0]);
                        EventTimer = 3500;
                        break;
                    case 19:
                        for(uint8 itr = 0; itr < MAX_CREATURES; ++itr)
                            if (pTowerCreatures[itr])
                                pTowerCreatures[itr]->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_ATTACKABLE_1);
                        if (pTowerCreatures[0] && pTowerCreatures[6])
                        {
                            pTowerCreatures[0]->DeleteThreatList();

                            if (pTowerCreatures[6]->AI())
                                pTowerCreatures[6]->AI()->AttackStart(pTowerCreatures[0]);

                            if (pTowerCreatures[0]->AI())
                                pTowerCreatures[0]->AI()->AttackStart(pTowerCreatures[6]);
                        }
                        EventTimer = 0;
                        DeathTimer = 2500;
                        break;
                    case 20:
                        // Crimson Elites must be defeated
                        for(uint8 itr = 1; itr != 6; ++itr)
                            if (pTowerCreatures[itr] && pTowerCreatures[itr]->isAlive())
                                if (!pTowerCreatures[itr]->getVictim())
                                    if (pTowerCreatures[6] && pTowerCreatures[6]->isAlive() && pTowerCreatures[itr]->AI())
                                        pTowerCreatures[itr]->AI()->AttackStart(pTowerCreatures[6]);

                        if (pTowerCreatures[6])
                        {
                            if (!pTowerCreatures[6]->getVictim())
                            {
                                DoScriptText(SAY_TIRION3, pTowerCreatures[6]);
                                pTowerCreatures[6]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                                if (pTowerCreatures[6]->AI())
                                    pTowerCreatures[6]->AI()->ResetToHome();
                            }
                            else
                            {
                                --EventPhase;
                                EventTimer = 2500;
                            }
                        }
                        EventTimer = 8000;
                        break;
                    case 21:
                        if (pTowerCreatures[6])
                        {
                            pTowerCreatures[6]->SetFacingToObject(m_creature);
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        EventTimer = 2000;
                        break;
                    case 22:
                        if (pTowerCreatures[6])
                        {
                            DoScriptText(SAY_TIRION4, pTowerCreatures[6]);
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_STAND);
                            pTowerCreatures[6]->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);      // TODO: When core support Excited emote, use it!
                        }
                        EventTimer = 2000;
                        break;
                    case 23:
                        if (pTowerCreatures[6])
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        EventTimer = 4000;
                        break;
                    case 24:
                        if (pTowerCreatures[6])
                            DoScriptText(SAY_TIRION5, pTowerCreatures[6]);
                        EventTimer = 5000;
                        break;
                    case 25:
                        if (pTowerCreatures[6])
                            DoScriptText(SAY_TIRION6, pTowerCreatures[6]);
                        EventTimer = 5000;
                        break;
                    case 26:
                        if (GetPlayerForEscort())
                            GetPlayerForEscort()->GroupEventHappens(QUEST_IN_DREAMS, m_creature);
                        if (pTowerCreatures[6])
                        {
                            pTowerCreatures[6]->SetStandState(UNIT_STAND_STATE_STAND);
                            DoScriptText(SAY_TIRION7, pTowerCreatures[6]);
                            pTowerCreatures[6]->HandleEmote(EMOTE_ONESHOT_ROAR);
                        }
                        EventTimer = 45000;
                        break;
                    case 27:
                        // event done
                        /*for(uint8 itr = 0; itr < MAX_CREATURES-1; ++itr)
                            if (pTowerCreatures[itr])
                            {
                                pTowerCreatures[itr]->RemoveFromWorld();
                                pTowerCreatures[itr] = 0;
                            }*/
                        EventTimer = 0;
                        EventPhase = 0;
                        m_creature->ForcedDespawn();
                        break;

                    // NOOOOOOOOOOOO! event
                    case 100:
                        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                        GetCreatureListWithEntryInGrid(lTaelansGuards, m_creature, NPC_SCARLET_CAVALIER, 15.f);
                        if (!lTaelansGuards.empty())
                        {
                            uint8 pGuardNo = 0;
                            for(std::list<Creature*>::iterator itr = lTaelansGuards.begin(); itr != lTaelansGuards.end(); ++itr)
                                if ((*itr))
                                {
                                    if ((*itr)->isAlive())
                                    {
                                        ++pGuardNo;
                                        (*itr)->SetFacingToObject(m_creature);
                                        switch(pGuardNo)
                                        {
                                            case 1: DoScriptText(SAY_GUARD1, (*itr));
                                            case 2: DoScriptText(SAY_GUARD2, (*itr));
                                            case 3: DoScriptText(SAY_GUARD3, (*itr));
                                            case 4: DoScriptText(SAY_GUARD4, (*itr));
                                        }
                                    }
                                }
                        }
                        else
                            debug_log("SD0: Taelan can't find his guards around him.");

                        EventTimer = 3000;
                        break;
                    case 101:
                        if (!lTaelansGuards.empty())
                            for(std::list<Creature*>::iterator itr = lTaelansGuards.begin(); itr != lTaelansGuards.end(); ++itr)
                                if ((*itr))
                                    (*itr)->GetMotionMaster()->MoveFollow(m_creature, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                        EventTimer = 7000;
                        break;
                    case 102:
                        DoScriptText(YELL_NOOO, m_creature);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->HandleEmote(EMOTE_ONESHOT_ROAR);
                        if (!lTaelansGuards.empty())
                            for(std::list<Creature*>::iterator itr = lTaelansGuards.begin(); itr != lTaelansGuards.end(); ++itr)
                                if ((*itr))
                                    (*itr)->HandleEmote(EMOTE_ONESHOT_KICK);
                        EventTimer = 2000;
                        break;
                    case 103:
                        if (!lTaelansGuards.empty())
                            for(std::list<Creature*>::iterator itr = lTaelansGuards.begin(); itr != lTaelansGuards.end(); ++itr)
                                if ((*itr))
                                {
                                    DoScriptText(urand(0,1) == 1 ? SAY_GUARD5 : SAY_GUARD6, (*itr));
                                    
                                    if ((*itr)->AI())
                                        (*itr)->AI()->AttackStart(m_creature);

                                    if (!m_creature->getVictim() && itr == lTaelansGuards.begin())
                                        AttackStart((*itr));
                                }
                        EventPhase = 0;
                        EventTimer = 0;
                        break;
                }
                ++EventPhase;
            }
            else
                EventTimer -= uiDiff;
        }

        if (DeathTimer)
        {
            if (DeathTimer <= uiDiff)
            {
                DeathTimer = 2500;
                if (pTowerCreatures[0])
                {
                    if (pTowerCreatures[0]->isDead())
                    {
                        EventTimer = 1;
                        DeathTimer = 0;
                    }
                }
                else
                    debug_log("SD0: Creature: Grand Inquisiton Isillien can't be selected for quest \"In Dreams!\"");
            }
            else
                DeathTimer -= uiDiff;
        }

        // Mount
        if (HasEscortState(STATE_ESCORT_ESCORTING))
	{
            if (m_uiMountTimer <= uiDiff)
            {
                if (bCanMount && !m_creature->IsMounted() && !m_creature->getVictim())
                    DoCastSpellIfCan(m_creature, SPELL_MOUNT, CAST_TRIGGERED + CAST_FORCE_CAST + CAST_AURA_NOT_PRESENT);
                else if ((!bCanMount && m_creature->IsMounted()) || (m_creature->isInCombat()))
                    DismountUnitIfCan();
                m_uiMountTimer = 1000;
            }
            else
                m_uiMountTimer -= uiDiff;
	}

        // Return if we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Lay on Hands
        if (!bUsedLayOnHands && m_creature->GetHealthPercent() < 10.0f)
        {
            bUsedLayOnHands = true;
            m_creature->InterruptNonMeleeSpells(false);
            DoCastSpellIfCan(m_creature, SPELL_LAY_ON_HANDS, CAST_FORCE_CAST);
        }

        // m_uiHolyCleaveTimer
        if (m_uiHolyCleaveTimer <= uiDiff)
        {
            m_uiHolyCleaveTimer = 0;
            if (m_creature->getAttackers().size() > 2 && DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_CLEAVE) == CAST_OK)
                m_uiHolyCleaveTimer = 3000;
        }
        else
            m_uiHolyCleaveTimer -= uiDiff;

         // m_uiHolyStrikeTimer
        if (m_uiHolyStrikeTimer <= uiDiff)
        {
            m_uiHolyStrikeTimer = 0;
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_STRIKE) == CAST_OK)
                m_uiHolyStrikeTimer = 7500;
        }
        else
            m_uiHolyStrikeTimer -= uiDiff;
        
        // m_uiCrusaderStrikeTimer
        if (m_uiCrusaderStrikeTimer <= uiDiff)
        {
            m_uiCrusaderStrikeTimer = 0;
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADER_STRIKE) == CAST_OK)
                m_uiCrusaderStrikeTimer = 5000;
        }
        else
            m_uiCrusaderStrikeTimer -= uiDiff;

        // m_uiHolyLightTimer
        if (m_uiHolyLightTimer <= uiDiff)
        {
            m_uiHolyLightTimer = 0;

            if (m_creature->GetHealthPercent() < 50.0f && m_creature->getAttackers().size() < 3)
            {
                Unit* pFriend = DoSelectLowestHpFriendly(15.f, 1500);
                if (pFriend)
                {
                    if (DoCastSpellIfCan(pFriend, SPELL_HOLY_LIGHT, CAST_FORCE_CAST) == CAST_OK)
                        m_uiHolyLightTimer = urand(5000,15000);
                }
            }
        }
        else
            m_uiHolyLightTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_highlord_taelan_fordring(Creature* pCreature)
{
    return new npc_highlord_taelan_fordringAI(pCreature);
}

bool QuestAccept_npc_highlord_taelan_fordring(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_IN_DREAMS)
        if (npc_highlord_taelan_fordringAI* pEscortAI = dynamic_cast<npc_highlord_taelan_fordringAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    return true;
}

bool QuestRewarded_npc_highlord_taelan_fordring(Player* /*pPlayer*/, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SCARLET_SUBTERFUGE)
        if (npc_highlord_taelan_fordringAI* pTaelanAI = dynamic_cast<npc_highlord_taelan_fordringAI*>(pCreature->AI()))
            pTaelanAI->DoStartEvent();
    return true;
}

/*######
## mob_grand_inquisitor_isillien
######*/

enum eIsillien
{
    //SPELL_DOMINATE_MIND     = 14515,
    //SPELL_FLASH_HEAL        = 10917,
    //SPELL_GREATER_HEAL      = 10965,
    SPELL_MANA_BURN         = 15800,
    SPELL_MIND_BLAST        = 17194,
    SPELL_MIND_FLAY         = 17165,
    //SPELL_POWER_WORD_SHIELD = 10901
};

struct MANGOS_DLL_DECL mob_grand_inquisitor_isillienAI : public ScriptedAI
{
    mob_grand_inquisitor_isillienAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    /*uint32 m_uiDominateMindTimer;
    uint32 m_uiFlashHealTimer;
    uint32 m_uiGreaterHealTimer;*/
    uint32 m_uiManaBurnTimer;
    uint32 m_uiMindBlastTimer;
    uint32 m_uiMindFlayTimer;
    //uint32 m_uiPowerWordShieldTimer;
    bool bTaelanDeathCasted;

    void Reset()
    {
        /*m_uiDominateMindTimer = 12000;
        m_uiFlashHealTimer = 7000;
        m_uiGreaterHealTimer = 10000;*/
        m_uiManaBurnTimer = 15000;
        m_uiMindBlastTimer = 3000;
        m_uiMindFlayTimer = 18000;
        //m_uiPowerWordShieldTimer = 60000;
        bTaelanDeathCasted = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Dominate Mind
        /*if (m_uiDominateMindTimer <= uiDiff)
        {
            if (m_creature->getVictim()->GetEntry() != NPC_TIRION_FORDRING)
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_DOMINATE_MIND);
            m_uiDominateMindTimer = 45000;
        }
        else
            m_uiDominateMindTimer -= uiDiff;

        // Flash Heal
        if (m_uiFlashHealTimer <= uiDiff)
        {
            Unit* pFriend = DoSelectLowestHpFriendly(40.0f, 2000);
            if (pFriend || HealthBelowPct(75))
                DoCastSpellIfCan((pFriend ? pFriend : m_creature), SPELL_FLASH_HEAL);
            m_uiFlashHealTimer = 13000;
        }
        else
            m_uiFlashHealTimer -= uiDiff;

        // Greater Heal
        if (m_uiGreaterHealTimer <= uiDiff)
        {
            Unit* pFriend = DoSelectLowestHpFriendly(40.0f, 4000);
            if (pFriend || HealthBelowPct(35))
                DoCastSpellIfCan((pFriend ? pFriend : m_creature), SPELL_GREATER_HEAL);
            m_uiGreaterHealTimer = 15000;
        }
        else
            m_uiGreaterHealTimer -= uiDiff;*/

        // Mana Burn
        if (m_uiManaBurnTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANA_BURN);
            m_uiManaBurnTimer = 10000;
        }
        else
            m_uiManaBurnTimer -= uiDiff;

        // Mind Blast
        if (m_uiMindBlastTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_BLAST);
            m_uiMindBlastTimer = 10000;
        }
        else
            m_uiMindBlastTimer -= uiDiff;

        // Mind Flay
        if (m_uiMindFlayTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIND_FLAY);
            m_uiMindFlayTimer = 20000;
        }
        else
            m_uiMindFlayTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_grand_inquisitor_isillien(Creature* pCreature)
{
    return new mob_grand_inquisitor_isillienAI(pCreature);
}

/*######
## go_lumber_mill_crate
######*/

bool GossipHello_go_lumber_mill_crate(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->IsCurrentQuest(5902) || pPlayer->IsCurrentQuest(5904))
	{
		GameObject* Barrel = GetClosestGameObjectWithEntry(pGo, 177491, 15.0f);
		if (Barrel && !Barrel->isSpawned())
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT,"Put down the Barrel of Termites here",GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+1);
	}

	pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pGo), pGo->GetObjectGuid());
    return true;
}

bool GossipSelect_go_lumber_mill_crate(Player* pPlayer, GameObject* pGo, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
		GameObject* Barrel = GetClosestGameObjectWithEntry(pGo, 177491, 15.0f);
		if (Barrel && !Barrel->isSpawned())
		{
			Barrel->SetRespawnTime(30);
			Barrel->Refresh();
		}	
		pPlayer->CLOSE_GOSSIP_MENU();
    }
    return true;
}

/*######
## mob_temporal_parasite
######*/

enum TemporalParasite
{
    TEMPORAL_PARASITE = 10717, 
	TEMPORAL_PARASITE_SPELL_SLOW = 11436,
};

struct MANGOS_DLL_DECL mob_temporal_parasite : public ScriptedAI
{
    mob_temporal_parasite(Creature* pCreature) : ScriptedAI(pCreature) { Reset();}

	uint32 m_uiSlowTimer;

	void Reset() 
	{
		m_uiSlowTimer = urand(5000,15000);
	}

	/* The temporal parasites have a chance of spawning a new parasite at death. */

	void JustDied(Unit* /*Killer*/)
	{
		int newParasite = urand(1, 100);

		if(newParasite > 70)
		{
			m_creature->SummonCreature(TEMPORAL_PARASITE, m_creature->GetPositionX(), m_creature->GetPositionY(), 
				m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if(m_uiSlowTimer <= uiDiff)
		{
			if(m_creature->getVictim())
				DoCastSpellIfCan(m_creature->getVictim(), TEMPORAL_PARASITE_SPELL_SLOW);
			m_uiSlowTimer = urand(5000,15000);
		}
		else
		{
			m_uiSlowTimer -= uiDiff;
		}

		DoMeleeAttackIfReady();
	}


};

CreatureAI* GetAI_mob_temporal_parasite(Creature *pCreature)
{
    return new mob_temporal_parasite (pCreature);
}

/*######
##
######*/

void AddSC_western_plaguelands()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_andorhal_tower";
    pNewScript->GetAI = &GetAI_npc_andorhal_tower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npcs_dithers_and_arbington";
    pNewScript->pGossipHello = &GossipHello_npcs_dithers_and_arbington;
    pNewScript->pGossipSelect = &GossipSelect_npcs_dithers_and_arbington;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eva_sarkhoff";
    pNewScript->pGossipHello = &GossipHello_npc_eva_sarkhoff;
    pNewScript->pGossipSelect = &GossipSelect_npc_eva_sarkhoff;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_myranda_the_hag";
    pNewScript->pGossipHello = &GossipHello_npc_myranda_the_hag;
    pNewScript->pGossipSelect = &GossipSelect_npc_myranda_the_hag;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_the_scourge_cauldron";
    pNewScript->GetAI = &GetAI_npc_the_scourge_cauldron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_loose_dirt_mound";
    pNewScript->pGOUse = &GOUse_go_loose_dirt_mound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_highlord_taelan_fordring";
    pNewScript->GetAI = &GetAI_npc_highlord_taelan_fordring;
    pNewScript->pQuestAcceptNPC =   &QuestAccept_npc_highlord_taelan_fordring;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_highlord_taelan_fordring;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_grand_inquisitor_isillien";
    pNewScript->GetAI = &GetAI_mob_grand_inquisitor_isillien;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_lumber_mill_crate";
    pNewScript->pGossipHelloGO = &GossipHello_go_lumber_mill_crate;
    pNewScript->pGossipSelectGO = &GossipSelect_go_lumber_mill_crate;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
    pNewScript->Name = "mob_temporal_parasite";
    pNewScript->GetAI = &GetAI_mob_temporal_parasite;
    pNewScript->RegisterSelf();
}
