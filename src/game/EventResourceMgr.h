#ifndef EVENT_RESOURCE_MGR
#define EVENT_RESOURCE_MGR
#include <map>
#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"

struct ResourceType
{
    uint32 current_count;
    uint32 full_count;
};

typedef std::map<uint32, ResourceType> ResourceEvent ;

class EventResourceMgr
{
    void LoadResourceEvents();
    
    private:
        std::map<uint32, ResourceEvent> m_resourceEvents;
};

#define sEventResourceMgr MaNGOS::Singleton<EventResourceMgr>::Instance()
#endif
