#ifndef BUG_REPORT_MGR_H
#define BUG_REPORT_MGR_H

#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"
#include "GMTicketMgr.h"

struct BugReport
{
    BugReport(GMTicket &ticket)
    {
        m_creator = ticket.GetPlayerGuid();
        m_commentor = ObjectGuid();
        m_text = ticket.GetText();
        m_comment = "";
        date = ticket.GetLastUpdate();
    }

    ObjectGuid m_creator;
    ObjectGuid m_commentor;
    std::string m_text;
    std::string m_comment;
    time_t date;
};

typedef std::vector<BugReport> BugReportList;

class BugReportMgr
{
public:
    BugReportMgr();

    void AddReport(GMTicket &ticket);
    bool RemoveReport(size_t index);
    BugReport& GetBugReport(size_t index);
    BugReportList const& GetBugReportList();

private:
    BugReportList m_bugReports;
};

#define sBugReportMgr MaNGOS::Singleton<BugReportMgr>::Instance()
#endif
