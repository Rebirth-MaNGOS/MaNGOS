/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos-zero>
 *
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

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundAV.h"
#include "BattleGroundMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "Language.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"

BattleGroundAV::BattleGroundAV()
{
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_AV_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_AV_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_AV_HAS_BEGUN;
}

BattleGroundAV::~BattleGroundAV()
{
}

void BattleGroundAV::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    BattleGround::HandleKillPlayer(player, killer);
}

void BattleGroundAV::HandleKillUnit(Creature *creature, Player *killer)
{
    DEBUG_LOG("BattleGroundAV: HandleKillUnit %i", creature->GetEntry());
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    uint8 event1 = (sBattleGroundMgr.GetCreatureEventIndex(creature->GetGUIDLow())).event1;
    if (event1 == BG_EVENT_NONE)
        return;
    switch(event1)
    {
    case BG_AV_BOSS_A:
        CastSpellOnTeam(BG_AV_BOSS_KILL_QUEST_SPELL, HORDE);   // this is a spell which finishes a quest where a player has to kill the boss
        RewardReputationToTeam(BG_AV_FACTION_H, m_RepBoss, HORDE);
        RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_BOSS), HORDE);
        SendYellToAll(LANG_BG_AV_A_GENERAL_DEAD, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0));
        EndBattleGround(HORDE, true);
        break;
    case BG_AV_BOSS_H:
        CastSpellOnTeam(BG_AV_BOSS_KILL_QUEST_SPELL, ALLIANCE); // this is a spell which finishes a quest where a player has to kill the boss
        RewardReputationToTeam(BG_AV_FACTION_A, m_RepBoss, ALLIANCE);
        RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_BOSS), ALLIANCE);
        SendYellToAll(LANG_BG_AV_H_GENERAL_DEAD, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0));
        EndBattleGround(ALLIANCE, true);
        break;
    case BG_AV_CAPTAIN_A:
        if (IsActiveEvent(BG_AV_NodeEventCaptainDead_A, 0))
            return;
        RewardReputationToTeam(BG_AV_FACTION_H, m_RepCaptain, HORDE);
        RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_CAPTAIN), HORDE);
        // spawn destroyed aura
        SpawnEvent(BG_AV_NodeEventCaptainDead_A, 0, true);
        break;
    case BG_AV_CAPTAIN_H:
        if (IsActiveEvent(BG_AV_NodeEventCaptainDead_H, 0))
            return;
        RewardReputationToTeam(BG_AV_FACTION_A, m_RepCaptain, ALLIANCE);
        RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_CAPTAIN), ALLIANCE);
        // spawn destroyed aura
        SpawnEvent(BG_AV_NodeEventCaptainDead_H, 0, true);
        break;
    case BG_AV_MINE_BOSSES_NORTH:
        ChangeMineOwner(BG_AV_NORTH_MINE, GetAVTeamIndexByTeamId(killer->GetTeam()));
        break;
    case BG_AV_MINE_BOSSES_SOUTH:
        ChangeMineOwner(BG_AV_SOUTH_MINE, GetAVTeamIndexByTeamId(killer->GetTeam()));
        break;
    }
}

void BattleGroundAV::HandleQuestComplete(uint32 questid, Player *player)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    BattleGroundAVTeamIndex teamIdx = GetAVTeamIndexByTeamId(player->GetTeam());
    MANGOS_ASSERT(teamIdx != BG_AV_TEAM_NEUTRAL);

    uint32 reputation = 0;                                  // reputation for the whole team (other reputation must be done in db)
    // TODO add events (including quest not available anymore, next quest availabe, go/npc de/spawning)
    sLog.outError("BattleGroundAV: Quest %i completed", questid);
    switch(questid)
    {
    case BG_AV_QUEST_A_SCRAPS1:
    case BG_AV_QUEST_A_SCRAPS2:
    case BG_AV_QUEST_H_SCRAPS1:
    case BG_AV_QUEST_H_SCRAPS2:
        m_Team_QuestStatus[teamIdx][0] += 20;
        reputation = 1;
        if( m_Team_QuestStatus[teamIdx][0] == 500 || m_Team_QuestStatus[teamIdx][0] == 1000 || m_Team_QuestStatus[teamIdx][0] == 1500 ) //25,50,75 turn ins
        {
            DEBUG_LOG("BattleGroundAV: Quest %i completed starting with unit upgrading..", questid);
            for (BG_AV_Nodes i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
                if (m_Nodes[i].Owner == teamIdx && m_Nodes[i].State == POINT_CONTROLLED)
                    PopulateNode(i);

            // Upgrade units in the mines.
            for (int mine = 0; mine < 2; mine++)
            {
                if (m_Mine_Owner[mine] == teamIdx)
                {
                    uint32 unit_upgrades = 0;
                    if (m_Team_QuestStatus[teamIdx][0] >= 1500)
                        unit_upgrades = 3;
                    else if (m_Team_QuestStatus[teamIdx][0] >= 1000)
                        unit_upgrades = 2;
                    else if (m_Team_QuestStatus[teamIdx][0] >= 500)
                        unit_upgrades = 1;


                    SpawnEvent(BG_AV_MINE_EVENT + mine, teamIdx + 3 * unit_upgrades, true);
                }
            }
        }
        break;
    case BG_AV_QUEST_A_COMMANDER1:
    case BG_AV_QUEST_H_COMMANDER1:
        m_Team_QuestStatus[teamIdx][1]++;
        reputation = 1;
        if (m_Team_QuestStatus[teamIdx][1] == 120)
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
        break;
    case BG_AV_QUEST_A_COMMANDER2:
    case BG_AV_QUEST_H_COMMANDER2:
        m_Team_QuestStatus[teamIdx][2]++;
        reputation = 2;
        if (m_Team_QuestStatus[teamIdx][2] == 60)
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
        break;
    case BG_AV_QUEST_A_COMMANDER3:
    case BG_AV_QUEST_H_COMMANDER3:
        m_Team_QuestStatus[teamIdx][3]++;
        reputation = 5;
        if (m_Team_QuestStatus[teamIdx][1] == 30)
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
        break;
    case BG_AV_QUEST_A_BOSS1:
    case BG_AV_QUEST_H_BOSS1:
        m_Team_QuestStatus[teamIdx][4] += 4;               // there are 2 quests where you can turn in 5 or 1 item.. ( + 4 cause +1 will be done some lines below)
        reputation = 4;
    case BG_AV_QUEST_A_BOSS2:
    case BG_AV_QUEST_H_BOSS2:
        m_Team_QuestStatus[teamIdx][4]++;
        reputation += 1;
        if (m_Team_QuestStatus[teamIdx][4] >= 200)
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
        break;
    case BG_AV_QUEST_A_NEAR_MINE:
    case BG_AV_QUEST_H_NEAR_MINE:
        m_Team_QuestStatus[teamIdx][5]++;
        reputation = 2;
        if (m_Team_QuestStatus[teamIdx][5] == 28)
        {
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            if (m_Team_QuestStatus[teamIdx][6] == 7)
                DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here - ground assault ready", questid);
        }
        break;
    case BG_AV_QUEST_A_OTHER_MINE:
    case BG_AV_QUEST_H_OTHER_MINE:
        m_Team_QuestStatus[teamIdx][6]++;
        reputation = 3;
        if (m_Team_QuestStatus[teamIdx][6] == 7)
        {
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            if (m_Team_QuestStatus[teamIdx][5] == 20)
                DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here - ground assault ready", questid);
        }
        break;
    case BG_AV_QUEST_A_RIDER_HIDE:
    case BG_AV_QUEST_H_RIDER_HIDE:
        m_Team_QuestStatus[teamIdx][7]++;
        reputation = 1;
        if (m_Team_QuestStatus[teamIdx][7] == 25)
        {
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            if (m_Team_QuestStatus[teamIdx][8] == 25)
                DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here - rider assault ready", questid);
        }
        break;
    case BG_AV_QUEST_A_RIDER_TAME:
    case BG_AV_QUEST_H_RIDER_TAME:
        m_Team_QuestStatus[teamIdx][8]++;
        reputation = 1;
        if (m_Team_QuestStatus[teamIdx][8] == 25)
        {
            DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here", questid);
            if (m_Team_QuestStatus[teamIdx][7] == 25)
                DEBUG_LOG("BattleGroundAV: Quest %i completed (need to implement some events here - rider assault ready", questid);
        }
        break;
    default:
        DEBUG_LOG("BattleGroundAV: Quest %i completed but is not interesting for us", questid);
        return;
        break;
    }
    if (reputation)
        RewardReputationToTeam((player->GetTeam() == ALLIANCE) ? BG_AV_FACTION_A : BG_AV_FACTION_H, reputation, player->GetTeam());
}

void BattleGroundAV::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // add points from mine owning, and look if the neutral team can reclaim the mine
    for(uint8 mine = 0; mine < BG_AV_MAX_MINES; mine++)
    {
        if (m_Mine_Owner[mine] != BG_AV_TEAM_NEUTRAL)
        {
            if (m_Mine_Reclaim_Timer[mine] > diff)
                m_Mine_Reclaim_Timer[mine] -= diff;
            else
                ChangeMineOwner(mine, BG_AV_TEAM_NEUTRAL);
        }
    }

    // looks for all timers of the nodes and destroy the building (for graveyards the building wont get destroyed, it goes just to the other team
    for(BG_AV_Nodes i = BG_AV_NODES_FIRSTAID_STATION; i < BG_AV_NODES_MAX; ++i)
    {
        if (m_Nodes[i].State == POINT_ASSAULTED)
        {
            if (m_Nodes[i].Timer > diff)
                m_Nodes[i].Timer -= diff;
            else
                EventPlayerDestroyedPoint(i);
        }
    }

    // Check if any airstrike has been launched. And spawn the gryphons if that's the case.
    for (int i = 0; i < BG_AV_AIRSTRIKES_MAX; i++)
    {
        if (m_bAirstrikeLaunched[i])
        {
            SpawnEvent(BG_AV_AIRSTRIKES, i, true, false);
            m_bAirstrikeLaunched[i] = false;
        }
    }

    if (!m_vecBeaconEventQueue.empty())
	{
		// Handle queued beacon events.
		auto event_itr = m_vecBeaconEventQueue.begin();
		while (event_itr != m_vecBeaconEventQueue.end())
		{
			HandleBeaconEvent(*event_itr);
			event_itr = m_vecBeaconEventQueue.erase(event_itr);
		}
	}

	if (!m_BeaconTimerMap.empty())
	{
		for (std::pair<uint32, uint32> beacon_timer : m_BeaconTimerMap)
		{
			if (beacon_timer.second)
			{
				if (beacon_timer.second <= diff)
				{	      
					m_BeaconTimerMap[beacon_timer.first] = 0;

					GameObject* beacon = GetSingleGameObjectFromStorage(beacon_timer.first);
					if (beacon)
					{
						beacon->SetRespawnTime(10 * RESPAWN_ONE_DAY);
						beacon->UpdateVisibilityAndView();
		      
						uint32 creature_id;
		    
						switch (beacon_timer.first)
						{
						  case BG_AV_BEACON_MULVERICK:
							  creature_id = BG_AV_BEACON_BIRD_MULVERICK;
							  break;
						  case BG_AV_BEACON_GUSE:
							  creature_id = BG_AV_BEACON_BIRD_GUSE;
							  break;
						  case BG_AV_BEACON_JEZTOR:
							  creature_id = BG_AV_BEACON_BIRD_JEZTOR;
							  break;
						  case BG_AV_BEACON_ICHMAN:
							  creature_id = BG_AV_BEACON_BIRD_ICHMAN;
							  break;
						  case BG_AV_BEACON_VIPORE:
							  creature_id = BG_AV_BEACON_BIRD_VIPORE;
							  break;
						  case BG_AV_BEACON_SLIDORE:
							  creature_id = BG_AV_BEACON_BIRD_SLIDORE;
							  break;
						}
		    
						Creature* summoned_bird = beacon->SummonCreature(creature_id, beacon->GetPositionX(), beacon->GetPositionY(), beacon->GetPositionZ() + 60.f, 0.f, TEMPSUMMON_CORPSE_DESPAWN, 0);

						// Manually call this function to make the bird start patrolling.
						if (summoned_bird && summoned_bird->AI())
							summoned_bird->AI()->JustRespawned();
					}
		
				}
				else
					m_BeaconTimerMap[beacon_timer.first] -= diff;

			}
		}
	}

}

void BattleGroundAV::StartingEventCloseDoors()
{
    DEBUG_LOG("BattleGroundAV: entering state STATUS_WAIT_JOIN ...");
}

void BattleGroundAV::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void BattleGroundAV::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    // create score and add it to map, default values are set in constructor
    BattleGroundAVScore* sc = new BattleGroundAVScore;
    m_PlayerScores[plr->GetObjectGuid()] = sc;

    // Reset the pre-quests for the repeatables in the BG.
    int offset = 0;
    if (plr->getFaction() == ALLIANCE)
        offset = 4;

    for (int i = 0 + offset; i < 4 + offset; i++)
        if (plr->GetQuestRewardStatus(BG_AV_reset_quests[i]) == QUEST_STATUS_COMPLETE)
        {
            plr->SetQuestStatus(BG_AV_reset_quests[i], QUEST_STATUS_NONE);
            plr->getQuestStatusMap()[BG_AV_reset_quests[i]].m_rewarded = false;
        }
}

void BattleGroundAV::EndBattleGround(Team winner, bool killedBoss)
{
    // if they killed the boss hand out extra rep, otherwise skip it
    if(killedBoss)
    {
        // calculate bonuskills for both teams:
        uint32 tower_survived[BG_TEAMS_COUNT]  = {0, 0};
        uint32 graves_owned[BG_TEAMS_COUNT]    = {0, 0};
        uint32 mines_owned[BG_TEAMS_COUNT]     = {0, 0};
        // towers all not destroyed:
        for(BG_AV_Nodes i = BG_AV_NODES_DUNBALDAR_SOUTH; i <= BG_AV_NODES_STONEHEART_BUNKER; ++i)
            if (m_Nodes[i].State == POINT_CONTROLLED)
                if (m_Nodes[i].TotalOwner == BG_AV_TEAM_ALLIANCE)
                    ++tower_survived[BG_TEAM_ALLIANCE];
        for(BG_AV_Nodes i = BG_AV_NODES_ICEBLOOD_TOWER; i <= BG_AV_NODES_FROSTWOLF_WTOWER; ++i)
            if (m_Nodes[i].State == POINT_CONTROLLED)
                if (m_Nodes[i].TotalOwner == BG_AV_TEAM_HORDE)
                    ++tower_survived[BG_TEAM_HORDE];

        // graves all controlled
        for(BG_AV_Nodes i = BG_AV_NODES_FIRSTAID_STATION; i < BG_AV_NODES_MAX; ++i)
            if (m_Nodes[i].State == POINT_CONTROLLED && m_Nodes[i].Owner != BG_AV_TEAM_NEUTRAL)
                ++graves_owned[m_Nodes[i].Owner];

        for (uint32 i = 0; i < BG_AV_MAX_MINES; ++i)
            if (m_Mine_Owner[i] != BG_AV_TEAM_NEUTRAL)
                ++mines_owned[m_Mine_Owner[i]];

        // now we have the values give the honor/reputation to the teams:
        Team team[BG_TEAMS_COUNT]      = { ALLIANCE, HORDE };
        uint32 faction[BG_TEAMS_COUNT]   = { BG_AV_FACTION_A, BG_AV_FACTION_H };
        for (uint32 i = 0; i < BG_TEAMS_COUNT; i++)
        {
            if (tower_survived[i])
            {
                RewardReputationToTeam(faction[i], tower_survived[i] * m_RepSurviveTower, team[i]);
                RewardHonorToTeam(GetBonusHonorFromKill(tower_survived[i] * BG_AV_KILL_SURVIVING_TOWER), team[i]);
            }
            DEBUG_LOG("BattleGroundAV: EndbattleGround: bgteam: %u towers:%u honor:%u rep:%u", i, tower_survived[i], GetBonusHonorFromKill(tower_survived[i] * BG_AV_KILL_SURVIVING_TOWER), tower_survived[i] * BG_AV_REP_SURVIVING_TOWER);
            if (graves_owned[i])
                RewardReputationToTeam(faction[i], graves_owned[i] * m_RepOwnedGrave, team[i]);
            if (mines_owned[i])
                RewardReputationToTeam(faction[i], mines_owned[i] * m_RepOwnedMine, team[i]);
            // captain survived?:
            if (!IsActiveEvent(BG_AV_NodeEventCaptainDead_A + GetTeamIndexByTeamId(team[i]), 0))
            {
                RewardReputationToTeam(faction[i], m_RepSurviveCaptain, team[i]);
                RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_SURVIVING_CAPTAIN), team[i]);
            }
        }

        // both teams:
        if (m_HonorMapComplete)
        {
            RewardHonorToTeam(m_HonorMapComplete, ALLIANCE);
            RewardHonorToTeam(m_HonorMapComplete, HORDE);
        }
    }
    BattleGround::EndBattleGround(winner);
}

void BattleGroundAV::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
}

void BattleGroundAV::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    switch(Trigger)
    {
    case 95:
    case 2608:
        if (Source->GetTeam() != ALLIANCE)
            Source->GetSession()->SendNotification(LANG_BATTLEGROUND_ONLY_ALLIANCE_USE);
        else
            Source->LeaveBattleground();
        break;
    case 2606:
        if (Source->GetTeam() != HORDE)
            Source->GetSession()->SendNotification(LANG_BATTLEGROUND_ONLY_HORDE_USE);
        else
            Source->LeaveBattleground();
        break;
    case 3326:
    case 3327:
    case 3328:
    case 3329:
    case 3330:
    case 3331:
        //Source->Unmount();
        break;
    default:
        DEBUG_LOG("BattleGroundAV: WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
//            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
        break;
    }
}

void BattleGroundAV::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{

    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetObjectGuid());
    if(itr == m_PlayerScores.end())                         // player not found...
        return;

    switch(type)
    {
    case SCORE_GRAVEYARDS_ASSAULTED:
        ((BattleGroundAVScore*)itr->second)->GraveyardsAssaulted += value;
        break;
    case SCORE_GRAVEYARDS_DEFENDED:
        ((BattleGroundAVScore*)itr->second)->GraveyardsDefended += value;
        break;
    case SCORE_TOWERS_ASSAULTED:
        ((BattleGroundAVScore*)itr->second)->TowersAssaulted += value;
        break;
    case SCORE_TOWERS_DEFENDED:
        ((BattleGroundAVScore*)itr->second)->TowersDefended += value;
        break;
    case SCORE_SECONDARY_OBJECTIVES:
        ((BattleGroundAVScore*)itr->second)->SecondaryObjectives += value;
        break;
    default:
        BattleGround::UpdatePlayerScore(Source, type, value);
        break;
    }
}

void BattleGroundAV::EventPlayerDestroyedPoint(BG_AV_Nodes node)
{
    DEBUG_LOG("BattleGroundAV: player destroyed point node %i", node);

    MANGOS_ASSERT(m_Nodes[node].Owner != BG_AV_TEAM_NEUTRAL)
    BattleGroundTeamIndex ownerTeamIdx = BattleGroundTeamIndex(m_Nodes[node].Owner);
    Team ownerTeam = ownerTeamIdx == BG_TEAM_ALLIANCE ? ALLIANCE : HORDE;

    // despawn banner
    DestroyNode(node);
    PopulateNode(node);
    UpdateNodeWorldState(node);

    if (IsTower(node))
    {
        uint8 tmp = node - BG_AV_NODES_DUNBALDAR_SOUTH;
        // despawn marshal (one of those guys protecting the boss)
        SpawnEvent(BG_AV_MARSHAL_A_SOUTH + tmp, 0, false);
        // spawn enemy marshal
        SpawnEvent(BG_AV_MARSHAL_A_SOUTH + tmp, 1, true);

        RewardReputationToTeam((ownerTeam == ALLIANCE) ? BG_AV_FACTION_A : BG_AV_FACTION_H, m_RepTowerDestruction, ownerTeam);
        RewardHonorToTeam(GetBonusHonorFromKill(BG_AV_KILL_TOWER), ownerTeam);
        SendYell2ToAll(LANG_BG_AV_TOWER_TAKEN, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0), GetNodeName(node), (ownerTeam == ALLIANCE) ? LANG_BG_ALLY : LANG_BG_HORDE);
    }
    else
    {
        SendYell2ToAll(LANG_BG_AV_GRAVE_TAKEN, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0), GetNodeName(node), (ownerTeam == ALLIANCE) ? LANG_BG_ALLY : LANG_BG_HORDE);
    }
}

void BattleGroundAV::ChangeMineOwner(uint8 mine, BattleGroundAVTeamIndex teamIdx)
{
    // TODO implement quest 7122
    // mine=0 northmine, mine=1 southmine
    // TODO changing the owner should result in setting respawntime to infinite for current creatures (they should fight the new ones), spawning new mine owners creatures and changing the chest - objects so that the current owning team can use them
    MANGOS_ASSERT(mine == BG_AV_NORTH_MINE || mine == BG_AV_SOUTH_MINE);
    if (m_Mine_Owner[mine] == teamIdx)
        return;

    m_Mine_PrevOwner[mine] = m_Mine_Owner[mine];
    m_Mine_Owner[mine] = teamIdx;

	// Update the supplies in the mines.
	for (GameObject* current_supply : m_mineSupplies[mine])
	{
		current_supply->SetLootState(LootState::GO_READY);
		current_supply->SetGoState(GOState::GO_STATE_READY);
	}

	for (auto plr_itr = GetPlayers().begin(); plr_itr != GetPlayers().end(); ++plr_itr)
	{
		Player* player = GetBgMap()->GetPlayer(plr_itr->first);

		if (player)
			player->UpdateForQuestWorldObjects();
	}


    uint32 unit_upgrades = 0;
    if (teamIdx != BG_AV_TEAM_NEUTRAL)
    {
        if (m_Team_QuestStatus[teamIdx][0] >= 1500)
            unit_upgrades = 3;
        else if (m_Team_QuestStatus[teamIdx][0] >= 1000)
            unit_upgrades = 2;
        else if (m_Team_QuestStatus[teamIdx][0] >= 500)
            unit_upgrades = 1;
    }

    SendMineWorldStates(mine);

    SpawnEvent(BG_AV_MINE_EVENT + mine, teamIdx + 3 * unit_upgrades, true, false);
    SpawnEvent(BG_AV_MINE_BOSSES + mine, teamIdx, true);

    if (teamIdx != BG_AV_TEAM_NEUTRAL)
    {
        PlaySoundToAll((teamIdx == BG_AV_TEAM_ALLIANCE) ? BG_AV_SOUND_ALLIANCE_GOOD : BG_AV_SOUND_HORDE_GOOD);
        m_Mine_Reclaim_Timer[mine] = BG_AV_MINE_RECLAIM_TIMER;
        SendYell2ToAll(LANG_BG_AV_MINE_TAKEN , LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0),
                       (teamIdx == BG_AV_TEAM_ALLIANCE ) ? LANG_BG_ALLY : LANG_BG_HORDE,
                       (mine == BG_AV_NORTH_MINE) ? LANG_BG_AV_MINE_NORTH : LANG_BG_AV_MINE_SOUTH);
    }
}

bool BattleGroundAV::PlayerCanDoMineQuest(int32 GOId, Team team)
{
    if (GOId == BG_AV_OBJECTID_MINE_N)
        return (m_Mine_Owner[BG_AV_NORTH_MINE] == GetAVTeamIndexByTeamId(team));
    if (GOId == BG_AV_OBJECTID_MINE_S)
        return (m_Mine_Owner[BG_AV_SOUTH_MINE] == GetAVTeamIndexByTeamId(team));
    return true;                                            // cause it's no mine'object it is ok if this is true
}

/// will spawn and despawn creatures around a node
/// more a wrapper around spawnevent cause graveyards are special
void BattleGroundAV::PopulateNode(BG_AV_Nodes node)
{
    BattleGroundAVTeamIndex teamIdx = m_Nodes[node].Owner;
    if (IsGrave(node) && teamIdx != BG_AV_TEAM_NEUTRAL)
    {
        uint32 graveDefenderType;
        if (m_Team_QuestStatus[teamIdx][0] < 500 )
            graveDefenderType = 0;
        else if (m_Team_QuestStatus[teamIdx][0] < 1000 )
            graveDefenderType = 1;
        else if (m_Team_QuestStatus[teamIdx][0] < 1500 )
            graveDefenderType = 2;
        else
            graveDefenderType = 3;

        if (m_Nodes[node].State == POINT_CONTROLLED) // we can spawn the current owner event
            SpawnEvent(BG_AV_NODES_MAX + node, teamIdx * BG_AV_MAX_GRAVETYPES + graveDefenderType, true);
        else // we despawn the event from the prevowner
            SpawnEvent(BG_AV_NODES_MAX + node, m_Nodes[node].PrevOwner * BG_AV_MAX_GRAVETYPES + graveDefenderType, false);
    }
    SpawnEvent(node, (teamIdx * BG_AV_MAX_STATES) + m_Nodes[node].State, true);
}

/// called when using a banner
void BattleGroundAV::EventPlayerClickedOnFlag(Player *source, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
    DEBUG_LOG("BattleGroundAV: using gameobject %i", target_obj->GetEntry());
    uint8 event = (sBattleGroundMgr.GetGameObjectEventIndex(target_obj->GetGUIDLow())).event1;
    if (event >= BG_AV_NODES_MAX)                           // not a node
        return;
    BG_AV_Nodes node = BG_AV_Nodes(event);
    switch ((sBattleGroundMgr.GetGameObjectEventIndex(target_obj->GetGUIDLow())).event2 % BG_AV_MAX_STATES)
    {
    case POINT_CONTROLLED:
        EventPlayerAssaultsPoint(source, node);
        break;
    case POINT_ASSAULTED:
        EventPlayerDefendsPoint(source, node);
        break;
    default:
        break;
    }
}

void BattleGroundAV::EventPlayerDefendsPoint(Player* player, BG_AV_Nodes node)
{
    MANGOS_ASSERT(GetStatus() == STATUS_IN_PROGRESS);

    BattleGroundTeamIndex teamIdx = GetTeamIndexByTeamId(player->GetTeam());

    if (m_Nodes[node].Owner == BattleGroundAVTeamIndex(teamIdx) || m_Nodes[node].State != POINT_ASSAULTED)
        return;
    if( m_Nodes[node].TotalOwner == BG_AV_TEAM_NEUTRAL )    // initial snowfall capture
    {
        // until snowfall doesn't belong to anyone it is better handled in assault - code (best would be to have a special function
        // for neutral nodes.. but doing this just for snowfall will be a bit to much i think
        MANGOS_ASSERT(node == BG_AV_NODES_SNOWFALL_GRAVE);  // currently the only neutral grave
        EventPlayerAssaultsPoint(player, node);
        return;
    }

    DEBUG_LOG("BattleGroundAV: player defends node: %i", node);
    if (m_Nodes[node].PrevOwner != BattleGroundAVTeamIndex(teamIdx))
    {
        sLog.outError("BattleGroundAV: player defends point which doesn't belong to his team %i", node);
        return;
    }

    DefendNode(node, teamIdx);                              // set the right variables for nodeinfo
    PopulateNode(node);                                     // spawn node-creatures (defender for example)
    UpdateNodeWorldState(node);                             // send new mapicon to the player

    if (IsTower(node))
    {
        SendYell2ToAll( LANG_BG_AV_TOWER_DEFENDED, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0),
                        GetNodeName(node),
                        ( teamIdx == BG_TEAM_ALLIANCE ) ? LANG_BG_ALLY:LANG_BG_HORDE);
        UpdatePlayerScore(player, SCORE_TOWERS_DEFENDED, 1);
        PlaySoundToAll(BG_AV_SOUND_BOTH_TOWER_DEFEND);
    }
    else
    {
        SendYell2ToAll(LANG_BG_AV_GRAVE_DEFENDED, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0),
                       GetNodeName(node),
                       ( teamIdx == BG_TEAM_ALLIANCE ) ? LANG_BG_ALLY:LANG_BG_HORDE);
        UpdatePlayerScore(player, SCORE_GRAVEYARDS_DEFENDED, 1);
        // update the statistic for the defending player
        PlaySoundToAll((teamIdx == BG_TEAM_ALLIANCE)?BG_AV_SOUND_ALLIANCE_GOOD:BG_AV_SOUND_HORDE_GOOD);
    }
}

void BattleGroundAV::EventPlayerAssaultsPoint(Player* player, BG_AV_Nodes node)
{
    // TODO implement quest 7101, 7081
    BattleGroundTeamIndex teamIdx  = GetTeamIndexByTeamId(player->GetTeam());
    DEBUG_LOG("BattleGroundAV: player assaults node %i", node);
    if (m_Nodes[node].Owner == BattleGroundAVTeamIndex(teamIdx) || BattleGroundAVTeamIndex(teamIdx) == m_Nodes[node].TotalOwner)
        return;

    AssaultNode(node, teamIdx);                                // update nodeinfo variables
    UpdateNodeWorldState(node);                             // send mapicon
    PopulateNode(node);

    if (IsTower(node))
    {
        SendYell2ToAll(LANG_BG_AV_TOWER_ASSAULTED, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0),
                       GetNodeName(node),
                       ( teamIdx == BG_TEAM_ALLIANCE ) ? LANG_BG_ALLY:LANG_BG_HORDE);
        UpdatePlayerScore(player, SCORE_TOWERS_ASSAULTED, 1);
    }
    else
    {
        SendYell2ToAll(LANG_BG_AV_GRAVE_ASSAULTED, LANG_UNIVERSAL, GetSingleCreatureGuid(BG_AV_HERALD, 0),
                       GetNodeName(node),
                       ( teamIdx == BG_TEAM_ALLIANCE ) ? LANG_BG_ALLY:LANG_BG_HORDE);
        // update the statistic for the assaulting player
        UpdatePlayerScore(player, SCORE_GRAVEYARDS_ASSAULTED, 1);
    }

    PlaySoundToAll((teamIdx == BG_TEAM_ALLIANCE) ? BG_AV_SOUND_ALLIANCE_ASSAULTS : BG_AV_SOUND_HORDE_ASSAULTS);
}

void BattleGroundAV::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    bool stateok;
    for (uint32 i = BG_AV_NODES_FIRSTAID_STATION; i < BG_AV_NODES_MAX; ++i)
    {
        for (uint8 j = 0; j < BG_AV_MAX_STATES; j++)
        {
            stateok = (m_Nodes[i].State == j);
            FillInitialWorldState(data, count, BG_AV_NodeWorldStates[i][GetWorldStateType(j, BG_AV_TEAM_ALLIANCE)],
                                  m_Nodes[i].Owner == BG_AV_TEAM_ALLIANCE && stateok);
            FillInitialWorldState(data, count, BG_AV_NodeWorldStates[i][GetWorldStateType(j, BG_AV_TEAM_HORDE)],
                                  m_Nodes[i].Owner == BG_AV_TEAM_HORDE && stateok);
        }
    }

    if( m_Nodes[BG_AV_NODES_SNOWFALL_GRAVE].Owner == BG_AV_TEAM_NEUTRAL )   // cause neutral teams aren't handled generic
        FillInitialWorldState(data, count, AV_SNOWFALL_N, 1);

    if( GetStatus() == STATUS_IN_PROGRESS )                 // only if game is running the teamscores are displayed
    {
        FillInitialWorldState(data, count, BG_AV_SHOW_A_SCORE, 1);
        FillInitialWorldState(data, count, BG_AV_SHOW_H_SCORE, 1);
    }
    else
    {
        FillInitialWorldState(data, count, BG_AV_SHOW_A_SCORE, 0);
        FillInitialWorldState(data, count, BG_AV_SHOW_H_SCORE, 0);
    }

    FillInitialWorldState(data, count, BG_AV_MineWorldStates[BG_AV_NORTH_MINE][m_Mine_Owner[BG_AV_NORTH_MINE]], 1);
    if (m_Mine_Owner[BG_AV_NORTH_MINE] != m_Mine_PrevOwner[BG_AV_NORTH_MINE])
        FillInitialWorldState(data, count, BG_AV_MineWorldStates[BG_AV_NORTH_MINE][m_Mine_PrevOwner[BG_AV_NORTH_MINE]], 0);

    FillInitialWorldState(data, count, BG_AV_MineWorldStates[BG_AV_SOUTH_MINE][m_Mine_Owner[BG_AV_SOUTH_MINE]], 1);
    if (m_Mine_Owner[BG_AV_SOUTH_MINE] != m_Mine_PrevOwner[BG_AV_SOUTH_MINE])
        FillInitialWorldState(data, count, BG_AV_MineWorldStates[BG_AV_SOUTH_MINE][m_Mine_PrevOwner[BG_AV_SOUTH_MINE]], 0);

}

void BattleGroundAV::UpdateNodeWorldState(BG_AV_Nodes node)
{
    UpdateWorldState(BG_AV_NodeWorldStates[node][GetWorldStateType(m_Nodes[node].State,m_Nodes[node].Owner)], 1);
    if( m_Nodes[node].PrevOwner == BG_AV_TEAM_NEUTRAL )     // currently only snowfall is supported as neutral node
        UpdateWorldState(AV_SNOWFALL_N, 0);
    else
        UpdateWorldState(BG_AV_NodeWorldStates[node][GetWorldStateType(m_Nodes[node].PrevState,m_Nodes[node].PrevOwner)], 0);
}

void BattleGroundAV::SendMineWorldStates(uint32 mine)
{
    MANGOS_ASSERT(mine == BG_AV_NORTH_MINE || mine == BG_AV_SOUTH_MINE);

    UpdateWorldState(BG_AV_MineWorldStates[mine][m_Mine_Owner[mine]], 1);
    if (m_Mine_Owner[mine] != m_Mine_PrevOwner[mine])
        UpdateWorldState(BG_AV_MineWorldStates[mine][m_Mine_PrevOwner[mine]], 0);
}

WorldSafeLocsEntry const* BattleGroundAV::GetClosestGraveYard(Player *plr)
{
    float x = plr->GetPositionX();
    float y = plr->GetPositionY();
    BattleGroundAVTeamIndex teamIdx = GetAVTeamIndexByTeamId(plr->GetTeam());
    WorldSafeLocsEntry const* good_entry = NULL;
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // Is there any occupied node for this team?
        float mindist = 9999999.0f;
        for(uint8 i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)
        {
            if (m_Nodes[i].Owner != teamIdx || m_Nodes[i].State != POINT_CONTROLLED)
                continue;
            WorldSafeLocsEntry const * entry = sWorldSafeLocsStore.LookupEntry( BG_AV_GraveyardIds[i] );
            if (!entry)
                continue;
            float dist = (entry->x - x) * (entry->x - x) + (entry->y - y) * (entry->y - y);
            if (mindist > dist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }
    }
    // If not, place ghost in the starting-cave
    if (!good_entry)
        good_entry = sWorldSafeLocsStore.LookupEntry( BG_AV_GraveyardIds[teamIdx + 7] );

    return good_entry;
}

uint32 BattleGroundAV::GetNodeName(BG_AV_Nodes node)
{
    switch (node)
    {
    case BG_AV_NODES_FIRSTAID_STATION:
        return LANG_BG_AV_NODE_GRAVE_STORM_AID;
    case BG_AV_NODES_DUNBALDAR_SOUTH:
        return LANG_BG_AV_NODE_TOWER_DUN_S;
    case BG_AV_NODES_DUNBALDAR_NORTH:
        return LANG_BG_AV_NODE_TOWER_DUN_N;
    case BG_AV_NODES_STORMPIKE_GRAVE:
        return LANG_BG_AV_NODE_GRAVE_STORMPIKE;
    case BG_AV_NODES_ICEWING_BUNKER:
        return LANG_BG_AV_NODE_TOWER_ICEWING;
    case BG_AV_NODES_STONEHEART_GRAVE:
        return LANG_BG_AV_NODE_GRAVE_STONE;
    case BG_AV_NODES_STONEHEART_BUNKER:
        return LANG_BG_AV_NODE_TOWER_STONE;
    case BG_AV_NODES_SNOWFALL_GRAVE:
        return LANG_BG_AV_NODE_GRAVE_SNOW;
    case BG_AV_NODES_ICEBLOOD_TOWER:
        return LANG_BG_AV_NODE_TOWER_ICE;
    case BG_AV_NODES_ICEBLOOD_GRAVE:
        return LANG_BG_AV_NODE_GRAVE_ICE;
    case BG_AV_NODES_TOWER_POINT:
        return LANG_BG_AV_NODE_TOWER_POINT;
    case BG_AV_NODES_FROSTWOLF_GRAVE:
        return LANG_BG_AV_NODE_GRAVE_FROST;
    case BG_AV_NODES_FROSTWOLF_ETOWER:
        return LANG_BG_AV_NODE_TOWER_FROST_E;
    case BG_AV_NODES_FROSTWOLF_WTOWER:
        return LANG_BG_AV_NODE_TOWER_FROST_W;
    case BG_AV_NODES_FROSTWOLF_HUT:
        return LANG_BG_AV_NODE_GRAVE_FROST_HUT;
    default:
        return 0;
        break;
    }
}

void BattleGroundAV::AssaultNode(BG_AV_Nodes node, BattleGroundTeamIndex teamIdx)
{
    MANGOS_ASSERT(m_Nodes[node].TotalOwner != BattleGroundAVTeamIndex(teamIdx));
    MANGOS_ASSERT(m_Nodes[node].Owner != BattleGroundAVTeamIndex(teamIdx));
    // only assault an assaulted node if no totalowner exists:
    MANGOS_ASSERT(m_Nodes[node].State != POINT_ASSAULTED || m_Nodes[node].TotalOwner == BG_AV_TEAM_NEUTRAL);
    // the timer gets another time, if the previous owner was 0 == Neutral
    m_Nodes[node].Timer      = (m_Nodes[node].PrevOwner != BG_AV_TEAM_NEUTRAL) ? BG_AV_CAPTIME : BG_AV_SNOWFALL_FIRSTCAP;
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].Owner      = BattleGroundAVTeamIndex(teamIdx);
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_ASSAULTED;
}

void BattleGroundAV::DestroyNode(BG_AV_Nodes node)
{
    MANGOS_ASSERT(m_Nodes[node].State == POINT_ASSAULTED);

    m_Nodes[node].TotalOwner = m_Nodes[node].Owner;
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
}

void BattleGroundAV::InitNode(BG_AV_Nodes node, BattleGroundAVTeamIndex teamIdx, bool tower)
{
    m_Nodes[node].TotalOwner = teamIdx;
    m_Nodes[node].Owner      = teamIdx;
    m_Nodes[node].PrevOwner  = teamIdx;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
    m_Nodes[node].Tower      = tower;
    m_ActiveEvents[node] = teamIdx * BG_AV_MAX_STATES + m_Nodes[node].State;
    if (IsGrave(node))                                      // grave-creatures are special cause of a quest
        m_ActiveEvents[node + BG_AV_NODES_MAX]  = teamIdx * BG_AV_MAX_GRAVETYPES;
}

void BattleGroundAV::DefendNode(BG_AV_Nodes node, BattleGroundTeamIndex teamIdx)
{
    MANGOS_ASSERT(m_Nodes[node].TotalOwner == BattleGroundAVTeamIndex(teamIdx));
    MANGOS_ASSERT(m_Nodes[node].Owner != BattleGroundAVTeamIndex(teamIdx));
    MANGOS_ASSERT(m_Nodes[node].State != POINT_CONTROLLED);
    m_Nodes[node].PrevOwner  = m_Nodes[node].Owner;
    m_Nodes[node].Owner      = BattleGroundAVTeamIndex(teamIdx);
    m_Nodes[node].PrevState  = m_Nodes[node].State;
    m_Nodes[node].State      = POINT_CONTROLLED;
    m_Nodes[node].Timer      = 0;
}

void BattleGroundAV::Reset()
{
    BattleGround::Reset();
    // set the reputation and honor variables:
    bool isBGWeekend = BattleGroundMgr::IsBGWeekend(GetTypeID());

    m_HonorMapComplete    = (isBGWeekend) ? BG_AV_KILL_MAP_COMPLETE_HOLIDAY : BG_AV_KILL_MAP_COMPLETE;
    m_RepTowerDestruction = (isBGWeekend) ? BG_AV_REP_TOWER_HOLIDAY         : BG_AV_REP_TOWER;
    m_RepCaptain          = (isBGWeekend) ? BG_AV_REP_CAPTAIN_HOLIDAY       : BG_AV_REP_CAPTAIN;
    m_RepBoss             = (isBGWeekend) ? BG_AV_REP_BOSS_HOLIDAY          : BG_AV_REP_BOSS;
    m_RepOwnedGrave       = (isBGWeekend) ? BG_AV_REP_OWNED_GRAVE_HOLIDAY   : BG_AV_REP_OWNED_GRAVE;
    m_RepSurviveCaptain   = (isBGWeekend) ? BG_AV_REP_SURVIVING_CAPTAIN_HOLIDAY : BG_AV_REP_SURVIVING_CAPTAIN;
    m_RepSurviveTower     = (isBGWeekend) ? BG_AV_REP_SURVIVING_TOWER_HOLIDAY : BG_AV_REP_SURVIVING_TOWER;
    m_RepOwnedMine        = (isBGWeekend) ? BG_AV_REP_OWNED_MINE_HOLIDAY    : BG_AV_REP_OWNED_MINE;

    for(uint8 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        for(uint8 j = 0; j < 9; j++)                        // 9 quests getting tracked
            m_Team_QuestStatus[i][j] = 0;
        m_ActiveEvents[BG_AV_NodeEventCaptainDead_A + i] = BG_EVENT_NONE;
    }

    for(uint8 i = 0; i < BG_AV_MAX_MINES; i++)
    {
        m_Mine_Owner[i] = BG_AV_TEAM_NEUTRAL;
        m_Mine_PrevOwner[i] = m_Mine_Owner[i];
        m_ActiveEvents[BG_AV_MINE_BOSSES+ i] = BG_AV_TEAM_NEUTRAL;
        m_ActiveEvents[BG_AV_MINE_EVENT + i] = BG_AV_TEAM_NEUTRAL;
    }

    m_ActiveEvents[BG_AV_CAPTAIN_A] = 0;
    m_ActiveEvents[BG_AV_CAPTAIN_H] = 0;
    m_ActiveEvents[BG_AV_HERALD] = 0;
    m_ActiveEvents[BG_AV_BOSS_A] = 0;
    m_ActiveEvents[BG_AV_BOSS_H] = 0;
    for(BG_AV_Nodes i = BG_AV_NODES_DUNBALDAR_SOUTH; i <= BG_AV_NODES_FROSTWOLF_WTOWER; ++i)   // towers
        m_ActiveEvents[BG_AV_MARSHAL_A_SOUTH + i - BG_AV_NODES_DUNBALDAR_SOUTH] = 0;

    for(BG_AV_Nodes i = BG_AV_NODES_FIRSTAID_STATION; i <= BG_AV_NODES_STONEHEART_GRAVE; ++i)   // alliance graves
        InitNode(i, BG_AV_TEAM_ALLIANCE, false);
    for(BG_AV_Nodes i = BG_AV_NODES_DUNBALDAR_SOUTH; i <= BG_AV_NODES_STONEHEART_BUNKER; ++i)   // alliance towers
        InitNode(i, BG_AV_TEAM_ALLIANCE, true);

    for(BG_AV_Nodes i = BG_AV_NODES_ICEBLOOD_GRAVE; i <= BG_AV_NODES_FROSTWOLF_HUT; ++i)        // horde graves
        InitNode(i, BG_AV_TEAM_HORDE, false);
    for(BG_AV_Nodes i = BG_AV_NODES_ICEBLOOD_TOWER; i <= BG_AV_NODES_FROSTWOLF_WTOWER; ++i)     // horde towers
        InitNode(i, BG_AV_TEAM_HORDE, true);

    InitNode(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_TEAM_NEUTRAL, false);                            // give snowfall neutral owner

	// Set all airstrikes to not launched.
	for (int i = 0; i < BG_AV_AIRSTRIKES_MAX; i++)
		m_bAirstrikeLaunched[i] = false;

	// Set the last airstrike to one that doesn't exist.
	SpawnEvent(BG_AV_AIRSTRIKES, 6, true);

}

void BattleGroundAV::GetInsigniaLoot(Loot *loot, Player *victim, Player* /*looter*/)
{
    //Always give armor scraps
    loot->AddItem(LootStoreItem(17422,0,0,0,4,16));

    //Always give race item
    switch (victim->getRace())
    {
    case RACE_HUMAN:    //Human bone chip
        loot->AddItem(LootStoreItem(18144,0,0,0,1,1));
        break;
    case RACE_GNOME:    //Tuft of gnome hair
        loot->AddItem(LootStoreItem(18143,0,0,0,1,1));
        break;
    case RACE_DWARF:    //Dwarf Spine
        loot->AddItem(LootStoreItem(18206,0,0,0,1,1));
        break;
    case RACE_NIGHTELF: //Severed Night Elf Head
        loot->AddItem(LootStoreItem(18142,0,0,0,1,1));
        break;
    case RACE_ORC:      //Orc Tooth
        loot->AddItem(LootStoreItem(18207,0,0,0,1,1));
        break;
    case RACE_TAUREN:   //Tauren Hoof
        loot->AddItem(LootStoreItem(18145,0,0,0,1,1));
        break;
    case RACE_TROLL:    //Darkspear Troll Mojo
        loot->AddItem(LootStoreItem(18146,0,0,0,1,1));
        break;
    case RACE_UNDEAD:    //Forsaken Heart
        loot->AddItem(LootStoreItem(18147,0,0,0,1,1));
        break;
    }

	// Flavor Loot Chances
	if (roll_chance_f(0.05f))
	{
		if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
			loot->AddItem(LootStoreItem(18227,0,0,0,1,1));
		else
			loot->AddItem(LootStoreItem(18234,0,0,0,1,1));
	}

	if (roll_chance_f(0.05f))
	{
		if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
			loot->AddItem(LootStoreItem(18231,0,0,0,1,1));
		else
			loot->AddItem(LootStoreItem(18234,0,0,0,1,1));
	}

	if (roll_chance_f(0.1f))
	{
		if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
			loot->AddItem(LootStoreItem(18225,0,0,0,1,1));
		else
			loot->AddItem(LootStoreItem(18233,0,0,0,1,1));
	}

	if (roll_chance_f(0.025f))
	{
		loot->AddItem(LootStoreItem(18230,0,0,0,1,1));
	}

	if (roll_chance_f(0.025f))
	{
		loot->AddItem(LootStoreItem(18229,0,0,0,1,1));
	}

	if (roll_chance_f(0.005f))
	{
		loot->AddItem(LootStoreItem(18228,0,0,0,1,1));
	}

	if (roll_chance_f(0.001f))
	{
		loot->AddItem(LootStoreItem(18226,0,0,0,1,1));
	}
	


    //50% chance of Storm Crystal/Stormpike Blood
    if (roll_chance_f(50))
    {
        if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
            loot->AddItem(LootStoreItem(17306,0,0,0,1,1));
        else
            loot->AddItem(LootStoreItem(17423,0,0,0,1,1));
    }

	//50% chance of a medal dropping - medal type is based on rank
    if (roll_chance_f(50))
    {
        int8 rank = victim->GetHonorRankInfo().visualRank;
        if (rank >= 16)
        {
            //Marshal/Warlord or higher
            if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
                loot->AddItem(LootStoreItem(17328,0,0,0,1,1));
            else
                loot->AddItem(LootStoreItem(17504,0,0,0,1,1));
        } else if ( rank >= 10)
        {
            //Officer
            if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
                loot->AddItem(LootStoreItem(17327,0,0,0,1,1));
            else
                loot->AddItem(LootStoreItem(17503,0,0,0,1,1));
        } else
        {
            //Enlisted
            if (victim->getRace() == RACE_HUMAN || victim->getRace() == RACE_GNOME || victim->getRace() == RACE_DWARF || victim->getRace() ==  RACE_NIGHTELF)
                loot->AddItem(LootStoreItem(17326,0,0,0,1,1));
            else
                loot->AddItem(LootStoreItem(17502,0,0,0,1,1));
        }
    }
}

void BattleGroundAV::OnObjectDBLoad(GameObject* object)
{
    BattleGround::OnObjectDBLoad(object);

    switch(object->GetEntry())
    {
    case BG_AV_BEACON_GUSE:
    case BG_AV_BEACON_ICHMAN:
    case BG_AV_BEACON_SLIDORE:
    case BG_AV_BEACON_VIPORE:
    case BG_AV_BEACON_MULVERICK:
    case BG_AV_BEACON_JEZTOR:
        object->SetGoState(GO_STATE_ACTIVE);
        object->SetRespawnTime(10 * RESPAWN_ONE_DAY);
        break;
	case BG_AV_OBJECTID_MINE_N:
		m_mineSupplies[BG_AV_NORTH_MINE].push_back(object);
		return;
	case BG_AV_OBJECTID_MINE_S:
		m_mineSupplies[BG_AV_SOUTH_MINE].push_back(object);
		return;
    default:
        return;
    }
    m_GameObjectStorageMap[object->GetEntry()] = object->GetGUID();
}

GameObject* BattleGroundAV::GetSingleGameObjectFromStorage(uint32 entry)
{
    ObjectGuid current_guid = m_GameObjectStorageMap[entry];

    if (current_guid)
        return GetBgMap()->GetGameObject(current_guid);
    else
        return NULL;

}


void BattleGroundAV::HandleBeaconEvent(uint32 event_id)
{
    uint32 beacon_id;

    switch(event_id)
    {
    case BG_AV_EVENT_SPAWN_GUSE_BEACON:
        beacon_id = BG_AV_BEACON_GUSE;
        break;
    case BG_AV_EVENT_SPAWN_ICHMAN_BEACON:
        beacon_id = BG_AV_BEACON_ICHMAN;
        break;
    case BG_AV_EVENT_SPAWN_VIPORE_BEACON:
        beacon_id = BG_AV_BEACON_VIPORE;
        break;
    case BG_AV_EVENT_SPAWN_SLIDORE_BEACON:
        beacon_id = BG_AV_BEACON_SLIDORE;
        break;
    case BG_AV_EVENT_SPAWN_JEZTOR_BEACON:
        beacon_id = BG_AV_BEACON_JEZTOR;
        break;
    case BG_AV_EVENT_SPAWN_MULVERICK_BEACON:
        beacon_id = BG_AV_BEACON_MULVERICK;
        break;
    default:
        return;
    }

    m_BeaconTimerMap[beacon_id] = 60000;

    GameObject* beacon = GetSingleGameObjectFromStorage(beacon_id);
    if (beacon)
	{
        beacon->Respawn();
		beacon->SetGoState(GOState::GO_STATE_READY);
		beacon->SetLootState(LootState::GO_READY);
	}
}

