#include "networktransaction.h"
#include "mainpipedefines.h"
#include <QNetworkReply>
#include <QDomDocument>
#include <QList>


#include <QtDebug>

#ifndef _MSC_VER
const int NetworkTransaction::PORT_80;
const int NetworkTransaction::PORT_5000;
#endif  // !_MSC_VER

#define SOAP_RESPONSE_OK    0
#define SOAP_RESPONSE_REBOOT    1
#define SOAP_RESPONSE_FN_NOT_SUPPORT    2
#define SOAP_RESPONSE_INVALID_ACTION    401
#define SOAP_RESPONSE_INVALID_ARGS   402
#define SOAP_RESPONSE_ACTION_FAILED  501


NetworkTransaction::NetworkTransaction(QNetworkReply *reply, QByteArray *requestData, QObject *parent) :
    QObject(parent)
  ,m_networkReply(reply)
  ,m_requestData(requestData)
  ,m_timeoutThresholdSeconds(-1)
  ,m_retryTimesRemain(1)
{
    connect(m_networkReply, SIGNAL(finished()), SLOT(RequestFinished()));
}

NetworkTransaction::~NetworkTransaction()
{
    if(m_networkReply && !m_networkReply->isFinished()){
        disconnect(m_networkReply, SIGNAL(finished()), this, SLOT(RequestFinished()));
        m_networkReply->abort();
    }

    delete m_networkReply;
    delete m_requestData;
}


void NetworkTransaction::ParseResponseData(const QByteArray &xmlData, QMap<QString, QString> &parseResult)
{
    QDomDocument domDoc;
    domDoc.setContent(xmlData, true);

    QDomNode domNode = domDoc.firstChild();
    if(!domNode.isElement()){
        domNode = domNode.nextSibling();
    }


    QList<QDomNodeList> nodeLists;
    nodeLists.append(domNode.childNodes());

    int nodeCount = 0;
    QDomNode iterNode;
    QDomNodeList childrenList;

    while(!nodeLists.isEmpty()){
        childrenList = nodeLists.takeFirst();
        nodeCount = childrenList.count();

        for(int index = 0 ; index < nodeCount ; ++index){
            iterNode = childrenList.item(index);

            if(iterNode.firstChild().isElement()){
                nodeLists.append(iterNode.childNodes());
            }else{
                QString tagName = iterNode.toElement().tagName();
                if(tagName.contains(":")){
                    tagName = tagName.mid(tagName.indexOf(":") + 1);
                }
                parseResult[tagName] = iterNode.toElement().text();
            }
        }
    }
}

void NetworkTransaction::TransactionTimeout()
{
    ProcessTransactionEnd(SOAP_TIMEOUT, 0);
    qDebug() << "Transaction time out!!!";
}

void NetworkTransaction::AbortTransaction()
{
    this->disconnect(SIGNAL(Finished(NetworkTransaction*)));
    ProcessTransactionEnd(SOAP_ABORT, 0);
    qDebug() << "NetworkTransaction Abort!!!";
}

void NetworkTransaction::ProcessTransactionEnd(int soapCode, const QMap<QString, QString> *resultMap)
{
    emit ReturnResult(soapCode, resultMap);
    emit Finished(this);
}

int NetworkTransaction::ParseResponseCode(const QString &responseCode)
{
    int soapCode = SOAP_UNKNOW_ERROR;
    bool convertOK = false;
    int iretCode = responseCode.trimmed().toInt(&convertOK);
    if(!convertOK)
        return soapCode;

    switch(iretCode){
    case SOAP_RESPONSE_OK:
        soapCode = SOAP_OK;
        break;
    case SOAP_RESPONSE_REBOOT:
        soapCode = SOAP_REBOOT_REQUIRED;
        break;
    case SOAP_RESPONSE_FN_NOT_SUPPORT:
        soapCode = SOAP_FUN_NOT_SUPPORT;
        break;
    case SOAP_RESPONSE_INVALID_ACTION:
        soapCode = SOAP_INVALID_ACTION;
        break;
    case SOAP_RESPONSE_INVALID_ARGS:
        soapCode = SOAP_INVALID_ARGS;
        break;
    case SOAP_RESPONSE_ACTION_FAILED:
        soapCode = SOAP_ACTION_FAILED;
        break;
    default:
        soapCode = SOAP_UNKNOW_ERROR;
    }

    return soapCode;
}

void NetworkTransaction::RequestFinished()
{
    QNetworkReply *currentReply = m_networkReply;
    QNetworkRequest correspondRequest = currentReply->request();

    //    qDebug () << "Corresponding Request url:" << correspondRequest.url().toString()
    //              << ";" << correspondRequest.url().host() << ";" << correspondRequest.url().port()
    //              << ";" << correspondRequest.url().path();

    QMap<QString, QString> *resultMap = 0;
    int soapCode = SOAP_FAILED;

    if(currentReply->error() == QNetworkReply::NoError){
        resultMap = new QMap<QString,QString>;//delete by NETGEARSoapApi
        QByteArray replyData = currentReply->readAll();
        ParseResponseData(replyData, *resultMap);

        //qDebug() << "Relpy data :" << replyData ;
        //qDebug() << "resultMap :" << *resultMap;

        soapCode = ParseResponseCode((*resultMap)["ResponseCode"]);

    }else{

        if((m_retryTimesRemain--) > 0){
            QUrl newUrl = correspondRequest.url();
            newUrl.setPort((correspondRequest.url().port() == PORT_80) ? PORT_5000 : PORT_80);
            correspondRequest.setUrl(newUrl);
            qDebug () << "try new port : " << newUrl.port();

            m_networkReply = currentReply->manager()->post(correspondRequest, *m_requestData);
            connect(m_networkReply, SIGNAL(finished()), SLOT(RequestFinished()));

            currentReply->deleteLater();
        }else{
            qDebug() << "Reply finished error : " << currentReply->errorString();
        }
    }

    if(m_networkReply == currentReply){
        ProcessTransactionEnd(soapCode, resultMap);
    }
}
