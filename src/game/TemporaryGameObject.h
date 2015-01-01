#pragma once
#include "GameObject.h"

class MANGOS_DLL_SPEC TemporaryGameObject :
	public GameObject
{
public:
	/* Creates a temporary GameObject that is deleted after duration ms, 
	with duration = 0 being an infinite duration. If duration = 0 the object
	has to be manually deleted by calling Delete() on it.*/
	TemporaryGameObject(uint32 duration);
	~TemporaryGameObject();

	void Update(uint32 update_diff, uint32 p_time);

	/* Set the duration the GameObject should exist before being deleted.
	   The value is in ms and duration = 0 is an infinite duration. */
	void SetDuration(uint32 duration) { m_uiDuration = duration; }

private:
	uint32 m_uiDuration;
};

