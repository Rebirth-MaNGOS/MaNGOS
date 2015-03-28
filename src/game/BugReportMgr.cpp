#include "BugReportMgr.h"
#include "ProgressBar.h"

#include <stdexcept>

INSTANTIATE_SINGLETON_1(BugReportMgr);

BugReportMgr::BugReportMgr()
{
}

void BugReportMgr::AddReport(std::string title, GMTicket &ticket)
{
    BugReport report(title, ticket);

    std::string escTitle = title;
    CharacterDatabase.escape_string(escTitle);

    std::string escText = report.m_text;
    CharacterDatabase.escape_string(escText);
    
    QueryResult *result = CharacterDatabase.Query("SELECT MAX(id) AS max_id FROM character_bugreport");
    
    if (result)
    {
        Field *fields = result->Fetch();

        report.m_id = fields[0].GetUInt64() + 1; // Add one since the new report won't have been written yet.
        
        if (report.m_id == 1)
        {
            // If we don't find any reports we reset the ID counter.
            CharacterDatabase.PExecute("TRUNCATE TABLE character_bugreport");
        }
    } 
    
    CharacterDatabase.PExecute("INSERT INTO character_bugreport (`creator`, `title`, `text`, `date`) VALUES ('%u', '%s', '%s', '%lu')", report.m_creator.GetCounter(), escTitle.c_str(), escText.c_str(), time(nullptr));

    m_bugReports.push_back(report);
}

bool BugReportMgr::RemoveReport(size_t index)
{
    if (index >= m_bugReports.size())
        return false;

    BugReport report = sBugReportMgr.GetBugReport(index);
    CharacterDatabase.PExecute("DELETE FROM character_bugreport WHERE id = '%lu' LIMIT 1", report.m_id);

    m_bugReports.erase(m_bugReports.begin() + index);

    return true;
}

BugReport& BugReportMgr::GetBugReport(size_t index)
{
    if (index >= m_bugReports.size())
        throw std::out_of_range("Error: The BugReport index is out of range!");

    return m_bugReports[index];
}

BugReportList const& BugReportMgr::GetBugReportList()
{
    return m_bugReports;
}

void BugReportMgr::LoadBugReports()
{
    m_bugReports.clear();                                  // For reload case

    QueryResult *result = CharacterDatabase.Query(
                "SELECT id, creator, title, text, date FROM character_bugreport ORDER BY id ASC");

    if( !result )
    {
        BarGoLink bar(1);

        bar.step();

        sLog.outString();
        sLog.outString(">> Loaded `character_bugreport`, table is empty.");
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt64();

        ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, fields[1].GetUInt32());

        std::string title = std::string(fields[2].GetCppString());

        std::string text = std::string(fields[3].GetCppString());

        time_t date = fields[4].GetUInt64();

        BugReport report(id, guid, title, text, date);

        m_bugReports.push_back(report);

    } while (result->NextRow());
    delete result;

    sLog.outString();
    sLog.outString(">> Loaded " SIZEFMTD " bug reports", m_bugReports.size());
    
}
