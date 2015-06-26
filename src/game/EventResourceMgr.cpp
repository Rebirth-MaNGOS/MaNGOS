#include "EventResourceMgr.h"
#include "Policies/SingletonImp.h"
#include "ProgressBar.h"
#include "GridNotifiers.h"
#include "MapManager.h"

INSTANTIATE_SINGLETON_1(EventResourceMgr);

void EventResourceMgr::LoadResourceEvents()
{
    QueryResult* result = WorldDatabase.Query("SELECT event_id, resource_id, resource_full_count FROM event_resource");

    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outBasic(">> Loaded 0 resource events.");
        return;
    }

    {
        BarGoLink bar(result->GetRowCount());
        uint32 count = 0;

        Field* fields;
        do
        {
            bar.step();

            fields = result->Fetch();
            uint32 event_id = fields[0].GetUInt32();
            uint32 resource_id = fields[1].GetUInt32();
            uint32 resource_full_count = fields[2].GetUInt32();

            m_resourceEvents[event_id][resource_id].full_count = resource_full_count;
            
            ++count;
        } while (result->NextRow());
        delete result;

        sLog.outBasic(">> Loaded %u resource events.", count);
    }

    result = CharacterDatabase.Query("SELECT id, event_id, resource_id, resource_count FROM event_resource_count");
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outBasic(">> Loaded 0 resource counts for the resource events.");
    }
    else
    {
        BarGoLink bar(result->GetRowCount());
        uint32 count = 0;

        Field* fields;
        do
        {
            ++count;

            bar.step();

            fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            uint32 event_id = fields[1].GetUInt32();
            uint32 resource_id = fields[2].GetUInt32();
            uint32 resource_count = fields[3].GetUInt32();

            m_resourceEvents[event_id][resource_id].current_count = resource_count;
            m_resourceEvents[event_id][resource_id].id = id;
        } while (result->NextRow());
        delete result;

        sLog.outBasic(">> Loaded %u resource counts for the resource events.", count);
    }

    result = WorldDatabase.Query("SELECT id, event_id, resource_id, resource_limit, object_guid FROM event_resource_gameobject");
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outBasic(">> Loaded 0 resource gameobjects for the resource events.");
    }
    else
    {
        BarGoLink bar(result->GetRowCount());
        uint32 count = 0;

        Field* fields;
        do
        {
            ++count;
            bar.step();

            fields = result->Fetch();
            uint32 event_id = fields[1].GetUInt32();
            uint32 resource_id = fields[2].GetUInt32();
            uint32 resource_limit = fields[3].GetUInt32();
            uint32 object_guid = fields[4].GetUInt32();

            ResourceCreatureList& list = m_resourceEvents[event_id][resource_id].objects;

            ResourceCreatureInfo info = { resource_limit, object_guid };
            list.push_back(info);
        } while (result->NextRow());
        delete result;

        sLog.outBasic(">> Loaded %u resource gameobjects for the resource events.", count);
    }

    // Load the initial states for all the objects.
    for (std::pair<const uint32, ResourceEvent>& eventPair : m_resourceEvents)
        CheckSpawnGOEvent(eventPair.first);

    sLog.outBasic(">> Finished loading the resource events.");
}

bool EventResourceMgr::AddResourceCount(uint32 event_id, uint32 resource_id, int count)
{
    bool notDone = true;
    ResourceType& resource_type = m_resourceEvents.at(event_id).at(resource_id);

    if (resource_type.current_count + count >= resource_type.full_count)
        notDone = false;

    resource_type.current_count += count;

    CharacterDatabase.PQuery("REPLACE INTO event_resource_count (`id`, `event_id`, `resource_id`,"
                             " `resource_count`) VALUES ('%u', '%u', '%u', '%u')", 
                             resource_type.id, event_id, resource_id, resource_type.current_count);

    CheckSpawnGOEvent(event_id);

    return notDone;
}

uint32 EventResourceMgr::GetResourceCount(uint32 event_id, uint32 resource_id)
{
    ResourceType& resource_type = m_resourceEvents.at(event_id).at(resource_id);

    return resource_type.current_count;
}

uint32 EventResourceMgr::GetFullResourceCount(uint32 event_id, uint32 resource_id)
{
    ResourceType& resource_type = m_resourceEvents.at(event_id).at(resource_id);

    return resource_type.full_count;
}

void EventResourceMgr::CheckSpawnGOEvent(uint32 event_id)
{
    ResourceEvent& resourceEvent = m_resourceEvents[event_id];

    // Sum the total needed and the total gathered of each resource.
    // second contains the needed limit and first contains the number
    // of currently gatherered resources.
    std::map<uint32, std::pair<uint32, uint32> > resourceSum;

    for (std::pair<const uint32, ResourceType>& resourcePair : resourceEvent)
    {
        ResourceType& resource = resourcePair.second;

        // Sum all the requirements for that gameObject.
        for (ResourceCreatureInfo& goInfo : resource.objects)
        {
            resourceSum[goInfo.object_guid].first += resource.current_count;
            resourceSum[goInfo.object_guid].second += goInfo.trigger_limit;
        }
    }

    for (std::pair<const uint32, std::pair<uint32, uint32> >& resourceSumPair : resourceSum)
    {
        uint32 guid = resourceSumPair.first;
        uint32 current_count = resourceSumPair.second.first;
        uint32 trigger_limit = resourceSumPair.second.second;

        const GameObjectData* pData = sObjectMgr.GetGOData(guid);
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

                GameObject* pObject = pMap->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, pData->id, guid));

                // If the object isn't spawned and the limit is reached; spawn it.
                // Otherwise the gameobject should be despawned if it's under the limit.
                if (!pObject && current_count >= trigger_limit)
                {
                    sLog.outBasic("EventResourceMgr: The limit of %u has been reached for %u. Spawning it!", 
                            trigger_limit, guid);

                    sObjectMgr.AddGameobjectToGrid(guid, pData);
                    GameObject::SpawnInMaps(guid, pData);

                }
            }

            if (current_count < trigger_limit)
            {
                sObjectMgr.RemoveGameobjectFromGrid(guid, pData);
                GameObject::AddToRemoveListInMaps(guid, pData);
            }

        }
        else
            sLog.outError("EventResourceMgr: Could not find the GameObject with GUID %u!", 
                          guid);

    }
}

bool AddResourceCount(uint32 event_id, uint32 resource_id, int count)
{
    return sEventResourceMgr.AddResourceCount(event_id, resource_id, count);
}

uint32 GetResourceCount(uint32 event_id, uint32 resource_id)
{
    return sEventResourceMgr.GetResourceCount(event_id, resource_id);
}

uint32 GetFullResourceCount(uint32 event_id, uint32 resource_id)
{
    return sEventResourceMgr.GetFullResourceCount(event_id, resource_id);
}
