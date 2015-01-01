#ifndef _SENTINEL_H
#define _SENTINEL_H

#include "Player.h"
#include "Opcodes.h"

#include <math.h>

struct SentinelPoint
{
    float x;
    float y;
    float orientation;
    
    SentinelPoint(float x, float y, float orientation) : x(x), y(y), orientation(orientation) {}
    SentinelPoint() : x(0.f), y(0.f), orientation(0.f) {}
    float GetDistanceToPoint(SentinelPoint &point) const { return sqrt(pow(x - point.x, 2) + pow(y - point.y, 2)); }
};

class Sentinel
{
public:
	Sentinel(Player* pOwner);

	bool ValidatePlayerMovement(float new_x, float new_y, uint32 current_time, OpcodesList opcode);		// Used for normal movement.
	void SetPlayerPosition(float new_x, float new_y, uint32 current_time, OpcodesList opcode);			// Used for teleports.
	void SetPlayerPosition(float new_x, float new_y);

	void SetGraceTime();
	void SetTaxi() { m_bIsFlyingOrOnTransport = true; }
	
	// For bot detection
	void UpdatePosition(float x, float y, float orientation);

private:
	Player* m_pOwner;

protected:
	// Config words.
	uint32 m_uiActionLevel;
	uint32 m_uiBanLength;

	// Suspend Sentinel movement 60 secs from this time.
	uint32 m_uiGraceTime;

	// State variables.
	float m_fPlayerX;
	float m_fPlayerY;
	uint32 m_uiRunSpeed;
	bool m_bIsFlyingOrOnTransport;
	uint32 m_uiTime;
	uint32 m_uiUpdateCause;							// Save the opcode of the movement.
	std::vector<uint32> m_vecTriggerList;
	float m_fMovingAverageMoved;
	float m_fMovingAverageMax;
	uint32 m_uiDetectionCount;
	
	// Bot detection variables.
	static void AnalyseMovement(Sentinel* parent);
	std::vector<SentinelPoint> m_Points;
};

#endif
