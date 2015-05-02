#ifndef EXCLUSIVE_POOL_MGR_H
#define EXCLUSIVE_POOL_MGR_H

#include "Object.h"
#include "World.h"
#include "Policies/Singleton.h"

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
    uint32 poolID;
    
    time_t respawnDelay;
    time_t currentRespawnTime;
    
    // The key is a sub-group with exclusive objects in the pool.
    std::map<uint32, std::list<ObjectGuid> > m_objects;
    
    ExclusivePool() : poolID(0), respawnDelay(0) {}
    ExclusivePool(uint32 poolID, time_t respawnDelay) : 
    poolID(poolID), respawnDelay(respawnDelay), currentRespawnTime(0) {}
};

class ExclusivePoolMgr
{
    void ExecuteEvent(ExclusivePool &pool);
    void SaveRespawnTime(ExclusivePool &pool);
    void DespawnAllExcept(const std::list<ObjectGuid>& group, const ObjectGuid& except) const;
    
public:
    ExclusivePoolMgr();
 
    void LoadFromDB();
    
    void CheckEvents();
    
private:
    std::map<uint32, ExclusivePool> m_pools;
    std::map<uint32, std::list<ExclusivePoolSpot> > m_poolSpots;
};

#define sExclusivePoolMgr MaNGOS::Singleton<ExclusivePoolMgr>::Instance()

#endif
