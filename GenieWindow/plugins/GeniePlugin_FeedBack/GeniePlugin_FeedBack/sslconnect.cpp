#include "sslconnect.h"

SslConnect::SslConnect(QWidget *parent)
{
    sslSocket=0;
    sslSocket = new QSslSocket(parent);
    if (sslSocket)
    {
        connect(sslSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(es_SocketErr(QAbstractSocket::SocketError)));
        //connect(sslSocket,SIGNAL(disconnected()),this,SLOT(es_DisConnect()));
        connect(sslSocket,SIGNAL(readyRead()),this,SLOT(es_ReadyRead()));
        connect(sslSocket,SIGNAL(connected()),this,SLOT(es_Connected()));
    }
}

void SslConnect::iniSslConnect(const QString &hostAdd, const int &hostPort)
{
    //sslSocket->connectToHost(hostAdd,hostPort);;
   // Genie_Log("iniSslConnect start.");
    if (sslSocket)
    {
        sslSocket->connectToHostEncrypted(hostAdd,hostPort);
    }
    //Genie_Log("iniSslConnect end.");
}

void SslConnect::sendData(const QString &data)//(QString order, QString data)
{
    QByteArray block;

    block = data.toUtf8();
    if (sslSocket)
    {
        if (sslSocket->isValid())
        {
            sslSocket->write(block);
        }
    }
}

void SslConnect::es_SocketErr(QAbstractSocket::SocketError socketError)
{
    sslSocket->ignoreSslErrors();
    emit this->sslSocketError(QString::fromUtf8("Please Check Your Network Connection!"));
}

void SslConnect::es_DisConnect()
{
    nextBlockSize = 0;
    emit this->socketDisConn();
}

void SslConnect::es_ReadyRead()
{
//    QDataStream in(sslSocket);
//    in.setVersion(QDataStream::Qt_4_6);
//
//    forever {
//        if (nextBlockSize == 0)
//        {
//            if (sslSocket->bytesAvailable() < (int)sizeof(quint16))
//                break;
//            in >> nextBlockSize;
//        }
//
//        if (nextBlockSize == 0xFFFF)
//        {
//            break;
//        }
//
//        if (sslSocket->bytesAvailable() < nextBlockSize)
//           break;
//
//        QString dataStr ="";
//        in >> dataStr;
//        emit this->socketReadData(dataStr);
//
//        nextBlockSize = 0;
//    }

    while(sslSocket->bytesAvailable()>0)
    {
        if (sslSocket)
        {
            if (sslSocket->isValid())
            {
                QByteArray datagram;
                datagram.resize(sslSocket->bytesAvailable());
            //Genie_Log("sslSocket->read(datagram.data(),datagram.size());.");
                sslSocket->read(datagram.data(),datagram.size());
                QString msg=datagram.data();
                emit this->socketReadData(msg);
            }
        }
    }
}

void SslConnect::es_Connected()
{
    emit this->socketConnected();
}

void SslConnect::socketAbort()
{
    //Genie_Log(" sslSocket->abort().");
    if (sslSocket)
    {
        if (sslSocket->isValid())
        {
            sslSocket->abort();
        }
    }
}

void SslConnect::socketDisConn()
{
    //Genie_Log("sslSocket->disconnectFromHost().");
    if (sslSocket)
    {
        if (sslSocket->isValid())
        {
            //sslSocket->isOpen();
            sslSocket->disconnectFromHost();
        }
    }
}

