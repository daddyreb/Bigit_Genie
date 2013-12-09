#ifndef SOAPRETURNHANDLER_H
#define SOAPRETURNHANDLER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QWaitCondition>
#include <QMutex>

class SoapReturnHandler : public QObject
{
    Q_OBJECT
public:
    explicit SoapReturnHandler(QObject *parent = 0);
    
public:
    int WaitForSoapReturn(const QMap<QString, QString> **resultMap);

signals:
    
private slots:
    void OnSoapReturn(int soapCode, const QMap<QString, QString> *resultMap);

private:
    int m_returnCode;
	const QMap<QString, QString> *m_resultMap;
	bool m_returnedFlag;
	QWaitCondition m_returnedCdt;
	QMutex m_returnMutex;
};

#endif // SOAPRETURNHANDLER_H
