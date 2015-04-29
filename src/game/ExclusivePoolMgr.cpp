#include "ExclusivePoolMgr.h"
#include "Database/DatabaseEnv.h"
#include "ProgressBar.h"

ExclusivePoolMgr::ExclusivePoolMgr()
{

}

void ExclusivePoolMgr::LoadFromDB()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM exclusive_pool");
    if (!result)
    {
        sLog.outString(">> Table exclusive_pool is empty.");
        sLog.outString();
        return;
    }
    
    BarGoLink bar(result->GetRowCount());
     
    do
    {
        Field* fields = result->Fetch();
        
        bar.step();
        
        uint32 poolID = fields[1].GetUInt32();
        time_t respawnTime = fields[2].GetUInt64();
        
        m_pools[poolID] = ExclusivePool(respawnTime);
        
    } while (result->NextRow());
     
    delete result;
     
    sLog.outBasic("Loaded %lu exclusive events...", m_pools.size());
     
    QueryResult* result1 = WorldDatabase.Query("SELECT * FROM exclusive_pool_creature");
    if (!result1)
    {
       sLog.outString(">> Table exclusive_pool_creature is empty.");
       sLog.outString();
       return;
    }
     
    BarGoLink bar1(result1->GetRowCount());
    
    uint32 counter = 0;
    do
    {
        Field* fields = result1->Fetch();
            
        ++counter;
        bar1.step();
        
        uint32 poolID = fields[1].GetUInt32();
        uint32 groupID = fields[2].GetUInt32();
        uint32 guid = fields[3].GetUInt32();
        
        m_pools[poolID].m_objects[groupID].push_back(ObjectGuid(HIGHGUID_UNIT, guid));
        
    } while (result1->NextRow());
    
    delete result1;
    
    sLog.outBasic("Loaded %u exclusive_pool_creatures...", counter);
    
    QueryResult* result2 = WorldDatabase.Query("SELECT * FROM exclusive_pool_spot");
    if (!result2)
    {
       sLog.outString(">> Table exclusive_pool_spot is empty.");
       sLog.outString();
       return;
    }
    
    BarGoLink bar2(result2->GetRowCount());
    
    counter = 0;
    do
    {
        Field* fields = result2->Fetch();
        
        ++counter;
        bar2.step();
        
        uint32 poolID = fields[1].GetUInt32();
        uint32 mapID = fields[2].GetUInt32();
        float x = fields[3].GetFloat();
        float y = fields[4].GetFloat();
        float z = fields[5].GetFloat();
        
        ExclusivePoolSpot spot = { mapID, x, y, z };
        m_poolSpots[poolID].push_back(spot);
    } while (result2->NextRow());
    
    delete result2;
    
    sLog.outBasic("Loaded %u exclusive_pool_spots...", counter);
    
}

void ExclusivePoolMgr::CheckEvents()
{

}

void ExclusivePoolMgr::SpawnEvent(uint32 eventID)
{

}
