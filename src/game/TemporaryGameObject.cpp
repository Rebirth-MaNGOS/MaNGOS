#include "TemporaryGameObject.h"

TemporaryGameObject::TemporaryGameObject(uint32 duration) : GameObject()
{
	m_uiDuration = duration;
}


TemporaryGameObject::~TemporaryGameObject()
{
}

void TemporaryGameObject::Update(uint32 update_diff, uint32 p_time)
{
	if (m_uiDuration > 0)
	{
		if (m_uiDuration <= update_diff)
		{
			// If the duration has ended we delete the object.
			Delete();
		}
		else
			m_uiDuration -= update_diff;
	}

    if (m_lootState == GO_JUST_DEACTIVATED)
    {
        SetRespawnTime(0);
        Delete();
    }

	GameObject::Update(update_diff, p_time);
}
