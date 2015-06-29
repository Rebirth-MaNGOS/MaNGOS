#ifndef EVENT_RESOURCE_MGR
#define EVENT_RESOURCE_MGR
#include <map>
#include <list>
#include "Database/DatabaseEnv.h"
#include "Policies/Singleton.h"
#include "ObjectMgr.h"

struct ResourceGameObjectInfo
{
    uint32 trigger_limit;
    uint32 object_guid;
};

typedef std::list<ResourceGameObjectInfo> ResourceGameObjectList;
struct ResourceType
{
    uint32 current_count;
    uint32 full_count;

    ResourceGameObjectList objects;
};

typedef std::map<uint32, ResourceType> ResourceEvent ;

class EventResourceMgr
{
    public:
        void LoadResourceEvents();

        // Returns false if the added amount would bring the total over the cap
        // or to the cap
        bool AddResourceCount(uint32 event_id, uint32 resource_id, int count);

        uint32 GetResourceCount(uint32 event_id, uint32 resource_id);
        uint32 GetFullResourceCount(uint32 event_id, uint32 resource_id);

        void CheckSpawnGOEvent(uint32 event_id);
        
    private:
        std::map<uint32, ResourceEvent> m_resourceEvents;
};

#define sEventResourceMgr MaNGOS::Singleton<EventResourceMgr>::Instance()

// Export functions from the EventResourceMgr so they can be used in Scriptdev
// without breaking windows.
bool MANGOS_DLL_SPEC AddResourceCount(uint32 event_id, uint32 resource_id, int count);
uint32 MANGOS_DLL_SPEC GetResourceCount(uint32 event_id, uint32 resource_id);
uint32 MANGOS_DLL_SPEC GetFullResourceCount(uint32 event_id, uint32 resource_id);
#endif
