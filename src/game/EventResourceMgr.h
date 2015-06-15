#ifndef EVENT_RESOURCE_MGR
#define EVENT_RESOURCE_MGR
#include <map>
#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"

struct ResourceType
{
    uint32 id;
    uint32 current_count;
    uint32 full_count;
};

typedef std::map<uint32, ResourceType> ResourceEvent ;

class EventResourceMgr
{
    public:
        void LoadResourceEvents();
        void AddResourceCount(uint32 event_id, uint32 resource_id, int count);
        uint32 GetResourceCount(uint32 event_id, uint32 resource_id);
        uint32 GetFullResourceCount(uint32 event_id, uint32 resource_id);
        
    private:
        std::map<uint32, ResourceEvent> m_resourceEvents;
};

#define sEventResourceMgr MaNGOS::Singleton<EventResourceMgr>::Instance()
#endif
