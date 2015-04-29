#include "Object.h"
#include "World.h"

#include <list>
#include <map>

struct ExclusivePoolSpot
{
    uint32 mapID;
    float x;
    float y;
    float z;
};

struct ExclusivePool
{
    time_t respawnDelay;
    
    // The key is a sub-group with exclusive objects in the pool.
    std::map<uint32, std::list<ObjectGuid> > m_objects;
    
    ExclusivePool() : respawnDelay(0) {}
    ExclusivePool(time_t respawnDelay) : respawnDelay(respawnDelay) {}
};

class ExclusivePoolMgr
{
    void LoadFromDB();
    void SpawnEvent(uint32 eventID);
    
public:
    ExclusivePoolMgr();
    
    void CheckEvents();
    
private:
    std::map<uint32, ExclusivePool> m_pools;
    std::map<uint32, std::list<ExclusivePoolSpot> > m_poolSpots;
};