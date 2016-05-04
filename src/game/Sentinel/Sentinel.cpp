#include "Sentinel.h"
#include "Log.h"
#include "../World.h"
#include "SMA.h"
#include "dataanalysis.h"
#include "../Player.h"

#define PI 3.141592654f
#define MOVEMENT_POINTS_TO_ANALYSE 500

//#define SENTINEL_DEBUG

using namespace std;
using namespace alglib;

Sentinel::Sentinel(Player* pOwner)
{
	MANGOS_ASSERT(pOwner);
	m_pOwner = pOwner;

	m_fPlayerX = pOwner->GetPositionX();
	m_fPlayerY = pOwner->GetPositionY();

	m_uiActionLevel = sWorld.getConfig(CONFIG_UINT32_SENTINEL_ACTION_LEVEL);
	m_uiBanLength = sWorld.getConfig(CONFIG_UINT32_SENTINEL_BAN_TIME);

	m_fMovingAverageMax = 300;
	m_fMovingAverageMoved = 0;

	m_uiTime = WorldTimer::getMSTime();
	m_uiGraceTime = WorldTimer::getMSTime();

	m_uiDetectionCount = 0;

    m_bIsFlyingOrOnTransport = false;

	m_uiRunSpeed = m_pOwner->GetSpeed(MOVE_RUN);

	DEBUG_LOG("Sentinel: Module created for player %s. Account ID: %u", pOwner->GetName(), pOwner->GetSession()->GetAccountId());
}

void Sentinel::AnalyseMovement(Sentinel* parent)
{
#ifdef SENTINEL_DEBUG
	// Check how long the analysis takes to see if there might be performance issues.
	time_t start_time = time(0);
#endif

	// Perform data analysis with k-clustering to try and get waypoints from the movement data.
	double *data_points = new double[MOVEMENT_POINTS_TO_ANALYSE * 2];		// Input data.

	// Copy the input data to an array for use with alglib.
	for (int i = 0; i < MOVEMENT_POINTS_TO_ANALYSE; i++)
	{
		data_points[i * 2] = parent->m_Points[i].x;
		data_points[i * 2 + 1] = parent->m_Points[i].y;
	}

	real_2d_array xy;
	xy.setcontent(MOVEMENT_POINTS_TO_ANALYSE, 2, data_points);

	// Try K-values upto one fourth of the sample size.
	for (int i = 4; i < MOVEMENT_POINTS_TO_ANALYSE / 4; i += 20)
	{
		int current_level_hits = 0;	// Keep track of how many repetition hits we have on this level of clustering.

		clusterizerstate state; 	// Handles the analysis state.
		kmeansreport report;

		clusterizercreate(state);			// Initilise the state.
		clusterizersetpoints(state, xy, 2);		// Set the points to analyse.
		clusterizersetkmeanslimits(state, 2, 2);	// Set the parameters for the analyser. Here 2 tries for the best centers and 2 iterations.
		clusterizerrunkmeans(state, i, report);		// Execute the analysis with i being the number of clusters to try.

		if (report.terminationtype > 0)		// A positive value means success.
		{
#ifdef SENTINEL_DEBUG
			sLog.outWarden("%li number of clusters:", report.k);
#endif

			float *cluster_average = new float[report.k];
			int *point_counter = new int[report.k];
			for (int y = 0; y < report.k; y++) 
			{
				cluster_average[y] = 0;
				point_counter[y] = 0;

#ifdef SENTINEL_DEBUG		
				sLog.outWarden("X: %.2f Y: %.2f", report.c[y][0], report.c[y][1]);
#endif		
			}

			for (int y = 0; y < report.npoints; y++)
			{
				// Sum all the distances for points in their cluster center
				// for calculating the cluster mean later.
				cluster_average[report.cidx[y]] += sqrt(pow(data_points[y * 2] - report.c[report.cidx[y]][0], 2) + pow(data_points[y * 2 + 1] - report.c[report.cidx[y]][1], 2));
				point_counter[report.cidx[y]] += 1;
			}

			for (int y = 0; y < report.k; y++)
			{
				cluster_average[y] /= point_counter[y];
#ifdef SENTINEL_DEBUG		
				sLog.outWarden("Cluster Avg: %.2f, with %i points.", cluster_average[y], point_counter[y]);
#endif		
				// If the average distance for a cluster is below 10 yds and the player 
				// has made at least 2 passes on the cluster we count it as a hit.
				if (cluster_average[y] < 10 && point_counter[y] >= 2)
					++current_level_hits;
			}

			// If we have more than 25 % of the clusters counting as hits before we've reached half the points
			// we're probably dealing with a bot and will flag the player as such.
			if ((float) current_level_hits / (float) i >= 0.25f)
			{
				sWorld.SendAntiCheatMessageToGMs(parent->m_pOwner->GetName(), "Possible bot detected!");
				sLog.outWarden("The player %s is a possible bot!", parent->m_pOwner->GetName());

				break;
			}

			delete[] cluster_average;
			delete[] point_counter;
		}
	}
	delete[] data_points;

	parent->m_Points.clear();

#ifdef SENTINEL_DEBUG
	time_t time_diff = time(0) - start_time;
	sLog.outWarden("Analysis duration: %li", time_diff);
#endif

}

void Sentinel::SetGraceTime()
{
	m_uiGraceTime = WorldTimer::getMSTime();
}


void Sentinel::SetPlayerPosition(float new_x, float new_y, uint32 current_time, OpcodesList opcode)
{
	// Only allow teleports to update the position directly.
	if (opcode != MSG_MOVE_TELEPORT_ACK || opcode != MSG_MOVE_WORLDPORT_ACK)
	{
		DEBUG_LOG("Sentinel: Only teleports are allowed to update the position directly!");
		return;
	}

	m_uiUpdateCause = opcode;

	m_fPlayerX = new_x;
	m_fPlayerY = new_y;
	m_uiTime = current_time;

}

void Sentinel::SetPlayerPosition(float new_x, float new_y)
{
	m_fPlayerX = new_x;
	m_fPlayerY = new_y;

	m_uiUpdateCause = 0;

	m_uiTime = WorldTimer::getMSTime();
}

void Sentinel::UpdatePosition(float /*x*/, float /*y*/, float /*orientation*/)
{
	// Don't do bot detection in the capitols.
	switch(m_pOwner->GetAreaId())
	{
	case 1537: // Ironforge
	case 1519: // Stormwind
	case 1497: // Undercity
	case 1657: // Darnassus
	case 1637: // Orgrimmar
	case 1638: // Thunderbluff
		return;
	default:
		break;
	}

	// Don't look for bots in certain maps.
	switch (m_pOwner->GetMapId())
	{
	case 489:   // Warsong Gulch
		return;
	default:
		break;
	}

	// Disabled the bot detection until it can be reconfigured.

// 	if (m_Points.empty())
// 	{
// 		m_Points.push_back(SentinelPoint(x, y, orientation));
// 	}
// 	else
// 	{
// 		SentinelPoint new_point(x, y, orientation);
// 
// 
// 		if (m_Points.back().GetDistanceToPoint(new_point) > 10.f && fabs(m_Points.back().orientation - orientation) > 2.f * PI / 5.f)
// 		{
// 			m_Points.push_back(new_point);
// 		}
// 
// 		if (m_Points.size() >= MOVEMENT_POINTS_TO_ANALYSE)
// 		{
// 			sWorld.GetThreadPool()->schedule(boost::bind(&Sentinel::AnalyseMovement, this));
// 		}
// 	}
}

bool Sentinel::ValidatePlayerMovement(float new_x, float new_y, uint32 current_time, OpcodesList opcode)
{
	// An extra fly check.
	if (m_pOwner->hasUnitState(UNIT_STAT_TAXI_FLIGHT) || m_pOwner->GetTransport() != NULL)
	{
		m_bIsFlyingOrOnTransport = true;
		return true;
	}

	// Opcodes that should be ignored.
	switch(opcode)
	{
	case MSG_MOVE_SET_FACING:
	case MSG_MOVE_START_TURN_LEFT:
	case MSG_MOVE_START_TURN_RIGHT:
	case MSG_MOVE_STOP_TURN:
		return true;
	default:
		break;
	}

	// Maps that shouldn't be checked.
	switch(m_pOwner->GetMap()->GetId())
	{
	case 369: // Deeprun Tram
        case 409: // Molten Core.
		return true;
	}


	// Make sure MC doesn't get you kicked.
	if (!m_pOwner->IsSelfMover())
		return true;

	// If not enough time has passed. Do not perform the check.
	uint32 diff = WorldTimer::getMSTimeDiff(m_uiTime, current_time);
	if ( diff < 500 || diff * 5 < m_pOwner->GetSession()->GetLatency())
		return true;

	bool overspeed = false;

	// Use the greater value of the previous speed and the current speed to not get false positives from speed changes between checks.
	float current_run_speed = m_uiRunSpeed < m_pOwner->GetSpeed(MOVE_RUN) ? m_pOwner->GetSpeed(MOVE_RUN) : m_uiRunSpeed;

	// Calculate the maximum distance a player should be able to move between updates.
	float max_dist = 2.f * current_run_speed * diff / 1000.f;  // Allow the player to move twice as fast as should be possible.
	float distance = sqrt(pow(m_fPlayerX - new_x, 2) + pow(m_fPlayerY - new_y, 2));

	if (m_fMovingAverageMax > 300.f)
		m_fMovingAverageMax = 300.f;

	if (m_fMovingAverageMoved > 200.f)
		m_fMovingAverageMoved = 200.f;

	// If the player has been teleported or it's within the grace time we skip the check.
	if (opcode != MSG_MOVE_TELEPORT_ACK && opcode != MSG_MOVE_WORLDPORT_ACK && WorldTimer::getMSTimeDiff(current_time, m_uiGraceTime) > 60000)
	{
		if (!m_bIsFlyingOrOnTransport)
		{
			m_fMovingAverageMax = max_dist * 0.15 + (1 - (0.15)) * m_fMovingAverageMax;
			m_fMovingAverageMoved = distance * 0.15 + (1 - 0.15) * m_fMovingAverageMoved;

			// Overspeeding
			if (m_fMovingAverageMoved > m_fMovingAverageMax || distance - max_dist > 100.f)
			{
				// You have to move further than you should more than 4 times the last 7,5 secs or 15 times your latency, whichever is greater.
				uint32 check_coeff = m_pOwner->GetSession()->GetLatency() < 500 ? 500 : m_pOwner->GetSession()->GetLatency();
				auto time = m_vecTriggerList.begin();
				while ( time != m_vecTriggerList.end())
				{
					if (WorldTimer::getMSTimeDiff(*time, current_time) > 10 * check_coeff)
						time = m_vecTriggerList.erase(time);
					else
						++time;
				}

				m_vecTriggerList.push_back(current_time);

				// If the extra distance is greater than 100 someone has definately teleported.
				if (m_vecTriggerList.size() > 4 || distance - max_dist > 100.f)
				{
					sLog.outWarden("Sentinel: Detected overspeeding for the player %s, account ID %u!", m_pOwner->GetName(), m_pOwner->GetSession()->GetAccountId());
					sWorld.SendAntiCheatMessageToGMs(m_pOwner->GetName(), "Sentinel has detected overspeeding!");

					if (m_uiActionLevel == 2)
					{
						m_pOwner->GetSession()->KickPlayer();
						sLog.outBasic("Sentinel: Action level set to 2 - player has been kicked!");
					}
					else if (m_uiActionLevel == 3)
					{
						sWorld.BanAccount(BAN_CHARACTER, m_pOwner->GetName(), m_uiBanLength, "Overspeeding", "Sentinel System");
						if (!m_uiBanLength)
							sLog.outBasic("Sentinel: Action level set to 3 - player has been permanently banned!");
						else
							sLog.outBasic("Sentinel: Action level set to 3 - player has been banned for %u milliseconds!", m_uiBanLength);
					}

					overspeed = true;
				}
			}
		}
		else
		{
			m_bIsFlyingOrOnTransport = false;
			m_uiGraceTime = WorldTimer::getMSTime();
		}
	}

	m_uiUpdateCause = opcode;
	m_fPlayerX = new_x;
	m_fPlayerY = new_y;
	m_uiRunSpeed = m_pOwner->GetSpeed(MOVE_RUN);
	m_uiTime = current_time;
	return !overspeed;
}



