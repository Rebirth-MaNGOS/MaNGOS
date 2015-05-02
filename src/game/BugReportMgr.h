#ifndef BUG_REPORT_MGR_H
#define BUG_REPORT_MGR_H

#include "Policies/Singleton.h"
#include "Database/DatabaseEnv.h"
#include "GMTicketMgr.h"

struct BugReport
{
    BugReport(std::string title, GMTicket &ticket)
    {
        m_creator = ticket.GetPlayerGuid();
        m_title = title;
        m_text = ticket.GetText();
        m_date = ticket.GetLastUpdate();
    }

    BugReport(unsigned long id, ObjectGuid creator, std::string title, std::string text, time_t date) :
        m_id(id), m_creator(creator), m_title(title), m_text(text), m_date(date) {}

    unsigned long m_id;
    ObjectGuid m_creator;
    std::string m_title;
    std::string m_text;
    time_t m_date;
};

typedef std::vector<BugReport> BugReportList;

class BugReportMgr
{
public:
    BugReportMgr();

    void AddReport(std::string title, GMTicket &ticket);
    bool RemoveReport(size_t index);
    BugReport& GetBugReport(size_t index);
    BugReportList const& GetBugReportList();

    void LoadBugReports();

private:
    BugReportList m_bugReports;
};

#define sBugReportMgr MaNGOS::Singleton<BugReportMgr>::Instance()
#endif
