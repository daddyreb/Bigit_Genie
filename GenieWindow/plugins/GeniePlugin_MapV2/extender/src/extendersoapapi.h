#ifndef SOAPAPI_H
#define SOAPAPI_H

#include <QObject>
#include <QString>
#include <QtNetwork/QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ExtenderSoapApi : public QObject
{
    Q_OBJECT
public:
    explicit ExtenderSoapApi(QObject *parent = 0);

public:
    QString showSend();

signals:

public slots:
    //WLANConfiguration
    void Authenticate(QNetworkAccessManager *manager, QString port);

    void GetIsExtender(QNetworkAccessManager *manager, QString port);
    void GetInfo(QNetworkAccessManager *manager, QString port);

    void GetAPInfo(QNetworkAccessManager *manager, QString, QString port);

    void ConfigurationStarted(QNetworkAccessManager *manager, QString port);

    void SetExtenderMode(QNetworkAccessManager *manager, QString port,QString,QString,QString ,QString);

    void SetRouterWLANNoSecurity(QNetworkAccessManager *manager, QString port,QString,QString,QString ,QString ,int);
    void SetRouterWLANWEPByKeys(QNetworkAccessManager *manager, QString port,QString,QString,QString,QString ,QString ,int ,int,QString ,int);
    void SetRouterWLANWPAPSKByPassphrase(QNetworkAccessManager *manager, QString port,QString ,QString ,QString ,QString,QString,QString,int);

    void GetRouterWLANVerifyResult(QNetworkAccessManager *manager, QString port, QString NewRadio);

    void SetWLANNoSecurity(QNetworkAccessManager *manager, QString port,QString,QString,QString,QString);
    void SetWLANWEPByKeys(QNetworkAccessManager *manager,QString port, QString ,QString,QString,QString ,QString ,int ,int, QString,QString ,QString ,QString);
    void SetWLANWPAPSKByPassphrase(QNetworkAccessManager *manager, QString port,QString,QString,QString ,QString ,QString ,QString );

    void SetEnable(QNetworkAccessManager *manager, QString port);

    void ConfigurationFinished(QNetworkAccessManager *manager, QString port, QString NewRadio);

    void ResetToFactoryDefault(QNetworkAccessManager *manager, QString port, QString NewSerialNumber);

    //
    void GetCurrentSetting(QNetworkAccessManager *manager, QString port);
    void GetAttachedDevice(QNetworkAccessManager *manager, QString port); //
    void GetEnable(QNetworkAccessManager *manager, QString port); //
    void Is5GSupported(QNetworkAccessManager *manager, QString port); //


    // get configuration for advanced set
    void GetRouterWLANInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);
    void GetRouterWLANWPAInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);
    void GetRouterWLANWEPInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);
    void GetWLANInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);
    void GetWLANWPAInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);
    void GetWLANWEPInfo(QNetworkAccessManager *manager, QString port, QString NewRadio);

private:
    QNetworkReply  *GetAPInfoReplay;
    QString soapSend;
};

#endif // SOAPAPI_H
