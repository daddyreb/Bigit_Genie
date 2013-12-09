#include "QGenieWiFiProfileManagerController.h"
////this file is bond to win32///////////////
#include "DiagnoseInterface.h"
extern DiagnoseInterface *diagnose_inf ;

#include <QtDebug>

QGenieWiFiProfileManagerController g_WifiProfileManager;

QGenieWiFiProfileManagerController::QGenieWiFiProfileManagerController(QObject *parent) :
        QObject(parent)
{
}

QString QGenieWiFiProfileManagerController::getProfileInfo(const QString &profileXml,const QString &nodeType)
{
    QString info;

    int index1 = profileXml.indexOf(QString("<") + nodeType + ">");
    int index2 = -1;
    if(index1 != -1)
    {
        index2 = profileXml.indexOf(QString("</") + nodeType + ">",index1);
    }

    if(index1 != -1 && index2 != -1)
    {
        QString tmp(QString("<") + nodeType + ">");
        index1 += tmp.length();
        info = profileXml.mid(index1,index2 - index1);
    }

    return info;
}

bool QGenieWiFiProfileManagerController::getProfileAuthAndConnType(const QString &profileName,QString &auth,bool &binfrastructure)
{
    QString profileXml;
    bool bresult = false;

    auth.clear();
    binfrastructure = false;

    if(diagnose_inf && diagnose_inf->GetWlanInterfaceProfile(m_curInterface,profileName,profileXml))
    {
        auth = getProfileInfo(profileXml,"authentication");
        if(auth == "WPA2PSK")
        {
            auth = "WPA2-PSK";
        }
        else if(auth == "WPAPSK")
        {
            auth = "WPA-PSK";
        }

        QString connType = getProfileInfo(profileXml,"connectionType");
        binfrastructure = (connType == "ESS");

        bresult = true;
    }

    return bresult;
}

bool QGenieWiFiProfileManagerController::updateProfileList(QStringList &profiles)
{
    bool bok = false;

    m_profileList.clear();
    m_curInterface.clear();

    QStringList nic_list;
    if(diagnose_inf && diagnose_inf->GetWiFiInterfaces(nic_list) && (nic_list.size() != 0))
    {
        m_curInterface = nic_list[0];
        bok = (!m_curInterface.isEmpty() && diagnose_inf->GetWlanProfileList(m_curInterface,m_profileList));
    }

    profiles = m_profileList;

    return bok;
}


bool QGenieWiFiProfileManagerController::deleteProfile(int index)
{
    bool bret = false;

    if((index >= 0) && (index < m_profileList.count()))
    {
        bret = diagnose_inf->DeleteWlanProfile(m_curInterface,m_profileList[index]);
        if(bret)
        {
            m_profileList.removeAt(index);
        }
    }

    return bret;
}

bool QGenieWiFiProfileManagerController::addProfile(const QString &ssid, const QString &auth,
                                                    const QString &cipher, const QString &pwd, bool nBroadcast)
{
    if(diagnose_inf)
    {
        return diagnose_inf->SetWlanInterfaceProfile(m_curInterface,ssid,auth,cipher,pwd,nBroadcast);
    }
    return false;
}

bool QGenieWiFiProfileManagerController::moveUpProfile(int index)
{
    bool bret = false;

    if((index > 0) && (index < m_profileList.count()))
    {
        QStringList tmpList = m_profileList;
        tmpList.insert(index - 1,tmpList.takeAt(index));
        bret =  diagnose_inf->SetWlanProfileList(m_curInterface,tmpList);
        if(bret)
        {
            m_profileList.insert(index - 1,m_profileList.takeAt(index));
        }
    }

    return bret;
}

bool QGenieWiFiProfileManagerController::moveDownProfile(int index)
{
    bool bret = false;

    if((index >= 0) && (index < (m_profileList.count() - 1)))
    {
        QStringList tmpList = m_profileList;
        tmpList.insert(index,tmpList.takeAt(index + 1));
        bret =  diagnose_inf->SetWlanProfileList(m_curInterface,tmpList);
        if(bret)
        {
            m_profileList.insert(index,m_profileList.takeAt(index + 1));
        }
    }

    return bret;
}

