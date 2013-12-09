#include "DnsQuery_Win.h"
#include <windows.h>
#include <windns.h>

bool dnsquery_win(const QString &hostname)
{
    DNS_RECORD *pResult = NULL;
    DNS_STATUS status   = DnsQuery(hostname.toStdWString().c_str(),DNS_TYPE_A,DNS_QUERY_BYPASS_CACHE,NULL,&pResult,NULL);
    DnsRecordListFree(pResult,DnsFreeRecordList);
    return (0 == status);
}
