#include "EventResourceMgr.h"
#include "Policies/SingletonImp.h"
#include "ProgressBar.h"

INSTANTIATE_SINGLETON_1(EventResourceMgr);

void EventResourceMgr::LoadResourceEvents()
{
    QueryResult* result = WorldDatabase.Query("SELECT event_id, resource_id, resource_full_count FROM event_resource;");

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

    result = CharacterDatabase.Query("SELECT event_id, resource_id, resource_count FROM event_resource_count;");
    if (!result)
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outBasic(">> Loaded 0 resource counts for the resource events.");
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
            uint32 event_id = fields[0].GetInt32();
            uint32 resource_id = fields[1].GetInt32();
            uint32 resource_count = fields[2].GetUInt32();

            m_resourceEvents[event_id][resource_id].current_count = resource_count;
        } while (result->NextRow());
        delete result;

        sLog.outBasic(">> Loaded %u resource counts for the resource events.", count);
    }
}
