#pragma once
#include <QMap>
#include <QObject>
#include <QUrl>
#include <QMutex>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include "SmartWifiParser.h"
#include "smartWifiProtocol.h"
class QNetworkReply;


class SmartWifiConnector :
	public QObject
//	public IVPNConnector
	
{
	Q_OBJECT
public:
	SmartWifiConnector(void);
	virtual ~SmartWifiConnector(void);
	void SetReplyTimeout(int sec);
    virtual bool GetEnable(QUrl url, bool &enable, QString& status_name, QString& waittime, QString& error);
    virtual bool SetEnable(QUrl url, bool enable, QString& error);
    virtual bool GetTimeout(QUrl url, QString& sec, QString& error);
    virtual bool SetTimeout(QUrl url, int sec, QString& error);
    virtual bool GetVPNInfo(QUrl url, QString& type, QString& info, QString& error);
    virtual bool GetWhiteUrl(QUrl url, WHITE_URL_TYPE type, QStringList& while_url, QString& error);
    virtual bool AddWhiteUrl(QUrl url, ADD_TYPE type, QString& white_url, QString& error);
	virtual bool DelWhiteUrl(QUrl url, QString& while_url, QString& error);
    virtual bool GetUsed(QUrl url, QString& type, QString& used, QString& error);
    virtual bool GetVPNUserName(QUrl url, QString& user_name, QString& error);
    virtual bool Test(QUrl url);
private:
    void InitRequest(QUrl url, QNetworkRequest &req, int contentLength);
	void IndexIncrease();
    QString SendReq(QUrl url, QString& xml);
private:
	unsigned long m_req_index;
	QMutex m_mutex;
	QMap<QNetworkReply*, int> m_reply_map;
	SmartWifiParser m_parser;
	int m_replyTimeout;
//	QNetworkAccessManager *m_manager;
};

