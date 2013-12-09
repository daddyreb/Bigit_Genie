#include "detectportthread.h"
#include "udt.h"
#include <QtDebug>

detectportthread::detectportthread(QString strIp, QString strServiceName, QObject *parent) :
    QThread(parent)
{
    m_strIp = strIp;
    m_strServiceName = strServiceName;
}


void detectportthread::run()
{
    struct addrinfo hints, *peer;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    UDTSOCKET fhandle = UDT::socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

    std::string strIp = m_strIp.toStdString();
    std::string strDestPort = "7777";
    if (0 != getaddrinfo(strIp.c_str(), strDestPort.c_str(), &hints, &peer))
    {
        UDT::close(fhandle);
        emit portOffline(m_strIp, m_strServiceName);
        return ;
    }

    // connect to the server, implict bind
    if (UDT::ERROR == UDT::connect(fhandle, peer->ai_addr, peer->ai_addrlen))
    {
        qDebug() << "connect error : " << UDT::getlasterror().getErrorMessage() << endl;
        UDT::close(fhandle);
        emit portOffline(m_strIp, m_strServiceName);
        return ;
    }

    freeaddrinfo(peer);
    UDT::close(fhandle);
}
