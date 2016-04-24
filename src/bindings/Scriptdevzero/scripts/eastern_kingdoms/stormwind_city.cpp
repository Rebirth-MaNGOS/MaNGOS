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
SDName: Stormwind_City
SD%Complete: 100
SDComment: Quest support: 1640, 1447, 4185, 11223 (DB support required for spell 42711)
SDCategory: Stormwind City
EndScriptData */

/* ContentData
npc_archmage_malin
npc_bartleby
npc_dashel_stonefist
npc_lady_katrana_prestor
npc_lord_gregor_lescovar
npc_tyrion
npc_reginald_windsor
npc_squire_rowe
mob_rift_spawn
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_archmage_malin
######*/

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

bool GossipHello_npc_archmage_malin(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(11223) == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(11223))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_archmage_malin(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 42711, true);
    }

    return true;
}

/*######
## npc_bartleby
######*/

enum
{
    FACTION_ENEMY       = 168,
    QUEST_BEAT          = 1640
};

struct MANGOS_DLL_DECL npc_bartlebyAI : public ScriptedAI
{
    npc_bartlebyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage)*100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->GroupEventHappens(QUEST_BEAT, ((Player*)pDoneBy));

            ResetToHome();
        }
    }
};

bool QuestAccept_npc_bartleby(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BEAT)
    {
        pCreature->setFaction(FACTION_ENEMY);
        pCreature->AI()->AttackStart(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_bartleby(Creature* pCreature)
{
    return new npc_bartlebyAI(pCreature);
}

/*######
## npc_dashel_stonefist
######*/

enum
{
	SAY_DASHEL_END				= -1720016,
    QUEST_MISSING_DIPLO_PT8     = 1447,
    FACTION_HOSTILE             = 168,
	NPC_DASHELS_FRIEND			= 4969,
};

struct Loc
{
    float x, y, z, o;
};

static Loc aFriendSpawnLoc[]= 
{
	{-8672.33, 442.88, 99.98,3.5},
	{-8691.59, 441.66, 99.41,6.1},
};

struct MANGOS_DLL_DECL npc_dashel_stonefistAI : public ScriptedAI
{
    npc_dashel_stonefistAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNormalFaction = pCreature->getFaction();
        Reset();
    }

    uint32 m_uiNormalFaction;
	GUIDList lDashelFriends;
	bool m_bFriendSummoned;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormalFaction)
            m_creature->setFaction(m_uiNormalFaction);
		m_bFriendSummoned = false;

        lDashelFriends.clear();
    }

	void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_DASHELS_FRIEND)
        {
			lDashelFriends.push_back(pSummoned->GetObjectGuid());
			pSummoned->SetRespawnDelay(-10);			// make sure they won't respawn randomly
        }
    }

	void DespawnDashelFriends()
    {
        if (!lDashelFriends.empty())
            for(GUIDList::iterator itr = lDashelFriends.begin(); itr != lDashelFriends.end(); ++itr)
                if (Creature* pDashelFriend = m_creature->GetMap()->GetCreature(*itr))
                {
                    if (pDashelFriend->isInCombat())
                        pDashelFriend->AI()->ResetToHome();
                    pDashelFriend->setFaction(35);
					pDashelFriend->ForcedDespawn(5000);
                }
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || ((m_creature->GetHealth() - uiDamage)*100 / m_creature->GetMaxHealth() < 15))
        {
            uiDamage = 0;

            if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
                ((Player*)pDoneBy)->AreaExploredOrEventHappens(QUEST_MISSING_DIPLO_PT8);

			DoScriptText(SAY_DASHEL_END, m_creature);
			DespawnDashelFriends();
            ResetToHome();
        }
    }
};

bool QuestAccept_npc_dashel_stonefist(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_DIPLO_PT8)
    {
        pCreature->setFaction(FACTION_HOSTILE);
        pCreature->AI()->AttackStart(pPlayer);
		for(uint8 i = 0; i < 2; ++i)
		{
			pCreature->SummonCreature(NPC_DASHELS_FRIEND, aFriendSpawnLoc[i].x, aFriendSpawnLoc[i].y, aFriendSpawnLoc[i].z, aFriendSpawnLoc[i].o, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
		}
    }
    return true;
}

CreatureAI* GetAI_npc_dashel_stonefist(Creature* pCreature)
{
    return new npc_dashel_stonefistAI(pCreature);
}

/*######
## npc_lady_katrana_prestor
######*/

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

bool GossipHello_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(4185) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(2693, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_lady_katrana_prestor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2694, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(2695, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(2696, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(4185);
            break;
    }
    return true;
}

/*######
## npc_marzon_the_silent_pladeAI
######*/

struct MANGOS_DLL_DECL npc_marzon_the_silent_pladeAI : public ScriptedAI
{
	npc_marzon_the_silent_pladeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

	void Reset() {}

	void ResetToHome()
	{
		m_creature->ForcedDespawn();
	}

	void JustRespawned()
	{
		m_creature->ForcedDespawn();
	}

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_creature->SelectHostileTarget() || m_creature->getVictim())
			DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_npc_marzon_the_silent_pladeAI(Creature* pcreature)
{
	return new npc_marzon_the_silent_pladeAI(pcreature);
}


/*######
## npc_lord_gregor_lescovar
######*/

enum eLordGregorLescovar
{
    QUEST_THE_ATTACK            = 434,
    FACTION_ENEMYY              = 14,
    FACTION_FRIENDLYY           = 35, // not used
    FACTION_NORMAL_SW_R_GUARD   = 11,
    FACTION_NORMAL_LESCOVAR     = 12,
    FACTION_NORMAL_MARZON       = 84,

    NPC_LORD_GREGOR_LESCOVAR    = 1754,
    NPC_MARZON_THE_SILENT_BLADE = 1755,
    NPC_STORMWIND_ROYAL_GUARD   = 1756,
    NPC_TYRIONA                 = 7779,
    NPC_TYRION                  = 7766,
    NPC_TYRIONS_SPYBOT          = 8856,

    SAY_GREGOR_1                = -1000594,
    SAY_GREGOR_2                = -1000595,
    SAY_GREGOR_3                = -1000596,
    SAY_GREGOR_4                = -1000597,
    SAY_GREGOR_5                = -1000598,
    SAY_MARZON_1                = -1000599,
    SAY_MARZON_2                = -1000600,
    SAY_MARZON_3                = -1000601,
    SAY_TYRIONA_1               = -1000602,
    SAY_TYRIONA_2               = -1000603,
    SAY_TYRIONA_3               = -1000604,
    SAY_TYRIONA_4               = -1000605,
    SAY_ROYAL_GUARD_1           = -1000606,
    SAY_ROYAL_GUARD_2           = -1000607,
    SAY_SPYBOT                  = -1000608,
    SAY_TYRION_1                = -1000609,
    SAY_TYRION_2                = -1000610,
};

struct MANGOS_DLL_DECL npc_lord_gregor_lescovarAI : public npc_escortAI
{
	npc_lord_gregor_lescovarAI(Creature* pCreature) : npc_escortAI(pCreature)
	{
		Reset();
	}

	uint32 m_uiGuardReturn_Timer;
	uint32 EventTimer;
	uint8 EventPhase;
	uint8 GardenGuardsCounter;
	ObjectGuid GardenRoyalGuard[3];
	ObjectGuid Marzon;
	ObjectGuid Tyrion;

	bool StartGuardsReturnTimer;
	bool CanWalk;

	void Reset()
	{
		if (m_creature->getFaction() == FACTION_ENEMYY)
		{
			m_creature->setFaction(FACTION_NORMAL_LESCOVAR);
		}

		CanWalk = true;
		if (HasEscortState(STATE_ESCORT_ESCORTING))
			return;


		m_uiGuardReturn_Timer = 5000;
		EventTimer = 0;
		EventPhase = 0;

		GardenRoyalGuard[0] = ObjectGuid();
		GardenRoyalGuard[1] = ObjectGuid();
		GardenRoyalGuard[2] = ObjectGuid();
		GardenGuardsCounter = 0;
		Marzon = ObjectGuid();
		Tyrion = ObjectGuid();

		StartGuardsReturnTimer = false;
	}

	

	void ResetToHome()
	{
        Creature* pMarzon = m_creature->GetMap()->GetCreature(Marzon);
		if(pMarzon)
			pMarzon->ForcedDespawn();

		ReturnStormwindGuards();
		npc_escortAI::ResetToHome();
	}

	bool GetCreaturesForQuest(float fMaxSearchRange = 10.0f)
	{
		// Tyrion
		Creature* pTyrion = GetClosestCreatureWithEntry(m_creature, NPC_TYRION, VISIBLE_RANGE);
		if (!pTyrion)
		{
			debug_log("SD0: Creature 'Tyrion' (entry %u) has been not found for quest 'The Attack!'", NPC_TYRION);
			return false;
		}
		else
            Tyrion = pTyrion->GetObjectGuid();

		// Marzon The Silent Blade

		if (Creature* pMarzon = m_creature->SummonCreature(NPC_MARZON_THE_SILENT_BLADE, -8400.95f, 476.11f, 123.76f, 3.95f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
        {
            Marzon = pMarzon->GetObjectGuid();

			pMarzon->SetVisibility(VISIBILITY_OFF);
			pMarzon->UpdateVisibilityAndView();
			pMarzon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
			pMarzon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
			pMarzon->AI()->DoCastSpellIfCan(pMarzon, 1784);
		}

		// Stormwind Royal Guards (Garden)
		std::list<Creature*> lGardenStormwindRoyalGuards;
		GetCreatureListWithEntryInGrid(lGardenStormwindRoyalGuards, m_creature, NPC_STORMWIND_ROYAL_GUARD, fMaxSearchRange);

		if (!lGardenStormwindRoyalGuards.empty())
		{
			for (std::list<Creature*>::iterator iter = lGardenStormwindRoyalGuards.begin(); iter != lGardenStormwindRoyalGuards.end(); ++iter)
			{
				if (*iter)
				{
					Creature* GardenStormwindRoyalGuard = (*iter);
					if (GardenStormwindRoyalGuard->isDead() || !GardenStormwindRoyalGuard->isAlive())
						GardenStormwindRoyalGuard->Respawn();

					GardenRoyalGuard[++GardenGuardsCounter] = GardenStormwindRoyalGuard->GetObjectGuid();
				}
				debug_log("SD0: Quest: The Attack! Filling 'LibraryRoyalGuard' variable now!");
			}

            
            Creature* pGuard1 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[1]);
            Creature* pGuard2 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[2]);
            if(pGuard1 && pGuard2)
            {
                if(pGuard2->GetDistanceSqr(-8394.4f, 450.39f, 123.76f) > pGuard1->GetDistanceSqr(-8394.4f, 450.39f, 123.76f))
                {
                    ObjectGuid temp = GardenRoyalGuard[1];
                    GardenRoyalGuard[1] = GardenRoyalGuard[2];
                    GardenRoyalGuard[2] = temp;
                }
            }
		}
		else
		{
			debug_log("SD0: Creatures 'Stormwind Royal Guard' (entry %u) which is located in Garden of the Stormwind Keep has been not found for quest 'The Attack!'", NPC_STORMWIND_ROYAL_GUARD);
			return false;
		}

		return true;
	}

	void SummonedCreatureJustDied(Creature* pSummoned)
	{
		if(m_creature->isDead())
			ReturnStormwindGuards();

        ForceQuestCredit(pSummoned);
	}

	void SummonedCreatureDespawn(Creature* /*pSummoned*/)
	{
		if(m_creature->isDead())
			ReturnStormwindGuards();
	}

    void ForceQuestCredit(Creature *pMob)
    {
        Player *pPlayer = GetPlayerForEscort();

        if(pPlayer)
        {
            if( Group *pGroup = pPlayer->GetGroup() )
            {
                for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player *pGroupGuy = itr->getSource();

                    // for any leave or dead (with not released body) group member at appropriate distance
                    if( pGroupGuy && pGroupGuy->IsAtGroupRewardDistance(m_creature) && !pGroupGuy->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST) )
                        pGroupGuy->KilledMonsterCredit(pMob->GetEntry());
                }
            }
            else
                pPlayer->KilledMonsterCredit(pMob->GetEntry());
        }
    }

	
	void JustDied(Unit* /*pKiller*/)
	{
		ForceQuestCredit(m_creature);

        Creature* pMarzon = m_creature->GetMap()->GetCreature(Marzon);

		if(pMarzon)
		{
			if(pMarzon->isDead())
			{
				ReturnStormwindGuards();
			}
		}
		else
		{
			ReturnStormwindGuards();
		}

	}

	void Aggro(Unit* /*pAttacker*/) {}

	void WaypointReached(uint32 uiPoint)
	{
		Player* pPlayer = GetPlayerForEscort();
		if (!pPlayer)
			return;

		switch (uiPoint)
		{
			case 6:
				CanWalk = true;
				if(GetCreaturesForQuest())
				{
                    Creature* pGuard1 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[1]);
                    Creature* pGuard2 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[2]);

					if (pGuard1 && pGuard1->isAlive())
						pGuard1->SetFacingToObject(m_creature);

					if (pGuard2 && pGuard2->isAlive())
						pGuard2->SetFacingToObject(m_creature);

                    DoScriptText(SAY_GREGOR_2, m_creature);
					EventPhase = 1;
					EventTimer = 3500;
					CanWalk = false;
				}
				break;
			case 7:
				EventPhase = 4;
				EventTimer = 1500;
				break;
		}
	}

	/**
	* Returnss the Stormwind guards to their original positions.
	*/

	void ReturnStormwindGuards()
	{
        Creature* pGuard1 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[1]);
        Creature* pGuard2 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[2]);
                    
		if (pGuard2 && pGuard2->isAlive())
		{
			pGuard2->GetMotionMaster()->Clear();
			pGuard2->GetMotionMaster()->MovePoint(0, -8394.4f, 450.39f, 123.76f);	
            pGuard2->SetOrientation(2.29f);
		}
		if (pGuard1 && pGuard1->isAlive())
		{
			pGuard1->GetMotionMaster()->Clear();
			pGuard1->GetMotionMaster()->MovePoint(0, -8389.66f, 453.77f, 123.76f);
            pGuard1->SetOrientation(2.29f);
		}
	}

	void startTheAttackEscort(Player* pPlayer, const Quest* qQuest)
	{
		overrideDistanceLimit(true);
		npc_escortAI::Start(false, pPlayer, qQuest, true);
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (EventTimer)
		{
			if (EventTimer <= uiDiff)
			{
                    Creature* pGuard1 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[1]);
                    Creature* pGuard2 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[2]);
                    Creature* pMarzon = m_creature->GetMap()->GetCreature(Marzon);
                    Creature* pTyrion = m_creature->GetMap()->GetCreature(Tyrion);

					switch (EventPhase)
					{
						case 1:
							if (pGuard2 && pGuard2->isAlive() && !pGuard2->getVictim())
							{
                                DoScriptText(SAY_ROYAL_GUARD_2, pGuard2);
                                pGuard2->GetMotionMaster()->MovePoint(0, -8393.10f, 451.54f,123.76f);
                                
                                                              
							}

							if (pGuard1 && pGuard1->isAlive() && !pGuard1->getVictim())
							{
                                DoScriptText(SAY_ROYAL_GUARD_2, pGuard1);
                                pGuard1->GetMotionMaster()->MovePoint(0, -8391.10f, 452.54f,123.76f);
							}

                            EventTimer = 3000;
							CanWalk = true;
							break;
						case 2:
							if (pGuard2 && pGuard2->isAlive() && !pGuard2->getVictim())
							{
                                pGuard2->GetMotionMaster()->MovePoint(0, -8357.96f,405.65f,122.27f);
                                                            
							}
							if (pGuard1 && pGuard1->isAlive() && !pGuard1->getVictim())
							{
                                pGuard1->GetMotionMaster()->MovePoint(0, -8354.72f,407.99f,122.27f); 
							}                            
                            //EventTimer = 0;
							CanWalk = true;
							break;
						case 4:
							if(pMarzon)
							{
								pMarzon->SetVisibility(VISIBILITY_ON);
								pMarzon->UpdateVisibilityAndView();
								pMarzon->GetMotionMaster()->MovePoint(0, -8406.00f, 470.00f, 123.76f);
							}
							EventTimer = 5000;
							CanWalk = false;
							break;
						case 5:
							if(pMarzon)
							{
								pMarzon->RemoveAurasDueToSpell(1784);
								pMarzon->SetFacingToObject(m_creature);
							}
                            DoScriptText(SAY_GREGOR_3, m_creature);
							EventTimer = 5000;
							CanWalk = false;
							break;
						case 6:
							if (pMarzon)
                                DoScriptText(SAY_MARZON_1, pMarzon);
							EventTimer = 5000;
							CanWalk = false;
							break;
						case 7:
                            DoScriptText(SAY_GREGOR_4, m_creature);
							EventTimer = 5000;
							CanWalk = false;
							break;
						case 8:
							
							if (pMarzon)
                                DoScriptText(SAY_MARZON_2, pMarzon);
							EventTimer = 4000;
							if(Player* pPlayer = GetPlayerForEscort())
								pPlayer->GroupEventHappens(QUEST_THE_ATTACK, m_creature);
							CanWalk = false;
							break;
						case 9:
							if (pTyrion)
                                DoScriptText(SAY_TYRION_2, pTyrion);
							EventTimer = 1000;
							CanWalk = false;
							break;
						case 10:
							if(Player* pPlayer = GetPlayerForEscort())
							{
								if (m_creature->IsWithinDist(pPlayer, 25.0f, false))
								{
									if (pMarzon)
									{
										DoScriptText(SAY_MARZON_3, pMarzon);
										pMarzon->SetFacingToObject(pPlayer);
									}
									DoScriptText(SAY_GREGOR_5, m_creature);
									m_creature->SetFacingToObject(pPlayer);
								}
							}
							EventTimer = 1000;
							CanWalk = false;
							break;
						case 11:
							if(Player* pPlayer = GetPlayerForEscort())
							{
								if (pMarzon)
								{
									pMarzon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
									pMarzon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
									pMarzon->AI()->AttackStart(pPlayer);
								}
								m_creature->setFaction(FACTION_ENEMYY);
								m_creature->AI()->AttackStart(pPlayer);
							}
							//float mX, mY, mZ;
							//m_creature->GetPosition(mX, mY, mZ);
							//if (pGuard1 && pGuard1->isAlive())
							//{
							//	pGuard1->GetMotionMaster()->Clear();
							//	pGuard1->GetMotionMaster()->MovePoint(NULL, mX - 3, mY - 2, mZ);
							//}
							//if (pGuard2 && pGuard2->isAlive())
							//{
							//	pGuard2->GetMotionMaster()->Clear();
							//	pGuard2->GetMotionMaster()->MovePoint(NULL, mX + 3, mY + 2, mZ);
							//}
					/*		EventTimer = 15000;
							CanWalk = false;
							break;*/
				//		case 12:
							//if (m_creature->isAlive() && m_creature->getVictim())
							//{
							//	if (pGuard1 && pGuard1->isAlive())
							//	{
							//		pGuard1->setFaction(FACTION_ENEMYY);
							//		pGuard1->AI()->AttackStart(pPlayer);
							//		pGuard1->GetMotionMaster()->Clear();
							//		pGuard1->GetMotionMaster()->MoveChase(pPlayer);
							//	}
							//	if (pGuard2 && pGuard2->isAlive())
							//	{
							//		pGuard2->setFaction(FACTION_ENEMYY);
							//		pGuard2->AI()->AttackStart(pPlayer);
							//		pGuard2->GetMotionMaster()->Clear();
							//		pGuard2->GetMotionMaster()->MoveChase(pPlayer);
							//	}
							//}
						/*	EventTimer = 0;
							CanWalk = false;
							break;*/
					}
					++EventPhase;
			}
				else EventTimer -= uiDiff;
		}

        Creature* pGuard1 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[1]);
        Creature* pGuard2 = m_creature->GetMap()->GetCreature(GardenRoyalGuard[2]);

		// restoring factionID
		if (pGuard1 && pGuard1->isDead() && pGuard1->getFaction() == FACTION_ENEMYY)
			pGuard1->setFaction(FACTION_NORMAL_SW_R_GUARD);
		if (pGuard2 && pGuard2->isDead() && pGuard2->getFaction() == FACTION_ENEMYY)
			pGuard2->setFaction(FACTION_NORMAL_SW_R_GUARD);
		/*if (pMarzon && pMarzon->isDead() && pMarzon->getFaction() == FACTION_ENEMYY)
			pMarzon->setFaction(FACTION_NORMAL_MARZON);*/

		if (CanWalk)
			npc_escortAI::UpdateAI(uiDiff);

		if (m_creature->SelectHostileTarget() || m_creature->getVictim())
			DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_npc_lord_gregor_lescovar(Creature* pCreature)
{
	return new npc_lord_gregor_lescovarAI (pCreature);
}

/*######
## npc_tyrion_spybot
######*/

struct MANGOS_DLL_DECL npc_tyrion_spybotAI : public npc_escortAI
{
	npc_tyrion_spybotAI(Creature* pCreature) : npc_escortAI(pCreature)
	{
		Reset();
	}

	ObjectGuid LibraryRoyalGuard[3];
	ObjectGuid LordGregor;
	const Quest* qQuest;
	uint8 LibraryGuardsCounter;
	uint32 EventTimer;
	uint8 EventPhase;
	float m_fDefaultScaleSize;
	bool CanWalk;

	void Reset()
	{
		CanWalk = true;
		if (HasEscortState(STATE_ESCORT_ESCORTING))
			return;

		LordGregor = ObjectGuid();
		qQuest = 0;
		LibraryRoyalGuard[0] = ObjectGuid();
		LibraryRoyalGuard[1] = ObjectGuid();
		LibraryRoyalGuard[2] = ObjectGuid();
		LibraryGuardsCounter = 0;

		m_fDefaultScaleSize = 0.00f;

		EventTimer = 0;
		EventPhase = 0;
	}

	void Aggro(Unit* /*pAttacker*/) {}

	bool GetStormwindRoyalGuardsInLibrary(float fMaxSearchRange = 10.0f)
	{
		std::list<Creature*> lLibraryStormwindRoyalGuards;
		GetCreatureListWithEntryInGrid(lLibraryStormwindRoyalGuards, m_creature, NPC_STORMWIND_ROYAL_GUARD, fMaxSearchRange);

		if (!lLibraryStormwindRoyalGuards.empty())
		{
			for (std::list<Creature*>::iterator iter = lLibraryStormwindRoyalGuards.begin(); iter != lLibraryStormwindRoyalGuards.end(); ++iter)
			{
				if (*iter)
				{
					Creature* LibraryStormwindRoyalGuard = (*iter);
					if (LibraryStormwindRoyalGuard->isDead() || !LibraryStormwindRoyalGuard->isAlive())
						LibraryStormwindRoyalGuard->Respawn();

					LibraryRoyalGuard[++LibraryGuardsCounter] = LibraryStormwindRoyalGuard->GetObjectGuid();
				}
				debug_log("SD0: Quest: The Attack! Filling 'LibraryRoyalGuard' variable now!");
			}
		}
		else
		{
			debug_log("SD0: Creatures 'Stormwind Royal Guard' (entry %u) which is located in Library of the Stormwind Keep has been not found for quest 'The Attack!'", NPC_STORMWIND_ROYAL_GUARD);
			return false;
		}

		return true;
	}

	void startTheAttackEscort(Player* pPlayer, const Quest* qQuest)
	{
		qQuest = qQuest;
		overrideDistanceLimit(true);
		npc_escortAI::Start(false, pPlayer, qQuest, false);
        CanWalk = false;
        EventPhase = 0;
        EventTimer = 1000;
        DoCast(m_creature, 24085);
	}

	void WaypointReached(uint32 uiPoint)
	{
		/*Player* pPlayer = GetPlayerForEscort();
		if (!pPlayer)
			return;*/

        Creature* pGuard1 = m_creature->GetMap()->GetCreature(LibraryRoyalGuard[1]);
        Creature* pGuard2 = m_creature->GetMap()->GetCreature(LibraryRoyalGuard[2]);
		
		switch (uiPoint)
		{
			case 0:
                break;
			case 4:
				if (GetStormwindRoyalGuardsInLibrary())
				{
                    DoScriptText(SAY_TYRIONA_1, m_creature);

					if (pGuard1 && pGuard1->isAlive() && !pGuard1->getVictim())
						pGuard1->SetFacingToObject(m_creature);
					if (pGuard2 && pGuard2->isAlive() && !pGuard2->getVictim())
						pGuard2->SetFacingToObject(m_creature);

					EventPhase = 1;
					EventTimer = 5000;
					CanWalk = false;
				}
				break;
			case 3:
				if (pGuard1 && pGuard1->isAlive())
					pGuard1->SetOrientation(5.430000f);
				if (pGuard2 && pGuard2->isAlive())
					pGuard2->SetOrientation(5.430000f);
				break;
			case 18:
                {
                    DoScriptText(SAY_TYRIONA_3, m_creature);
                    Creature* pLordGregor = GetClosestCreatureWithEntry(m_creature, NPC_LORD_GREGOR_LESCOVAR, VISIBLE_RANGE);
                    if (pLordGregor)
                    {
                        pLordGregor->SetFacingToObject(m_creature);
                        LordGregor = pLordGregor->GetObjectGuid();
                    }
                    EventPhase = 4;
                    EventTimer = 5000;
                    CanWalk = false;

                    break;
                }
            case 34:
                DoCast(m_creature, 24085);
                EventTimer = 1000;
                EventPhase = 8;
                m_creature->SetObjectScale(0.4f);
                CanWalk = false;

		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (EventTimer)
		{
			if (EventTimer <= uiDiff)
			{
                Creature* pGuard1 = m_creature->GetMap()->GetCreature(LibraryRoyalGuard[1]);
                Creature* pGuard2 = m_creature->GetMap()->GetCreature(LibraryRoyalGuard[2]);

				switch (EventPhase)
				{
                    case 0:
                        {
                            m_fDefaultScaleSize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
                            m_creature->SetDisplayId(6703);
                            m_creature->SetEntry(NPC_TYRIONA);
                            m_creature->SetObjectScale(1.0f);
                            m_creature->UpdateVisibilityAndView();
                            CanWalk = true;
                            break;
                        }
					case 1:
                        {
                            if (pGuard1)
                            {
                                DoScriptText(SAY_ROYAL_GUARD_1, pGuard1);
                                m_creature->SetFacingToObject(pGuard1);
                            }
                            EventTimer = 5000;
                            ++EventPhase;
                            CanWalk = false;
                            break;
                        }
					case 2:
                        {
                            DoScriptText(SAY_TYRIONA_2, m_creature);
                            if (pGuard1)
                                pGuard1->HandleEmote(EMOTE_ONESHOT_KNEEL);
                            if (pGuard2)
                                pGuard2->HandleEmote(EMOTE_ONESHOT_KNEEL);
                            ++EventPhase; // 3 = nothing (It's OK)
                            CanWalk = true;
                            break;
                        }
					case 4:
                        {
                            Creature* pLordGregor = m_creature->GetMap()->GetCreature(LordGregor);
                            if (pLordGregor)
                                DoScriptText(SAY_GREGOR_1, pLordGregor);

                            EventTimer = 5000;
                            ++EventPhase;
                            CanWalk = false;
                            break;
                        }
					case 5:
                        {
                            DoScriptText(SAY_TYRIONA_4, m_creature);
                            EventTimer = 5000;
                            ++EventPhase;
                            CanWalk = false;
                            break;
                        }
					case 6:
                        {
						    Player* pPlayer = GetPlayerForEscort();

						    if(pPlayer)
						    {
                                Creature* pLordGregor = m_creature->GetMap()->GetCreature(LordGregor);
							    if (pLordGregor)
                                {
								    if (npc_lord_gregor_lescovarAI* pGregorEscortAI = dynamic_cast<npc_lord_gregor_lescovarAI*>(pLordGregor->AI()))
									    pGregorEscortAI->startTheAttackEscort(pPlayer, qQuest);
                                }
						    }
                            EventTimer = 8000;
                            EventPhase = 7;
						    CanWalk = false; // 1/2 escort complete
						    break;
                        }
                    case 7:
                        {
                            CanWalk = true;
                            break;
                        }
                    case 8:
                        {
                            DoCast(m_creature, 1784);
                            m_creature->SetDisplayId(1159);
                            m_creature->SetEntry(NPC_TYRIONS_SPYBOT);
                            m_creature->UpdateVisibilityAndView();
                            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            EventTimer = 3000;
                            EventPhase = 9;
                            CanWalk = false;
                            break;
                        }
                    case 9:
                        {
                            CanWalk = true;
                            break;
                        }

                    default:
                        break;
				}
			}
				else EventTimer -= uiDiff;
		}

		if (CanWalk)
			npc_escortAI::UpdateAI(uiDiff);
    }
};
CreatureAI* GetAI_npc_tyrion_spybot(Creature* pCreature)
{
	return new npc_tyrion_spybotAI (pCreature);
}

/*######
## npc_tyrion
######*/

bool QuestAccept_npc_tyrion(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_THE_ATTACK)
	{
        if(GetClosestCreatureWithEntry(pCreature, NPC_LORD_GREGOR_LESCOVAR, VISIBLE_RANGE) && 
            !GetClosestCreatureWithEntry(pCreature, NPC_LORD_GREGOR_LESCOVAR, 30.0f))
        {
		    if (Creature* TyrionSpyBot = GetClosestCreatureWithEntry(pCreature, NPC_TYRIONS_SPYBOT, VISIBLE_RANGE))
		    {
			    if (npc_tyrion_spybotAI* pSpybotEscortAI = dynamic_cast<npc_tyrion_spybotAI*>(TyrionSpyBot->AI()))
			    {
                    DoScriptText(SAY_SPYBOT, TyrionSpyBot);
				    pSpybotEscortAI->startTheAttackEscort(pPlayer, pQuest);

				    // Respawn Stormwind Royal Guards (Garden)
				    std::list<Creature*> lGardenStormwindRoyalGuards;
				    GetCreatureListWithEntryInGrid(lGardenStormwindRoyalGuards, pCreature, NPC_STORMWIND_ROYAL_GUARD, 50.0f);

				    if (!lGardenStormwindRoyalGuards.empty())
				    {
					    for (std::list<Creature*>::iterator iter = lGardenStormwindRoyalGuards.begin(); iter != lGardenStormwindRoyalGuards.end(); ++iter)
					    {
						    if (*iter)
						    {
							    Creature* GardenStormwindRoyalGuard = (*iter);
							    GardenStormwindRoyalGuard->Respawn();
						    }
					    }
				    }
			    }
		    }
        }
	}
	return true;
}

/*######
## npc_reginald_windsor
######*/

enum eReginaldWindsor
{
    // Creatures
    NPC_ONYXIAS_ELITE_GUARD       = 12739,
    NPC_GENERAL_MARCUS_JONATHAN   = 466,
    NPC_HIGHLORD_BOLVAR_FORDRAGON = 1748,
    NPC_ANDUIN_WRYNN              = 1747,
    NPC_LADY_KATRANA_PRESTOR      = 1749,
    NPC_LADY_ONYXIA               = 12756,
    NPC_STORMWIND_CITY_GUARD      = 68,
    NPC_THOMAS_MILLER             = 3518,
    NPC_WINDSOR_HORSE             = 5403,

    // Misc (quests, spells, ...)
    QUEST_THE_GREAT_MASQUERADE    = 6403,
    SPELL_SUMMON_HORSE            = 13819,  // 13819 (white pony 60%) ; 23214 (charger 100%) ; correct spell id (basic white pony)
    SPELL_REDEMPTION              = 7328,   // ..for visual effect
    SPELL_ONYXIA_TRANS            = 17136,
    SPELL_TELEPORT_VISUAL         = 26638,
    SPELL_LIGHTNING_BOLT          = 12167,  // TODO: correct spell id
    MODELID_LADY_ONYXIA           = 8309,
    MODELID_DRAGONKIN             = 12529,
    MODELID_ONYXIAS_ELITE_GUARD   = 2554,
    MODELID_WINDSORS_HORSE        = 2410,   // 2410 = White Pony
	MODELID_LADY_KATRANA	      = 8769,

    // Says and Yells
    SAY_RANDOM_WINDSOR_SW_GUARD_1 = -1000611,
    SAY_RANDOM_WINDSOR_SW_GUARD_2 = -1000612,
    SAY_RANDOM_WINDSOR_SW_GUARD_3 = -1000613,
    SAY_RANDOM_WINDSOR_SW_GUARD_4 = -1000614,
    SAY_RANDOM_WINDSOR_SW_GUARD_5 = -1000615,
    SAY_RANDOM_WINDSOR_SW_GUARD_6 = -1000616,
    SAY_RANDOM_WINDSOR_SW_GUARD_7 = -1000617,
    SAY_RANDOM_WINDSOR_SW_GUARD_8 = -1000618,
    SAY_RANDOM_WINDSOR_SW_CITIZEN = -1000619,

    SAY_SW_WINDSOR_1    = -1000620,
    SAY_SW_WINDSOR_2    = -1000621,
    SAY_SW_WINDSOR_3    = -1000622, // yell

    SAY_MARCUS_1        = -1000623,
    SAY_SW_WINDSOR_4    = -1000624,
    SAY_SW_WINDSOR_5    = -1000625,
    SAY_MARCUS_2        = -1000626, // emote (appears lost in contemplation.)
    SAY_MARCUS_3        = -1000627,
    SAY_MARCUS_4        = -1000628,
    SAY_SW_WINDSOR_6    = -1000629,
    SAY_SW_WINDSOR_7    = -1000630,
    SAY_MARCUS_5        = -1000631,
    SAY_MARCUS_6        = -1000632,
    SAY_MARCUS_7        = -1000633, // yell
    SAY_MARCUS_8        = -1000634,

    SAY_SW_WINDSOR_8    = -1000635,
    SAY_SW_WINDSOR_9    = -1000636,
    SAY_SW_WINDSOR_10   = -1000637,
    SAY_BOLVAR_1        = -1000638,
    SAY_SW_WINDSOR_11   = -1000639,
    SAY_LADY_KATRANA_1  = -1000640, // emote (laughs.)
    SAY_LADY_KATRANA_2  = -1000641,
    SAY_LADY_KATRANA_3  = -1000642,
    SAY_SW_WINDSOR_12   = -1000643,
    SAY_SW_WINDSOR_13   = -1000644,
    SAY_SW_WINDSOR_14   = -1000645,

    SAY_LADY_ONYXIA     = -1000646,
    SAY_BOLVAR_2        = -1000647, // yell
    SAY_SW_WINDSOR_15   = -1000648,
    SAY_BOLVAR_3        = -1000649, // emote (medallion shatters.)
    SAY_BOLVAR_4        = -1000650,
    SAY_SW_WINDSOR_16   = -1000651,
    SAY_SW_WINDSOR_17   = -1000652, // emote (dies.)
};

struct MANGOS_DLL_DECL npc_reginald_windsorAI : public npc_escortAI
{
    npc_reginald_windsorAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    uint32 m_uiLastReachedWaypointId;
    uint32 m_uiEventPhase;
    uint32 m_uiEventTimer;

    ObjectGuid m_uiEscorterGUID;
    ObjectGuid m_uiWindsorsHorseGUID;
    ObjectGuid m_uiMarcusGUID;
    ObjectGuid m_uiAnduinWrynnGUID;
    ObjectGuid m_uiMarcusGuardsGUID[6];

    Creature* MarcusGuard[6];

    GUIDList m_uiKneelAffectedNpcsList;
    GUIDList m_uiLadyOnyxiasSpawns;
    GUIDList m_uiSwRoyalGuards;

    bool bCanWalk;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        m_uiLastReachedWaypointId = 0;
        m_uiEventPhase = 0;
        m_uiEventTimer = 0;

		m_uiEscorterGUID.Clear();
		m_uiWindsorsHorseGUID.Clear();
		m_uiMarcusGUID.Clear();
		m_uiAnduinWrynnGUID.Clear();

        for(uint8 itr = 0; itr < 6; ++itr)
        {
            MarcusGuard[itr] = 0;
			m_uiMarcusGuardsGUID[itr].Clear();
        }

        m_uiKneelAffectedNpcsList.clear();
        m_uiLadyOnyxiasSpawns.clear();
        m_uiSwRoyalGuards.clear();

        bCanWalk = true;
    }

    void JustDied(Unit *)
    {
        if (GetPlayerForWindsorEscort())
        {
            //GetPlayerForWindsorEscort()->GetGroup();
            GetPlayerForWindsorEscort()->FailQuest(QUEST_THE_GREAT_MASQUERADE);
			
			m_uiEventTimer = 1000;
			m_uiEventPhase = 121;
			UpdateAI(2000);		// Force the reset state by manually updating the AI.
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        // Stormwind City guards and citizens will random say and kneel before Reginald Windsor.
        if (HasEscortState(STATE_ESCORT_ESCORTING) && m_creature->IsWithinDist(pWho, 10.0f))
        {
            if (pWho->GetObjectGuid().IsCreature())
            {
                if (m_uiLastReachedWaypointId >= 29)
                    return;

                // Reginald Windsor's horse can't saying anything. :-D
                if (pWho->GetDisplayId() == MODELID_WINDSORS_HORSE)
                    return;

                // Exceptions.
                switch(pWho->GetEntry())
                {
                    case NPC_GENERAL_MARCUS_JONATHAN:
                    case NPC_LADY_KATRANA_PRESTOR:
                    case NPC_ANDUIN_WRYNN:
                    case NPC_HIGHLORD_BOLVAR_FORDRAGON:
                        return;
                    case 2533:  // William (boy)
                    case 2532:  // Donna (girl)
                        pWho->StopMoving();
                        pWho->GetMotionMaster()->MovementExpired();
                        pWho->SetFacingToObject(m_creature);
						m_uiKneelAffectedNpcsList.push_back(pWho->GetObjectGuid());
                        return;
                }

                for(uint8 itr = 0; itr < 6; ++itr)
                    if (MarcusGuard[itr])
                        if (pWho->GetObjectGuid() == MarcusGuard[itr]->GetObjectGuid())
                            return;

                if (!m_uiKneelAffectedNpcsList.empty())
                    for(GUIDList::iterator itr = m_uiKneelAffectedNpcsList.begin(); itr != m_uiKneelAffectedNpcsList.end(); ++itr)
                        if (pWho->GetObjectGuid() == (*itr))
                            return;

                switch(rand()%9)
                {
                    case 0: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_1, pWho); break;
                    case 1: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_2, pWho); break;
                    case 2: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_3, pWho); break;
                    case 3: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_4, pWho); break;
                    case 4: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_5, pWho); break;
                    case 5: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_6, pWho); break;
                    case 6: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_7, pWho); break;
                    case 7: DoScriptText(SAY_RANDOM_WINDSOR_SW_GUARD_8, pWho); break;
                    case 8: DoScriptText(SAY_RANDOM_WINDSOR_SW_CITIZEN, pWho); break;
                }

                pWho->StopMoving();
                pWho->GetMotionMaster()->MovementExpired();
                pWho->SetFacingToObject(m_creature);
                // 50% chance to kneel.
                if (rand()%2 == 1)
                    pWho->SetStandState(UNIT_STAND_STATE_KNEEL);
                m_uiKneelAffectedNpcsList.push_back(pWho->GetObjectGuid());
            }
            // During escort Reginald Windsor ignoring aggro.
        }
        else
        {
            npc_escortAI::MoveInLineOfSight(pWho);
        }
    }

    Player* GetPlayerForWindsorEscort()
    {
        return m_creature->GetMap()->GetPlayer(m_uiEscorterGUID);
    }

    void JustStartedEscort()
    {
        // Reginald Windsor become immortal and ignoring aggro.
        // His mount aswell.
        if (m_creature->IsMounted())
            m_creature->Unmount();
        m_creature->Mount(MODELID_WINDSORS_HORSE, SPELL_SUMMON_HORSE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
    }

    void WaypointReached(uint32 uiPointId)
    {
        m_uiLastReachedWaypointId = uiPointId;

        switch(uiPointId)
        {
            case 1:
                SetEscortPaused(true);
                m_uiEventPhase = 10;
                m_uiEventTimer = 2000;
                break;
            case 4:
                SetEscortPaused(true);
                m_uiEventPhase = 30;
                m_uiEventTimer = 2000;
                break;
            case 24:
                SetEscortPaused(true);
                m_uiEventPhase = 70;
                m_uiEventTimer = 2000;
                break;
            case 29:
                //SetEscortPaused(true);
                bCanWalk = false;
                m_uiEventPhase = 90;
                m_uiEventTimer = 2000;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        Creature* pMarcus = NULL;
        Creature* pLadyKatrana = NULL;
        Creature* pAnduinWrynn = NULL;
        Creature* pBolvar = NULL;

        std::list<Creature*> m_uiGeneralMarcusList;
        std::list<Creature*> m_uiLadyKatranaList;
        std::list<Creature*> m_uiAnduinWrynnList;
        std::list<Creature*> m_uiBolvarList;

        if (m_uiLastReachedWaypointId <= 4)
        {
            // General Marcus Jonathan
            GetCreatureListWithEntryInGrid(m_uiGeneralMarcusList, m_creature, NPC_GENERAL_MARCUS_JONATHAN, 150.0f);
            if (!m_uiGeneralMarcusList.empty())
            {
                std::list<Creature*>::iterator itr = m_uiGeneralMarcusList.begin();
                std::advance(itr, (m_uiGeneralMarcusList.size()-1));
                pMarcus = (*itr);
                m_uiMarcusGUID = pMarcus->GetObjectGuid();
            }
        }
        if (m_uiLastReachedWaypointId >= 29)
        {
            // Lady KatranaPrestor
            GetCreatureListWithEntryInGrid(m_uiLadyKatranaList, m_creature, NPC_LADY_KATRANA_PRESTOR, 40.0f);
            if (!m_uiLadyKatranaList.empty())
            {
                std::list<Creature*>::iterator itr = m_uiLadyKatranaList.begin();
                std::advance(itr, (m_uiLadyKatranaList.size()-1));
                pLadyKatrana = (*itr);
            }
            // Anduin Wrynn
            GetCreatureListWithEntryInGrid(m_uiAnduinWrynnList, m_creature, NPC_ANDUIN_WRYNN, 40.0f);
            if (!m_uiAnduinWrynnList.empty())
            {
                std::list<Creature*>::iterator itr = m_uiAnduinWrynnList.begin();
                std::advance(itr, (m_uiAnduinWrynnList.size()-1));
                pAnduinWrynn = (*itr);
            }
            // Highlord Bolvar Fordragon
            GetCreatureListWithEntryInGrid(m_uiBolvarList, m_creature, NPC_HIGHLORD_BOLVAR_FORDRAGON, 40.0f);
            if (!m_uiBolvarList.empty())
            {
                std::list<Creature*>::iterator itr = m_uiBolvarList.begin();
                std::advance(itr, (m_uiBolvarList.size()-1));
                pBolvar = (*itr);
            }
        }

        if (!GetPlayerForWindsorEscort())
            return;

		if (GetPlayerForEscort() && GetPlayerForEscort()->GetQuestStatus(QUEST_THE_GREAT_MASQUERADE) == QuestStatus::QUEST_STATUS_FAILED)
			ResetNPCs(pMarcus, pAnduinWrynn, pLadyKatrana, pBolvar);

        if (m_uiEventTimer)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                m_uiEventTimer = 0; // Don't delete it!

                switch(m_uiEventPhase)
                {
                    case 10:
                        if (m_creature->IsMounted())
                            m_creature->Unmount();
                        float x,y,z;
                        m_creature->GetPosition(x,y,z);
                        if (Creature* pWindsorsHorse = m_creature->SummonCreature(NPC_WINDSOR_HORSE, x+1, y+2, z, m_creature->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0))
                        {
                            m_uiWindsorsHorseGUID = pWindsorsHorse->GetObjectGuid();
                            pWindsorsHorse->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE + UNIT_FLAG_NOT_SELECTABLE);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 11:
                        DoScriptText(SAY_SW_WINDSOR_1, m_creature);    //Yawww!
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_ATTACKUNARMED);
                        m_uiEventTimer = 800;
                        break;
                    case 12:
                        if (Creature* pWindsorsHorse = m_creature->GetMap()->GetCreature(m_uiWindsorsHorseGUID))
                        {
                            if (pWindsorsHorse->HasSplineFlag(SPLINEFLAG_WALKMODE))
                                pWindsorsHorse->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pWindsorsHorse->GetMotionMaster()->MovePoint(0, -9104.79f, 385.41f, 93.51f);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 13:
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                        m_creature->SetFacingToObject(GetPlayerForWindsorEscort());
                        DoScriptText(SAY_SW_WINDSOR_2, m_creature, GetPlayerForWindsorEscort());
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        // Escort will continue by accepting next quest in chain.
                        break;

                    case 20:
                        m_creature->SetFacingTo(0.67f);
                        DoScriptText(SAY_SW_WINDSOR_3, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 4000;
                        break;
                    case 21:
                        SetEscortPaused(false);
                        // Spawn Marcus's guards
                        if (pMarcus)
                        {
                            if (pMarcus->isDead())
                                pMarcus->Respawn();
                            if (pMarcus->IsMounted())
                                pMarcus->Unmount();
                            pMarcus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pMarcus->Relocate(-8968.43f, 509.64f, 96.35f, 3.88f);
                        }  
                        MarcusGuard[0] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8962.34f, 502.40f, 96.59f, 3.83f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        MarcusGuard[1] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8964.74f, 505.40f, 96.59f, 3.81f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        MarcusGuard[2] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8966.57f, 507.70f, 96.35f, 3.83f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        MarcusGuard[3] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8969.55f, 511.73f, 96.35f, 3.83f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        MarcusGuard[4] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8971.61f, 514.26f, 96.59f, 3.83f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        MarcusGuard[5] = m_creature->SummonCreature(NPC_STORMWIND_CITY_GUARD, -8974.13f, 517.34f, 96.59f, 3.83f, TEMPSUMMON_DEAD_DESPAWN, 0);
                        for(uint8 itr = 0; itr < 6; ++itr)
                            if (MarcusGuard[itr])
                            {
                                MarcusGuard[itr]->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                                MarcusGuard[itr]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            }
                        break;

                    case 30:
                        if (pMarcus)
                        {
                            DoScriptText(SAY_MARCUS_1, pMarcus);
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 6000;
                        break;
                    case 31:
                        DoScriptText(SAY_SW_WINDSOR_4, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 10000;
                        break;
                    case 32:
                        DoScriptText(SAY_SW_WINDSOR_5, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 6000;
                        break;
                    case 33:
                        if (pMarcus)
                        {
                            DoScriptText(SAY_MARCUS_2, pMarcus);
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 4000;
                        break;
                    case 34:
                        if (pMarcus)
                            DoScriptText(SAY_MARCUS_3, pMarcus);
                        m_uiEventTimer = 12000;
                        break;
                    case 35:
                        if (pMarcus)
                        {
                            DoScriptText(SAY_MARCUS_4, pMarcus);
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 8000;
                        break;
                    case 36:
                        DoScriptText(SAY_SW_WINDSOR_6, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 12000;
                        break;
                    case 37:
                        DoScriptText(SAY_SW_WINDSOR_7, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 8000;
                        break;
                    case 38:
                        if (pMarcus)
                        {
                            DoScriptText(SAY_MARCUS_5, pMarcus);
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                            pMarcus->SetFacingTo(5.38f);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 39:
                        // First side of Marcus's guards
                        if (MarcusGuard[0])
                        {
                            MarcusGuard[0]->SetFacingTo(2.23f);
                            MarcusGuard[0]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        if (MarcusGuard[1])
                            MarcusGuard[1]->GetMotionMaster()->MovePoint(0, -8960.99f, 503.33f, 96.59f);
                        if (MarcusGuard[2])
                            MarcusGuard[2]->GetMotionMaster()->MovePoint(0, -8958.96f, 504.91f, 96.59f);
                        m_uiEventTimer = 4500;
                        break;
                    case 40:
                        // Do-Kneel first side of Marcus's guards
                        if (MarcusGuard[1])
                        {
                            MarcusGuard[1]->SetFacingTo(2.23f);
                            MarcusGuard[1]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        if (MarcusGuard[2])
                        {
                            MarcusGuard[2]->SetFacingTo(2.23f);
                            MarcusGuard[2]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 41:
                        // Second side of Marcus's guards
                        // Rotate Marcus
                        if (pMarcus)
                            pMarcus->SetFacingTo(2.28f);
                        m_uiEventTimer = 2000;
                        break;
                    case 42:
                        if (pMarcus)
                        {
                            DoScriptText(SAY_MARCUS_6, pMarcus);
                            pMarcus->HandleEmote(EMOTE_ONESHOT_LAND);
                        }
                        m_uiEventTimer = 1000;
                        break;
                    case 43:
                        // Move guards
                        if (MarcusGuard[5])
                        {
                            MarcusGuard[5]->SetFacingTo(5.39f);
                            MarcusGuard[5]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        if (MarcusGuard[4])
                        {
                            MarcusGuard[4]->GetMotionMaster()->MovePoint(0, -8972.82f, 518.37f, 96.59f);
                        }
                        if (MarcusGuard[3])
                        {
                            MarcusGuard[3]->GetMotionMaster()->MovePoint(0, -8970.77f, 520.09f, 96.59f);
                        }
                        m_uiEventTimer = 4000;
                        break;
                    case 44:
                        // Do-Kneel second side of Marcus's guards
                        if (MarcusGuard[4])
                        {
                            MarcusGuard[4]->SetFacingTo(5.39f);
                            MarcusGuard[4]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        if (MarcusGuard[3])
                        {
                            MarcusGuard[3]->SetFacingTo(5.39f);
                            MarcusGuard[3]->SetStandState(UNIT_STAND_STATE_KNEEL);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 45:
                        if (pMarcus)
                        {
                            pMarcus->SetFacingToObject(m_creature);
                            DoScriptText(SAY_MARCUS_7, pMarcus);
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 5000;
                        break;
                    case 46:
                        if (pMarcus)
                            pMarcus->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                        m_uiEventTimer = 3000;
                        break;
                    case 47:
                        if (pMarcus)
                            DoScriptText(SAY_MARCUS_8, pMarcus);
                        m_uiEventTimer = 4000;
                        break;
                    case 48:
                        // This ->MovePoint generating an error in error log. :-/
                        if (pMarcus)
                            pMarcus->GetMotionMaster()->MovePoint(0, -8977.37f, 510.82f, 96.60f);
                        m_uiEventTimer = 3000;
                        break;
                    case 49:
                        if (pMarcus)
                            pMarcus->SetFacingToObject(m_creature);
                        m_uiEventTimer = 1500;
                        break;
                    case 50:
                        SetEscortPaused(false);
                        break;

                    case 70:
                        m_creature->SetFacingTo(5.40f);
                        DoScriptText(SAY_SW_WINDSOR_8, m_creature);
                        m_uiEventTimer = 2000;
                        break;
                    case 71:
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;

                    case 80:
                        DoScriptText(SAY_SW_WINDSOR_9, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                        m_uiEventTimer = 2500;
                        break;
                    case 81:
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                        m_uiEventTimer = 2500;
                        break;
                    case 82:
                        SetEscortPaused(false);
                        break;

                    case 90:
                        // Respawn needed NPC(s)
                        if (pBolvar)
                            if (pBolvar->isDead())
                                pBolvar->Respawn();
                        if (pLadyKatrana)
                            if (pLadyKatrana->isDead())
                                pLadyKatrana->Respawn();
                        if (pAnduinWrynn)
                            if (pAnduinWrynn->isDead())
                                pAnduinWrynn->Respawn();
                        m_uiEventTimer = 500;
                        break;
                    case 91:
                        // Remove Unit NPC flags.
                        if (pBolvar)
                            pBolvar->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                        if (pLadyKatrana)
                            pLadyKatrana->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                        DoScriptText(SAY_SW_WINDSOR_10, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 5000;
                        break;
                    case 92:
                        if (pBolvar)
                        {
                            DoScriptText(SAY_BOLVAR_1, pBolvar);
                            pBolvar->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 93:
                        if (pAnduinWrynn)
                        {
                            // Anduin Wrynn will run away..
                            m_uiAnduinWrynnGUID = pAnduinWrynn->GetObjectGuid();
                            if (pAnduinWrynn->HasSplineFlag(SPLINEFLAG_WALKMODE))
                                pAnduinWrynn->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pAnduinWrynn->GetMotionMaster()->MovePoint(0, -8505.08f, 338.17f, 120.88f);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 94:
                        DoScriptText(SAY_SW_WINDSOR_11, m_creature);
                        m_uiEventTimer = 6000;
                        break;
                    case 95:
                        if (pLadyKatrana)
                        {
                            DoScriptText(SAY_LADY_KATRANA_1, pLadyKatrana);
                            pLadyKatrana->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                        }
                        m_uiEventTimer = 3000;
                        break;
                    case 96:
                        if (pLadyKatrana)
                        {
                            DoScriptText(SAY_LADY_KATRANA_2, pLadyKatrana);
                            pLadyKatrana->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 10000;
                        break;
                    case 97:
                        if (pLadyKatrana)
                        {
                            DoScriptText(SAY_LADY_KATRANA_3, pLadyKatrana);
                            pLadyKatrana->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 10000;
                        break;
                    case 98:
                        DoScriptText(SAY_SW_WINDSOR_12, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 8000;
                        break;
                    case 99:
                        DoScriptText(SAY_SW_WINDSOR_13, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 8000;
                        break;
                    case 100:
                        DoScriptText(SAY_SW_WINDSOR_14, m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        m_uiEventTimer = 8000;
                        break;
                    case 101:
                        DoCastSpellIfCan(pLadyKatrana, SPELL_REDEMPTION, CAST_FORCE_CAST);
                        m_uiEventTimer = 10500;
                        break;
                    case 102:
                        if (pLadyKatrana)
                        {
                            pLadyKatrana->SetName("Lady Onyxia");
                            //pLadyKatrana->CastSpell(pLadyKatrana, SPELL_ONYXIA_TRANS, true);
                            //pLadyKatrana->SetDisplayId(MODELID_LADY_ONYXIA);
                            pLadyKatrana->SetDisplayId(MODELID_DRAGONKIN);
                        }
                        m_uiEventTimer = 1500;
                        break;
                    case 103:
                        if (pBolvar)
                        {
                            if (pBolvar->HasSplineFlag(SPLINEFLAG_WALKMODE))
                                pBolvar->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pBolvar->GetMotionMaster()->MovePoint(0, -8450.43f, 336.85f, 121.32f);
                        }
                        m_uiEventTimer = 1000;
                        break;
                    case 104:
                        if (pLadyKatrana)
                            DoScriptText(SAY_LADY_ONYXIA, pLadyKatrana);
                        m_uiEventTimer = 1200;
                        break;
                    case 105:
                        if (pBolvar && pLadyKatrana)
                            pBolvar->SetFacingToObject(pLadyKatrana);
                        m_uiEventTimer = 3000;
                        break;
                    case 106:
                        if (pBolvar)
                            DoScriptText(SAY_BOLVAR_2, pBolvar);
                        m_uiEventTimer = 2000;
                        break;
                    case 107:
                        if (pLadyKatrana)
                        {
                            std::list<Creature*> m_uiSwRoyalGuardList;
                            GetCreatureListWithEntryInGrid(m_uiSwRoyalGuardList, pLadyKatrana, NPC_STORMWIND_ROYAL_GUARD, 20.0f);
                            if (!m_uiSwRoyalGuardList.empty())
                            {
                                for(std::list<Creature*>::const_iterator itr = m_uiSwRoyalGuardList.begin(); itr != m_uiSwRoyalGuardList.end(); ++itr)
                                    if ((*itr))
                                    {
                                        m_uiSwRoyalGuards.push_back((*itr)->GetObjectGuid());

                                        // Replace Stormwind Royal Guards by Onyxia's Elite Guards
                                        float x,y,z;
                                        (*itr)->GetPosition(x,y,z);
                                        if (Creature* pLadyOnyxiaSpawn = m_creature->SummonCreature(NPC_ONYXIAS_ELITE_GUARD, x, y, (z + 1.0f), (*itr)->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0))
                                        {
                                            pLadyOnyxiaSpawn->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                            m_uiLadyOnyxiasSpawns.push_back(pLadyOnyxiaSpawn->GetObjectGuid());
                                        }
                                        (*itr)->SetHealth(0);
                                        (*itr)->SetDeathState(JUST_DIED);
                                        (*itr)->RemoveCorpse();
                                    }
                            }
                        }
                        m_uiEventTimer = 3000;
                        break;
                    case 108:
                        DoScriptText(SAY_SW_WINDSOR_15, m_creature);
                        m_uiEventTimer = 1000;
                        break;
                    case 109:
                        /*if (pLadyKatrana)
                            pLadyKatrana->CastSpell(m_creature, SPELL_LIGHTNING_BOLT, true);*/
                        m_uiEventTimer = 1000;
                        break;
                    case 110:
                        // Onyxia will begin cast teleport.
                        if (pLadyKatrana)
                            pLadyKatrana->CastSpell(pLadyKatrana, SPELL_TELEPORT_VISUAL, true);
                        // Reginald collapse.
                        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                        m_creature->SetHealth(1);
                        m_uiEventTimer = 1000;
                        break;
                    case 111:
                        if (pLadyKatrana)
                            pLadyKatrana->SetVisibility(VISIBILITY_OFF);
                        if (pBolvar)
                        {
                            pBolvar->setFaction(FACTION_ESCORT_A_NEUTRAL_ACTIVE);
                            pBolvar->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                        }
                        // Make Lady Onyxia's spawns hostile.
                        if (!m_uiLadyOnyxiasSpawns.empty())
                            for(GUIDList::iterator itr = m_uiLadyOnyxiasSpawns.begin(); itr != m_uiLadyOnyxiasSpawns.end(); ++itr)
                                if (Creature* pOnyxiaSpawn = m_creature->GetMap()->GetCreature(*itr))
                                {
                                    pOnyxiaSpawn->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
                                    if (pBolvar)
                                    {
                                        pOnyxiaSpawn->GetMotionMaster()->MoveChase(pBolvar);
                                        pOnyxiaSpawn->Attack(pBolvar, true);
                                    }
                                }
                        m_uiEventTimer = 5000;
                        break;
                    case 112:
                        debug_log("SD0: Stormwind City: Escort Reginald Windsor: Checking, if Lady Onyxia's spawns are alive.");
                        // Check, if all dragonkins are dead.
                        if (!m_uiLadyOnyxiasSpawns.empty())
                        {
                            bool m_bCheckAgain = false;

                            for(GUIDList::iterator itr = m_uiLadyOnyxiasSpawns.begin(); itr != m_uiLadyOnyxiasSpawns.end(); ++itr)
                            {
                                if (Creature* pOnyxiaSpawn = m_creature->GetMap()->GetCreature(*itr))
                                {
                                    if (!pOnyxiaSpawn->isDead() && pOnyxiaSpawn->isAlive())
                                        m_bCheckAgain = true;
                                }
                                else
                                {
                                    error_log("SD0: Stormwind City: Escort Reginald Windsor: Unable to select creature (Lady Onyxia's spawn - Onyxia's Elite Guard). Removing from list.");
                                    itr = m_uiLadyOnyxiasSpawns.erase(itr);
                                }
                            }
                            // Repeat this phase until all Lady Onyxia's spawns are not dead.
                            if (m_bCheckAgain)
                                --m_uiEventPhase;
                        }
                        else
                            debug_log("SD0: Stormwind City: Escort Reginald Windsor: All spawns of Lady Onyxia has been defeated. Escort will be completed soon.");
                        m_uiEventTimer = 2500;
                        break;
                    case 113:
                        if (pBolvar)
                        {
                            if (pBolvar->isInCombat())
                                pBolvar->CombatStop();
                            pBolvar->StopMoving();
                            pBolvar->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            float x,y,z;
                            m_creature->GetPosition(x,y,z);
                            if (pBolvar->HasSplineFlag(SPLINEFLAG_WALKMODE))
                                pBolvar->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pBolvar->GetMotionMaster()->MovementExpired();
                            pBolvar->GetMotionMaster()->MovePoint(0, x-1.0f, y-1.0f, z);
                        }
                        m_uiEventTimer = 2000;
                        break;
                    case 114:
                        if (pBolvar && !pBolvar->IsWithinDist(m_creature, 4.0f))
                        {
                            float x,y,z;
                            m_creature->GetPosition(x,y,z);
                            if (pBolvar->HasSplineFlag(SPLINEFLAG_WALKMODE))
                                pBolvar->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pBolvar->GetMotionMaster()->MovementExpired();
                            pBolvar->GetMotionMaster()->MovePoint(0, x-2, y-2, z);
                            --m_uiEventPhase;
                        }
                        else if (pBolvar && pBolvar->IsWithinDist(m_creature, 4.0f))
                        {
                            pBolvar->StopMoving();
                            pBolvar->SetFacingToObject(m_creature);
                        }
                        else
                        {
                            error_log("SD0: Stormwind City: Escort Reginald Windsor: Switch 'case' 114: Pointer pBolvar is NULL!");
                        }
                        m_uiEventTimer = 1000;
                        break;
                    case 115:
                        if (pBolvar)
                            pBolvar->SetStandState(UNIT_STAND_STATE_KNEEL);
                        m_uiEventTimer = 1500;
                        break;
                    case 116:
                        if (pBolvar)
                            DoScriptText(SAY_BOLVAR_3, pBolvar);
                        m_uiEventTimer = 2000;
                        break;
                    case 117:
                        if (pBolvar)
                        {
                            DoScriptText(SAY_BOLVAR_4, pBolvar);
                            pBolvar->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        m_uiEventTimer = 3000;
                        break;
                    case 118:
                        DoScriptText(SAY_SW_WINDSOR_16, m_creature);
                        m_uiEventTimer = 10000;
                        break;
                    case 119:
                        // Reginald Windsor dies. (emote)
                        DoScriptText(SAY_SW_WINDSOR_17, m_creature);
                        m_uiEventTimer = 2000;
                        break;
                    case 120:
                        if (pBolvar)
                            pBolvar->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
                        GetPlayerForWindsorEscort()->GroupEventHappens(QUEST_THE_GREAT_MASQUERADE, m_creature);
                        m_uiEventTimer = 20000;
                        break;
					case 121:
						ResetNPCs(pMarcus, pAnduinWrynn, pLadyKatrana, pBolvar);
                        
						m_uiEventTimer = 1000;
                        break;
                    case 122:
                        bCanWalk = true;    // to complete escort.
                        break;
                }
                ++m_uiEventPhase;
            }
            else
                m_uiEventTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() || m_creature->getVictim())
            DoMeleeAttackIfReady();

        if (bCanWalk)
            npc_escortAI::UpdateAI(uiDiff);
    }

	void ResetNPCs(Creature* pMarcus, Creature* pAnduinWrynn, Creature* pLadyKatrana, Creature* pBolvar)
	{
		// Reset all affected objects (e.g. Marcus on the Bridge of Heroes, NPCs of the Stormwind Keep, etc..)
        for(uint8 itr = 0; itr < 6; ++itr)
            if (MarcusGuard[itr])
                MarcusGuard[itr]->RemoveFromWorld();
        if (Creature* pWinsdorsHorse = m_creature->GetMap()->GetCreature(m_uiWindsorsHorseGUID))
            pWinsdorsHorse->RemoveFromWorld();
        if ((pMarcus = m_creature->GetMap()->GetCreature(m_uiMarcusGUID)))
        {
            pMarcus->SetHealth(0);
            pMarcus->SetDeathState(JUST_DIED);
            pMarcus->Respawn();
        }
        if ((pAnduinWrynn = m_creature->GetMap()->GetCreature(m_uiAnduinWrynnGUID)))
        {
            pAnduinWrynn->SetHealth(0);
            pAnduinWrynn->SetDeathState(JUST_DIED);
            //pAnduinWrynn->RemoveCorpse();
            pAnduinWrynn->Respawn();
        }
        if (pLadyKatrana)
        {
            pLadyKatrana->SetDisplayId(MODELID_LADY_KATRANA);
            pLadyKatrana->SetHealth(0);
            pLadyKatrana->SetDeathState(JUST_DIED);
            //pLadyKatrana->RemoveCorpse();
            pLadyKatrana->Respawn();
            pLadyKatrana->SetVisibility(VISIBILITY_ON);
        }
        if (pBolvar)
        {
            pBolvar->setFaction(pBolvar->GetCreatureInfo()->faction_A);
            pBolvar->SetStandState(UNIT_STAND_STATE_STAND);
            pBolvar->SetHealth(0);
            pBolvar->SetDeathState(JUST_DIED);
            //pBolvar->RemoveCorpse();
            pBolvar->Respawn();
        }

		// Respawn guards in throne hall.
        if (!m_uiSwRoyalGuards.empty())
            for(GUIDList::iterator itr = m_uiSwRoyalGuards.begin(); itr != m_uiSwRoyalGuards.end(); ++itr)
                if (Creature* pSwRoyalGuard = m_creature->GetMap()->GetCreature(*itr))
                    pSwRoyalGuard->Respawn();
        // To stand kneeled npcs.
        if (!m_uiKneelAffectedNpcsList.empty())
            for(GUIDList::iterator itr = m_uiKneelAffectedNpcsList.begin(); itr != m_uiKneelAffectedNpcsList.end(); ++itr)
                if (Creature* pTarget = m_creature->GetMap()->GetCreature(*itr))
                {
                    pTarget->SetStandState(UNIT_STAND_STATE_STAND);
                    pTarget->StopMoving();
                    if (pTarget->isInCombat())
                        pTarget->CombatStop();
                    pTarget->GetMotionMaster()->MovementExpired();
                    pTarget->SetHealth(0);
                    pTarget->SetDeathState(JUST_DIED);
                    pTarget->Respawn();
                }

        m_uiKneelAffectedNpcsList.clear();
        // Remove Lady Onyxia's spawns.
        if (!m_uiLadyOnyxiasSpawns.empty())
            for(GUIDList::iterator itr = m_uiLadyOnyxiasSpawns.begin(); itr != m_uiLadyOnyxiasSpawns.end(); ++itr)
                if (Creature* pTarget = m_creature->GetMap()->GetCreature(*itr))
                    pTarget->RemoveFromWorld();
	}

};

CreatureAI* GetAI_npc_reginald_windsor(Creature* pCreature)
{
    return new npc_reginald_windsorAI(pCreature);
}

bool GossipHello_npc_reginald_windsor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (npc_reginald_windsorAI* pEscortAI = dynamic_cast<npc_reginald_windsorAI*>(pCreature->AI()))
    {
        if (pEscortAI->GetPlayerForWindsorEscort())
        {
			if (pPlayer->GetObjectGuid() == pEscortAI->GetPlayerForWindsorEscort()->GetObjectGuid())
            {
                if ((pEscortAI->HasEscortState(STATE_ESCORT_ESCORTING) || pEscortAI->HasEscortState(STATE_ESCORT_PAUSED) ))
                {
                    // Only true escorter can let the escort continues.
                    if (pPlayer->GetObjectGuid() != pEscortAI->GetPlayerForWindsorEscort()->GetObjectGuid())
                        return true;

                    if (pEscortAI->m_uiLastReachedWaypointId == 24)
                    {
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I am ready, as are my forces. Let us end this masquerade!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        pPlayer->SEND_GOSSIP_MENU(5633, pCreature->GetObjectGuid());
                        return true;
                    }
                }
            }
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_reginald_windsor(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_reginald_windsorAI* pEscortAI = dynamic_cast<npc_reginald_windsorAI*>(pCreature->AI()))
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pEscortAI->m_uiEventPhase = 80;
            pEscortAI->m_uiEventTimer = 2000;
        }
    }
    return true;
}

bool QuestAccept_npc_reginald_windsor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_THE_GREAT_MASQUERADE)
    {
        if (npc_reginald_windsorAI* pEscortAI = dynamic_cast<npc_reginald_windsorAI*>(pCreature->AI()))
        {
            // Only true escort player can let escort continues.
            if (pEscortAI->GetPlayerForWindsorEscort())
                if (pEscortAI->GetPlayerForWindsorEscort()->GetObjectGuid() != pPlayer->GetObjectGuid())
                {
                    //
                    return true;
                }

            // Reginald Windsor will continue to the Stormwind City over bridge.
            // There stop him General Marcus Jonathan - Scripted dialog.
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP + UNIT_NPC_FLAG_QUESTGIVER);
            pEscortAI->m_uiEventPhase = 20;
            pEscortAI->m_uiEventTimer = 2000;
        }
    }
    return true;
}

/*######
## npc_squire_rowe
######*/

enum eSquireRowe
{
    QUEST_STORMWIND_RENDEZVOUS = 6402,
    NPC_REGINALD_WINDSOR       = 12580,
};

struct MANGOS_DLL_DECL npc_squire_roweAI : public npc_escortAI
{
    npc_squire_roweAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_uiReginaldWindsorGUID;
    uint32 m_uiChangeStandStateTimer;
    bool bStandStateChanged;

    void Reset()
    {
        m_uiChangeStandStateTimer = 3500;
        bStandStateChanged = false;
		m_uiReginaldWindsorGUID.Clear();
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 3:
                SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 4:
                if (Creature* pReginaldWindsor = m_creature->SummonCreature(NPC_REGINALD_WINDSOR, -9140.45f, 379.28f, 90.67f, 0.7f, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    if (GetPlayerForEscort())
                        if (npc_reginald_windsorAI* pEscortAI = dynamic_cast<npc_reginald_windsorAI*>(pReginaldWindsor->AI()))
                        {
							m_uiReginaldWindsorGUID = pReginaldWindsor->GetObjectGuid();
                            pEscortAI->Start(false, 0, 0, true);
                            pEscortAI->m_uiEscorterGUID = GetPlayerForEscort()->GetObjectGuid();
                        }
                }
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Stand again and run to the default spawn point.
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (!bStandStateChanged && m_uiChangeStandStateTimer <= uiDiff)
            {
                bStandStateChanged = true;
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                SetEscortPaused(false);
            }
            else
                m_uiChangeStandStateTimer -= uiDiff;
        }

        npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_squire_rowe(Creature* pCreature)
{
    return new npc_squire_roweAI(pCreature);
}

bool GossipHello_npc_squire_rowe(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_STORMWIND_RENDEZVOUS) == QUEST_STATUS_COMPLETE && pPlayer->GetQuestStatus(QUEST_THE_GREAT_MASQUERADE) != QUEST_STATUS_COMPLETE)
    {
        if (npc_squire_roweAI* pEscortAI = dynamic_cast<npc_squire_roweAI*>(pCreature->AI()))
        {
            if (Creature* pReginaldWindsor = pCreature->GetMap()->GetCreature(pEscortAI->m_uiReginaldWindsorGUID))
            {
                if (npc_reginald_windsorAI* pWindsorEscortAI = dynamic_cast<npc_reginald_windsorAI*>(pReginaldWindsor->AI()))
                {
                    if (!pWindsorEscortAI->HasEscortState(STATE_ESCORT_ESCORTING) && !pWindsorEscortAI->HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        // Prepare GUID variable for new value.
                        pReginaldWindsor->StopMoving();
                        if (pReginaldWindsor->isInCombat())
                            pReginaldWindsor->CombatStop();
                        pReginaldWindsor->RemoveFromWorld();
						pEscortAI->m_uiReginaldWindsorGUID.Clear();
                        pWindsorEscortAI = NULL;

                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let Marshal Windsor know that I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                        pPlayer->SEND_GOSSIP_MENU(9065, pCreature->GetObjectGuid());  // Windsor is free
                    }
                    else
                        pPlayer->SEND_GOSSIP_MENU(9064, pCreature->GetObjectGuid());  // Windsor is DND
                }
                else
                    error_log("SD0: Stormwind City: GossipHello_npc_squire_rowe: Reginald Windsor exists, but can't get npc_reginald_windsorAI. May be problem!");
            }
            else
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let Marshal Windsor know that I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                pPlayer->SEND_GOSSIP_MENU(9065, pCreature->GetObjectGuid());  // Windsor is free
            }
        }
        else
            error_log("SD0: Stormwind City: GossipHello_npc_squire_rowe: Can't get npc_squire_roweAI. Reginald Windsor escort can't continue!");
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_squire_rowe(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (npc_squire_roweAI* pEscortAI = dynamic_cast<npc_squire_roweAI*>(pCreature->AI()))
        {
            pPlayer->CLOSE_GOSSIP_MENU();
            pEscortAI->Start(true, pPlayer, NULL, true);
        }
    }
    return true;
}

/*######
## mob_rift_spawn
#####*/

enum eRiftSpawn
{
	SPELL_ROOT_SELF							= 23973,

    SPELL_CREATE_CONTAINMENT_COFFER         = 9082,
    SPELL_CREATE_FILLED_CONTAINMENT_COFFER  = 9010,
    SPELL_CANTATION_OF_MANIFESTATION        = 9095,
    SPELL_RIFT_SPAWN_INVISIBILITY           = 9093, // Passive

    GO_CHEST_OF_CONTAINMENT_COFFERS         = 122088,
};

struct MANGOS_DLL_DECL mob_rift_spawnAI : public ScriptedAI
{
    mob_rift_spawnAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bCanReset = true;
        Reset();
    }

    uint32 m_uiCreateFilledChestTimer;
    uint32 m_uiMakeInvisibleOOCTimer;
	uint32 m_uiReset;
    
    bool m_bCanReset;
    bool m_bVisible;
    bool m_bFreezed;
    bool m_bDefeated;
	bool m_bCanAttack;

    ObjectGuid m_uiUnfilledChestGUID;
    ObjectGuid m_uiFreezerGUID;

    void Reset()
    {
        if (!m_bCanReset)
            return;

        m_uiCreateFilledChestTimer = 0;
        m_uiMakeInvisibleOOCTimer = 0;
		m_uiReset = 30000;

        m_bVisible = false;
        m_bFreezed = false;
        m_bDefeated = false;
		m_bCanAttack = true;

		m_uiUnfilledChestGUID.Clear();
		m_uiFreezerGUID.Clear();
		m_creature->RemoveAllAuras();

        // Make invisible
        //DoCastSpellIfCan(m_creature, SPELL_RIFT_SPAWN_INVISIBILITY);
        m_creature->CastSpell(m_creature, SPELL_RIFT_SPAWN_INVISIBILITY, true);
    }

    void JustReachedHome()
    {
        if (m_bVisible)
        {
            m_bCanReset = true;
            ResetToHome();
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (m_bVisible)
        {
            if (uiDamage >= m_creature->GetHealth())
            {
                m_bDefeated = true;

                m_creature->SetHealth(2);
                uiDamage = 1;

                FreezeRiftSpawn();
            }
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CANTATION_OF_MANIFESTATION)
        {
            if (!m_bVisible)
            {
                m_bVisible = true;
                m_bCanReset = false;
				m_uiFreezerGUID = pCaster->GetObjectGuid();
                //m_creature->RemoveAurasDueToSpell(SPELL_RIFT_SPAWN_INVISIBILITY);
                m_creature->RemoveAllAuras();
                //AttackStart(pCaster);
            }
        }

        if (pSpell->Id == SPELL_CREATE_CONTAINMENT_COFFER)
        {
            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_CHEST_OF_CONTAINMENT_COFFERS, 1.0f))
            {
                if (m_bDefeated)
                {
                    m_uiUnfilledChestGUID = pGo->GetObjectGuid();
                    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    pGo->SetGoState(GO_STATE_ACTIVE);

                    m_uiCreateFilledChestTimer = 5000;
                }
                else
                {
                    pGo->AddObjectToRemoveList();
                }
            }
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CREATE_FILLED_CONTAINMENT_COFFER)
        {
            if (pTarget->GetObjectGuid() == m_creature->GetObjectGuid())
            {
                if (m_uiUnfilledChestGUID)
                    if (GameObject* pGo = m_creature->GetMap()->GetGameObject(m_uiUnfilledChestGUID))
                        pGo->AddObjectToRemoveList();

                m_bCanReset = true;

                m_creature->SetHealth(0);
                m_creature->SetDeathState(JUST_DIED);
                m_creature->RemoveCorpse();
            }
        }
    }

    void FreezeRiftSpawn()
    {
		if (m_bFreezed)
			return;

        m_bFreezed = true;

		DoCastSpellIfCan(m_creature, SPELL_ROOT_SELF, CAST_AURA_NOT_PRESENT);
		m_bCanAttack = false;
		m_uiReset = 45000;			// make sure they won't get stuck because someone doesn't use q item
		//m_creature->addUnitState(UNIT_STAT_ROOT);		// THESE CAUSED THE MOBS TO CHARGE AWAY INTO THE SUNSET AFTER RESET, not sure why tho
		//m_creature->addUnitState(UNIT_STAT_STUNNED);		// mobs couldn't cast the spell while stunned, bad idea
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (m_uiCreateFilledChestTimer)
		{
            if (m_uiCreateFilledChestTimer <= uiDiff)
            {
                m_creature->CastSpell(m_creature, SPELL_CREATE_FILLED_CONTAINMENT_COFFER, false);
                m_uiCreateFilledChestTimer = 0;
            }
            else
                m_uiCreateFilledChestTimer -= uiDiff;
		}

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
		{
			if (m_bVisible)
			{
				if (m_uiMakeInvisibleOOCTimer <= uiDiff)
				{
					m_bCanReset = true;
					ResetToHome();
				}
				else
					m_uiMakeInvisibleOOCTimer -= uiDiff;
			}
				return;
		}
        
		if (m_uiReset <= uiDiff)			// reset if stuck after root phase
				{
					m_bCanReset = true;
					ResetToHome();
				}
				else
					m_uiReset -= uiDiff;

		if (m_bCanAttack)
			DoMeleeAttackIfReady();
   }
};

CreatureAI* GetAI_mob_rift_spawn(Creature* pCreature)
{
    return new mob_rift_spawnAI(pCreature);
}

/*######
## AddSC
######*/

void AddSC_stormwind_city()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_archmage_malin";
    pNewScript->pGossipHello = &GossipHello_npc_archmage_malin;
    pNewScript->pGossipSelect = &GossipSelect_npc_archmage_malin;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bartleby";
    pNewScript->GetAI = &GetAI_npc_bartleby;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_bartleby;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dashel_stonefist";
    pNewScript->GetAI = &GetAI_npc_dashel_stonefist;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_dashel_stonefist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_katrana_prestor";
    pNewScript->pGossipHello = &GossipHello_npc_lady_katrana_prestor;
    pNewScript->pGossipSelect = &GossipSelect_npc_lady_katrana_prestor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lord_gregor_lescovar";
    pNewScript->GetAI = &GetAI_npc_lord_gregor_lescovar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tyrion_spybot";
    pNewScript->GetAI = &GetAI_npc_tyrion_spybot;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tyrion";
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_tyrion;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_reginald_windsor";
    pNewScript->GetAI = &GetAI_npc_reginald_windsor;
    pNewScript->pGossipHello = &GossipHello_npc_reginald_windsor;
    pNewScript->pGossipSelect = &GossipSelect_npc_reginald_windsor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_reginald_windsor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_squire_rowe";
    pNewScript->GetAI = &GetAI_npc_squire_rowe;
    pNewScript->pGossipHello = &GossipHello_npc_squire_rowe;
    pNewScript->pGossipSelect = &GossipSelect_npc_squire_rowe;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_rift_spawn";
    pNewScript->GetAI = &GetAI_mob_rift_spawn;
    pNewScript->RegisterSelf();

	pNewScript = new Script;
	pNewScript->Name = "npc_marzon_the_silent_pladeAI";
	pNewScript->GetAI = &GetAI_npc_marzon_the_silent_pladeAI;
	pNewScript->RegisterSelf();
}
