#ifndef SC_PATROLAI_H
#define SC_PATROLAI_H

#include "sc_creature.h"
#include "escort_ai.h"

/* AI that allows an npc to patrol a certain area. Loads its waypoints from the Scriptdevzero waypoint database.
   It follows the path to the end and then repeats from the waypoint with the ID passed when StartPatrol is called. */
struct MANGOS_DLL_DECL npc_patrolAI : public ScriptedAI
{
	npc_patrolAI(Creature *pCreature, float orientation, bool usePathing);  // Orientation is not saved in the waypoints. Set the NPC's initial facing in the constructor.
	~npc_patrolAI();

	virtual void FinishedPatrol();									// Called when patrol is finished if there is no repetition point.
	void MovementInform(uint32 uiMovementType, uint32 uiPointID);
	void Reset();
	void StartPatrol(uint32 repeatPoint = 0, bool run = true);                       // Start the patrol. Pass 0 to not have any repetition.

private:
	void FillPointMovementListForCreature();

	// Waypoint handling
	std::vector<Escort_Waypoint> WaypointList;
	std::vector<Escort_Waypoint>::const_iterator m_TargetPoint;
	std::vector<Escort_Waypoint>::const_iterator m_RepeatPoint;

	// Flags
	bool m_bUsePathing;
	bool m_bPatrolling;
	bool m_bDonePatrolling;
	uint32 m_uiRepeatPointID;

};

#endif
