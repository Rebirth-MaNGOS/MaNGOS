#include "ExclusivePoolMgr.h"
#include "Database/DatabaseEnv.h"
#include "ProgressBar.h"
#include "Policies/SingletonImp.h"
#include "ObjectMgr.h"
#include "MapManager.h"
#include "GridNotifiers.h"
#include "Cell.h"
#include "CellImpl.h"

#include <list>

INSTANTIATE_SINGLETON_1(ExclusivePoolMgr);

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
        
        uint32 poolID = fields[0].GetUInt32();
        time_t respawnTime = fields[1].GetUInt64();
        
        m_pools[poolID] = ExclusivePool(poolID, respawnTime);
        
    } while (result->NextRow());
     
    delete result;
     
    sLog.outBasic("Loaded %lu exclusive events...", m_pools.size());
     
    QueryResult* result1 = WorldDatabase.Query("SELECT * FROM exclusive_pool_creature");
    if (!result1)
    {
       sLog.outString(">> Table exclusive_pool_creature is empty.");
       sLog.outString();
    }
    else
    {
        BarGoLink bar1(result1->GetRowCount());
        
        uint32 counter = 0;
        do
        {
            Field* fields = result1->Fetch();
                
            ++counter;
            bar1.step();
            
            uint32 guid = fields[0].GetUInt32();
            uint32 poolID = fields[1].GetUInt32();
            uint32 groupID = fields[2].GetUInt32();
            
            
            const CreatureData* pData = sObjectMgr.GetCreatureData(guid);
            if (pData)
                m_pools[poolID].m_objects[groupID].push_back(pData->GetObjectGuid(guid));
            
        } while (result1->NextRow());
        
        delete result1;
        
        sLog.outBasic("Loaded %u exclusive_pool_creatures...", counter);
    }
    
    QueryResult* result2 = WorldDatabase.Query("SELECT * FROM exclusive_pool_spot");
    if (!result2)
    {
       sLog.outString(">> Table exclusive_pool_spot is empty.");
       sLog.outString();
    }
    else
    {   
        BarGoLink bar2(result2->GetRowCount());
        
        uint32 counter = 0;
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
            float o = fields[6].GetFloat();
            
            ExclusivePoolSpot spot = { mapID, x, y, z, o };
            m_poolSpots[poolID].push_back(spot);
        } while (result2->NextRow());
       
        delete result2;
        
        sLog.outBasic("Loaded %u exclusive_pool_spots...", counter);
    }
    
    QueryResult* result3 = WorldDatabase.Query("SELECT * FROM exclusive_pool_respawn");
    if (!result3)
    {
       sLog.outString(">> Table exclusive_pool_spot is empty.");
       sLog.outString();
    }
    else
    {
        BarGoLink bar3(result3->GetRowCount());
        
        do
        {
            Field* fields = result3->Fetch();
            
            bar3.step();
            
            uint32 poolID = fields[0].GetUInt32();
            time_t currentRespawnTime = fields[1].GetUInt64();
            
            m_pools[poolID].currentRespawnTime = currentRespawnTime;
        } while (result3->NextRow());
        
        delete result3;
    }
    
}

void ExclusivePoolMgr::CheckEvents()
{
    for (std::pair<const uint32, ExclusivePool>& poolPair : m_pools)
    {
        ExclusivePool& pool = poolPair.second;
        if (pool.currentRespawnTime <= time(nullptr))
            ExecuteEvent(pool);
    }
}

void ExclusivePoolMgr::ExecuteEvent(ExclusivePool& pool)
{
    sLog.outBasic("ExclusivePool: Shuffling pool %u...", pool.poolID);

    // Get the spawn points and shuffle them.
    std::vector<ExclusivePoolSpot> poolSpotList;
    poolSpotList.insert(poolSpotList.begin(), m_poolSpots[pool.poolID].begin(), m_poolSpots[pool.poolID].end());
    std::random_shuffle(poolSpotList.begin(), poolSpotList.end());
    
    for (std::pair<const uint32, std::list<ObjectGuid> >& poolPair : pool.m_objects)
    {
        // If we have run out of spawn positions we stop spawning creatures.
        if (poolSpotList.empty())
            break;
        
        std::list<ObjectGuid> poolObjectList = poolPair.second;

        // Check if any creatures in the current group are alive.
        // If they are the group should be skipped.
        bool foundAlive = false;
        for (ObjectGuid currentCreature : poolObjectList)
        {
            const CreatureData* pData = sObjectMgr.GetCreatureData(currentCreature.GetCounter());
            if (pData)
            {
                Map* pMap = sMapMgr.FindMap(pData->mapid);
                if (pMap)
                {
                    if (!pMap->IsLoaded(pData->posX, pData->posY))
                    {
                        MaNGOS::ObjectUpdater updater(0);
                        // for creature
                        TypeContainerVisitor<MaNGOS::ObjectUpdater, GridTypeMapContainer  > grid_object_update(updater);
                        // for pets
                        TypeContainerVisitor<MaNGOS::ObjectUpdater, WorldTypeMapContainer > world_object_update(updater);
                        // Make sure that the creature is loaded before checking its status.
                        CellPair cellPair = MaNGOS::ComputeCellPair(pData->posX, pData->posY);
                        Cell cell(cellPair);
                        pMap->Visit(cell, grid_object_update);
                        pMap->Visit(cell, world_object_update);
                    }


                    Creature* pCreature = pMap->GetCreature(currentCreature);
                    if (pCreature)
                    {
                        // If the creature is alive or being looted we don't include it in the randomisation.
                        if (pCreature->isAlive() || pCreature->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
                        {
                            sLog.outBasic("ExclusivePool: Skipping creature with guid %u.", currentCreature.GetCounter());
                            auto itr = std::find_if(poolSpotList.begin(), poolSpotList.end(),
                            [&](const ExclusivePoolSpot& spot)
                            {
                                if (spot.x == pData->posX && spot.y == pData->posY && 
                                    spot.z == pData->posZ && spot.mapID == pData->mapid)
                                {
                                    return true;
                                }
                                else
                                    return false;
                            });

                            // If we found the spot on which the living creature is standing
                            // we remove that spot since it's occupied.
                            if (itr != poolSpotList.end())
                            {
                                poolSpotList.erase(itr);
                                foundAlive = true;
                            }
                            else
                            {
                                sLog.outBasic("ExclusivePool: Could not find the pool position for creature %u. Moving it to avoid double spawns!", currentCreature.GetCounter());
                            }

                            DespawnAllExcept(poolObjectList, currentCreature);

                            break;
                        }
                    }
                }
            }
        }

        // If a creature in the current group was alive we skip it.
        if (foundAlive)
            continue;

        
        // Pick a random creature from the current group.
        auto itr = poolObjectList.begin();
        std::advance(itr, urand(0, poolObjectList.size() - 1));
        
        // Get a spawn point for the creature.
        ExclusivePoolSpot spot = poolSpotList.back();
        poolSpotList.pop_back();

        CreatureData& rData = sObjectMgr.mCreatureDataMap[itr->GetCounter()];
        
        // If the creature is already in the correct spot we skip it.
        if (rData.posX == spot.x && rData.posY == spot.y && 
            rData.posZ == spot.z && rData.mapid == spot.mapID)
        {
            continue;
        }
        
        // Do the actual spawning.
        sObjectMgr.RemoveCreatureFromGrid(itr->GetCounter(), &rData);
        Creature::AddToRemoveListInMaps(itr->GetCounter(), &rData);
        
        rData.posX = spot.x;
        rData.posY = spot.y;
        rData.posZ = spot.z;
        rData.orientation = spot.orientation;
        rData.mapid = spot.mapID;
        
        // Update the creature entry in the database.
        WorldDatabase.PQuery("UPDATE creature SET map=%u, position_x=%f, position_y=%f, position_z=%f, orientation=%f WHERE guid=%u",
                             spot.mapID, spot.x, spot.y, spot.z, spot.orientation, itr->GetCounter());

        // Make sure that all other creatures in the group are despawned.
        DespawnAllExcept(poolObjectList, *itr);
    }
    
    SaveRespawnTime(pool);

    sLog.outBasic("ExclusivePool: Finished shuffling pool %u.", pool.poolID);
}

void ExclusivePoolMgr::SaveRespawnTime(ExclusivePool& pool)
{
    time_t nextTime = time(nullptr) + pool.respawnDelay;
    
    WorldDatabase.PExecute("REPLACE INTO exclusive_pool_respawn (`poolID`, `spawntime`) VALUES (%u, %li)",
                           pool.poolID, nextTime);
    pool.currentRespawnTime = nextTime;    

    sLog.outBasic("ExclusivePool: Next shuffle in %lu seconds.", pool.respawnDelay);
}

void ExclusivePoolMgr::DespawnAllExcept(const std::list<ObjectGuid>& group, const ObjectGuid& except) const
{
   for (const ObjectGuid& guid : group)
   {
       if (except == guid)
           continue;

       const CreatureData* pData = sObjectMgr.GetCreatureData(guid.GetCounter());
       if (pData)
       {
            sObjectMgr.RemoveCreatureFromGrid(guid.GetCounter(), pData);
            Creature::AddToRemoveListInMaps(guid.GetCounter(), pData);
       } 
   }
}
