#include "precompiled.h"
#include "patrol_ai.h"
#include "system.h"

npc_patrolAI::npc_patrolAI(Creature *pCreature, float orientation, bool usePathing) : ScriptedAI(pCreature)
{
    m_bUsePathing = usePathing;
    m_bPatrolling = false;
    m_bDonePatrolling = false;
    m_bIsPaused = false;
    m_uiRepeatPointID = 0;

    // Loads all the waypoints from the DB.
    FillPointMovementListForCreature();

    // Reset the NPC to the first waypoint.
    if (!WaypointList.empty())
        m_creature->RelocateCreature(WaypointList.front().fX, WaypointList.front().fY, WaypointList.front().fZ, orientation);

    m_creature->SetActiveObjectState(true);
}

npc_patrolAI::~npc_patrolAI()
{
}

void npc_patrolAI::FillPointMovementListForCreature()
{
    std::vector<ScriptPointMove> const &pPointsEntries = pSystemMgr.GetPointMoveList(m_creature->GetEntry());

    if (pPointsEntries.empty())
        return;

    std::vector<ScriptPointMove>::const_iterator itr;

    for (itr = pPointsEntries.begin(); itr != pPointsEntries.end(); ++itr)
    {
        Escort_Waypoint pPoint(itr->uiPointId, itr->fX, itr->fY, itr->fZ, itr->uiWaitTime);
        WaypointList.push_back(pPoint);
    }
}

void npc_patrolAI::FinishedPatrol()
{
}

void npc_patrolAI::MovementInform(uint32 /*uiMovementType*/, uint32 /*uiPointID*/)
{
    std::vector<Escort_Waypoint>::const_iterator next_point = m_TargetPoint;

    // Save the creatures new position in case of a despawn.
    m_creature->RelocateCreature(m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, 0);

    // Get the next point to check if it's valid.
    ++next_point;
    if (next_point != WaypointList.end())
    {
        ++m_TargetPoint;

        // If we are at the point we want to repeat to we save it for later use.
        if (m_TargetPoint->uiId == m_uiRepeatPointID)
            m_RepeatPoint = m_TargetPoint;

        if (!m_bIsPaused)
            m_creature->GetMotionMaster()->MovePoint(m_TargetPoint->uiId, m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, m_bUsePathing);
    }
    else
    {
        // If we are at the end of the waypoint list and we have a repeat point we set that as the current target.
        if (m_RepeatPoint != WaypointList.end())
        {
            m_TargetPoint = m_RepeatPoint;

            if (!m_bIsPaused)
                m_creature->GetMotionMaster()->MovePoint(m_TargetPoint->uiId, m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, m_bUsePathing);
        }
        else
        {
            m_bPatrolling = false;
            m_bDonePatrolling = true; // If we don't have a repeat point we still want the NPC to reset to the end point.

            FinishedPatrol();
        }
    }

}

void npc_patrolAI::Reset()
{
    // If we are patrolling we should resume that or if we're done patrolling we move to the end spot.
    if (m_bPatrolling || m_bDonePatrolling)
        m_creature->GetMotionMaster()->MovePoint(m_TargetPoint->uiId, m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, m_bUsePathing);
}


void npc_patrolAI::StartPatrol(uint32 repeatPoint, bool run)
{
    m_uiRepeatPointID = repeatPoint;

    if (run)
        m_creature->RemoveSplineFlag(SplineFlags::SPLINEFLAG_WALKMODE);

    if (!WaypointList.empty())
    {
        // Since the list is constant we can save an iterator to the current waypoint.
        m_TargetPoint = WaypointList.begin();

        // Check if the first waypoint is the repeat point.
        if (m_TargetPoint->uiId == repeatPoint)
        {
            m_RepeatPoint = m_TargetPoint;
        }
        else
        {
            // Set an invalid iterator for the repeat point for later checking in MovementInform.
            m_RepeatPoint = WaypointList.end();
        }

        // Save the creatures new position in case of a despawn.
        m_creature->RelocateCreature(m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, 0);

        // Start moving to the first waypoint.
        m_creature->GetMotionMaster()->MovePoint(m_TargetPoint->uiId, m_TargetPoint->fX, m_TargetPoint->fY, m_TargetPoint->fZ, m_bUsePathing);

        m_bPatrolling = true;
    }
}

void npc_patrolAI::PausePatrol(bool paused)
{
    m_bIsPaused = paused;
    
    if (!paused)
        Reset();
}


