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

// Wrote this simple wrapper around bool
// to ensure that it's always initialised
// to false.
class ResourceEventStatus
{
    public:
        ResourceEventStatus() : m_completed(false) {}

        bool& operator=(const bool &lhs) { m_completed = lhs; return m_completed; }
        operator bool() { return m_completed; }
    private:
        bool m_completed;
};

class EventResourceMgr
{
    public:
        void LoadResourceEvents();

        // Returns false if the added amount would bring the total over the cap
        // or to the cap
        bool AddResourceCount(uint32 event_id, uint32 resource_id, int count);

        uint32 GetResourceCount(uint32 event_id, uint32 resource_id);
        uint32 GetFullResourceCount(uint32 event_id, uint32 resource_id);
        // Change all resources in an event by a percentage, ex. 0.8 to reduce it by 20 %.
        void ChangeAllResourcesByPercentage(uint32 event_id, float percentage);

        void CheckSpawnGOEvent(uint32 event_id);

        bool IsEventCompleted(uint32 event_id);
        
    private:
        void SaveResourceCount(uint32 event_id, uint32 resource_id, uint32 value);

        std::map<uint32, ResourceEvent> m_resourceEvents;
        std::map<uint32, ResourceEventStatus> m_resourceEventStatuses;
};

#define sEventResourceMgr MaNGOS::Singleton<EventResourceMgr>::Instance()

// Export functions from the EventResourceMgr so they can be used in Scriptdev
// without breaking windows.
bool MANGOS_DLL_SPEC AddResourceCount(uint32 event_id, uint32 resource_id, int count);
uint32 MANGOS_DLL_SPEC GetResourceCount(uint32 event_id, uint32 resource_id);
uint32 MANGOS_DLL_SPEC GetFullResourceCount(uint32 event_id, uint32 resource_id);
void MANGOS_DLL_SPEC ChangeAllResourcesByPercentage(uint32 event_id, float percentage);
bool MANGOS_DLL_SPEC IsEventCompleted(uint32 event_id);
#endif
