#ifndef SSLCONNECT_H
#define SSLCONNECT_H

#include <QObject>
#include <QSslSocket>
#include <QWidget>
#include <QSslError>
#include <QAbstractSocket>
#include <QDataStream>
#include <QIODevice>
#include <QByteArray>


class SslConnect:public QObject
{
    Q_OBJECT
public:
    SslConnect(QWidget *parent);

    void iniSslConnect(const QString &hostAdd,const int &hostPort);
    void sendData(const QString &data);//QString order,
    void socketAbort();
    void socketDisConn();
    //void receivedData(QString &rdata);

signals:
    void sslSocketError(const QString &errorStr);
    //void socketDisConn();
    void socketConnected();
    void socketReadData(const QString &readData);
    void socketSendData(const QString &sendData);

private:
    QSslSocket *sslSocket;
    QString rData;
    QString sData;
    quint16 nextBlockSize;

private slots:
    void es_SocketErr(QAbstractSocket::SocketError socketError);
    void es_DisConnect();
    void es_ReadyRead();
    void es_Connected();
};

#endif // SSLCONNECT_H
