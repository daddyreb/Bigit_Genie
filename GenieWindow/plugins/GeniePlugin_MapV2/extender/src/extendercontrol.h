#ifndef EXTENDERCONTROL_H
#define EXTENDERCONTROL_H
#include <QObject>
#include <QTranslator>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "extenderutilwifi.h"
#include "extendermainwindow.h"
#include "extendersoapapi.h"

class ExtenderControl : public QObject
{
    Q_OBJECT
public:
    explicit ExtenderControl();
    static ExtenderControl *GetInstance();
    ~ExtenderControl();

public:
    void extenderStart();
    void iniConnect();

public:
    QNetworkAccessManager GetCurrentSettingAccessManager;

    // check Extender: return 'Model=WN2500RP', 'Model=WN2500RP', 'NotRP'
    QString checkIsExtender(QString);
    void setHostIP(QString &strIP);
    bool getIsRunning();

protected:
    virtual void changeEvent(QEvent *);
    void retranslateUi();

public slots:
    void slt_getCurrentSetting(QNetworkReply* reply);
    void slt_connectInitExtender();
    void slt_startGetCurrentSetting();
    void slt_restart();

signals:
    void authenticateBegin(bool);
    void emitCurrentSettingStart();
    void emitConnectInitExtender();
    void translateText(int, QString *text);

private:
    QString translateText(int idx, const QString &defaultText);

private:
    ExtenderSoapApi *sp;
    ExtenderMainWindow *mw;
    ExtenderUtilWiFi *wifiUtil;
    int waitting_text_idx;

private:
    static ExtenderControl *m_instance;
};

#endif // EXTENDERCONTROL_H
