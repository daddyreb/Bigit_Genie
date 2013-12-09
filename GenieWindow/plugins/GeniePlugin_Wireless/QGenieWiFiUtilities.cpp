#include "QGenieWiFiUtilities.h"

#ifdef Q_OS_WIN
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;
#elif defined Q_OS_MACX
#include "WiFiApiGlobal.h"
#include "WiFiApiInterface.h"
extern WiFiApiInterface  *wifiApi_inf;
#endif

QString getWiFiNicUsable()
{
    QString nicguid;

    QStringList  nic_list;

    if(
#ifdef Q_OS_WIN
            (diagnose_inf != 0)  && diagnose_inf->GetWiFiInterfaces(nic_list)
#elif defined Q_OS_MACX
            (wifiApi_inf != 0) && wifiApi_inf->AllWLanInterfaces(nic_list)
#endif
            )
    {
        for(int y = 0 ; y < nic_list.size() ; ++y)
        {
            if(
#ifdef Q_OS_WIN
                    (diagnose_inf->IsWlanNicConnected(nic_list[y]) >= 0)
#elif defined Q_OS_MACX
                    wifiApi_inf->IsWlanNicActive(nic_list[y])
#endif
                    && (y != 0))
            {
                nic_list.swap(0,y);
                break;
            }
        }
    }

    if(nic_list.size() > 0)
    {
        nicguid = nic_list[0];
    }

    return nicguid;
}
