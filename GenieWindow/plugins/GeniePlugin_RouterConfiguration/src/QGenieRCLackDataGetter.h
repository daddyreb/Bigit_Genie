#ifndef QGENIERCLACKDATAGETTER_H
#define QGENIERCLACKDATAGETTER_H

#include <QObject>
#include "Singleton.h"
class QGenieRCWirelessObject;
class QGenieRCLackDataGetter:public QObject,public Singleton<QGenieRCLackDataGetter>
{
    Q_OBJECT
public:
    enum ENUM_RESON
    {
        ER_WIRELESS
        ,ER_GACC
    };

    enum ENUM_DATA_STATUS
    {
        EDS_24OK=(1<<0)
        ,EDS_5OK=(1<<0)
    };
    quint32 mStatues;

    QGenieRCLackDataGetter(QGenieRCWirelessObject *rcwo);
    QGenieRCWirelessObject *mRcwo;
    QString *_24Ssid;
    QString *_24Pwd;
    QString *_5Ssid;
    QString *_5Pwd;
    bool tryUpdateLackedData(ENUM_RESON reson);
    void updatePointers();
protected:
    int mSession24;
    int mSession5;
    ENUM_RESON mReson;
protected slots:
    void slot_DataReturned(int session, const QStringList &data);
};

#endif // QGENIERCLACKDATAGETTER_H
