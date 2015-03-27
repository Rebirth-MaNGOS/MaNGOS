#include "BugReportMgr.h"

#include <stdexcept>

INSTANTIATE_SINGLETON_1(BugReportMgr);

BugReportMgr::BugReportMgr()
{
}

void BugReportMgr::AddReport(GMTicket &ticket)
{
    m_bugReports.push_back(ticket);
}

bool BugReportMgr::RemoveReport(size_t index)
{
    if (index >= m_bugReports.size())
        return false;

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
