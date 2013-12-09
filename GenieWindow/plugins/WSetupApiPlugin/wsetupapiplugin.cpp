#include "wsetupapiplugin.h"
#include "wsetupdll.h"
#include <QtPlugin>

WSetupApiPlugin::WSetupApiPlugin()
{
}

WSetupApiPlugin::~WSetupApiPlugin()
{
}

bool  WSetupApiPlugin::get_export_information(QString &str_nicguid,QString &str_ssid,QString  &str_profilename)
{
    wchar_t *pnicguid=0,*pssid=0,*pprofilename=0;
    if(ex_get_export_information(&pnicguid,&pssid,&pprofilename))
    {
        str_nicguid           = QString::fromWCharArray(pnicguid);
        str_ssid                = QString::fromWCharArray(pssid);
        str_profilename    = QString::fromWCharArray(pprofilename);

        ex_free_export_information(pnicguid,pssid,pprofilename);

        return true;
    }
    else
    {
        return false;
    }
}

bool  WSetupApiPlugin::export_to_file(const QString &str_nicguid,const QString &str_ssid,
                     const QString &str_profilename,const QString &str_filename)
{
    return ex_export_to_file(str_nicguid.toStdWString().c_str(),str_ssid.toStdWString().c_str(),
                             str_profilename.toStdWString().c_str(),str_filename.toStdWString().c_str());
}

bool  WSetupApiPlugin::export_to_file2(const QString &str_ssid,
                         const QString &str_password,const QString &str_filename)
{
    return ex_export_to_file2(str_ssid.toStdWString().c_str(),str_password.toStdWString().c_str(),
                              str_filename.toStdWString().c_str());
}

bool  WSetupApiPlugin::export_to_file2_v2(const QString &str_ssid,const QString &str_password,
                     const QString &str_auth,const QString &str_cipher,const QString &str_filename)
{
    return ex_export_to_file2_v2(str_ssid.toStdWString().c_str(),
                                 str_password.toStdWString().c_str(),
                                 str_auth.toStdWString().c_str(),
                                 str_cipher.toStdWString().c_str(),
                                 str_filename.toStdWString().c_str());
}

bool  WSetupApiPlugin::get_logicaldriver_letters(QStringList &letters)
{
    bool bret = false;
    wchar_t *pletters = 0;
    if (ex_get_logicaldriver_letters(&pletters))
    {
        wchar_t *p = pletters;
        while (*p!=0)
        {
            letters.append(QString::fromWCharArray(p));
            p +=3;
            ++p;//skip '\0'
        }
        bret = true;
        ex_free_logicaldriver_letters(pletters);
    }
    return bret;
}

bool  WSetupApiPlugin::is_logicaldriver_removable(const QString &ldriver)
{
    return ex_is_logicaldriver_removable(ldriver.toStdWString().c_str());
}

bool  WSetupApiPlugin::is_logicaldriver_cdrom(const QString &ldriver)
{
     return ex_is_logicaldriver_cdrom(ldriver.toStdWString().c_str());
}

bool  WSetupApiPlugin::is_logicaldriver_system(const QString &ldriver)
{
    return ex_is_logicaldriver_system(ldriver.toStdWString().c_str());
}


