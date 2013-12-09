#ifndef QLAUNCHMANAGER_H
#define QLAUNCHMANAGER_H
#include "ILaunchManager.h"
#include "Singleton.h"
class QLaunchUpdateDialog;
class QUpdateProcess;



class QLaunchManager : public ILaunchManager,public Singleton<QLaunchManager>
{
public:
    QLaunchManager();
    ~QLaunchManager();
    bool startUpdate(UPDATE_RESON reson=UR_USER_CHOISE,QString lan="EN"
#ifdef SHEET_STYLE_FORMAC
                             ,QWidget *parent=0
#else
#endif
                     ); //if must restart return true

    virtual void setSetting(QString para,QVariant value);
#ifdef _WIN32
    virtual bool elevateThis();
#endif

    QLaunchUpdateDialog *mDialog;
    QUpdateProcess *mProcess;
    QString mGenieLanCache;
//    inline UPDATE_RESON getUpdateReson() const
//    {
//        return mUpdateReson;
//    }
    bool mQuitFlag;
//protected:
//    UPDATE_RESON mUpdateReson ;
};

#endif // QLAUNCHMANAGER_H
