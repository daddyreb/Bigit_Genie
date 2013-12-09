#include "httpget.h"



HttpGet::HttpGet()
{
    m_user = "";
    m_passwd = "";

    networkManager = new QNetworkAccessManager;
    connect(networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
		this, SLOT(autheticate(QNetworkReply*,QAuthenticator*)));

    connect(networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(netManagerFinish(QNetworkReply*)));

}

HttpGet::~HttpGet()
{

}

void HttpGet::netManagerFinish(QNetworkReply *reply)
{
    qDebug() << "Request Finished!" << reply->error();
}

void HttpGet::autheticate(QNetworkReply *reply, QAuthenticator *auth)
{
    qDebug() << "Request Finished!" << reply->error();
    qWarning() << "authenticate required..." << reply->readAll();

    if( !m_user.isEmpty() && auth)
        auth->setUser(m_user);

    if( !m_passwd.isEmpty() && auth)
        auth->setPassword(m_passwd);

}

QIODevice* HttpGet::requestUrl()
{
    qDebug() << "Get http by basic authentication!";

    m_user = "admin";
    m_passwd = "passwor";

    QString mstrUrl = "http://routerlogin.net/index.htm";
    QUrl mmGetUrl = QUrl(mstrUrl);
    mmGetUrl.setUserName(m_user);
    mmGetUrl.setPassword(m_passwd);


    QNetworkReply* mreply = networkManager->get(QNetworkRequest(mmGetUrl));
    connect(mreply, SIGNAL(finished()), this, SLOT(finishReply()));
    
    
    return mreply;

}

void HttpGet::replyFinished(QNetworkReply* reply)
{
    qDebug() << "Request Finished!" << reply;
}

void HttpGet::finishReply()
{

    QObject* obj = sender();
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(obj);
    qDebug() << "Finished!" << "unknown error: " << reply->error();

    if( reply->error()){
        QString errstr = tr("No Error!");

        switch(reply->error()){
            case QNetworkReply::ConnectionRefusedError:
                errstr = tr("Connection refused!");
                break;
            case QNetworkReply::HostNotFoundError:
                errstr = tr("Host not found!");
                break;
            case QNetworkReply::RemoteHostClosedError:
                errstr = tr("Remote host closed!");
                break;
            case QNetworkReply::TimeoutError:
                errstr = tr("Timeout!");
                break;
            case QNetworkReply::ContentAccessDenied:
                errstr = tr("Content access denied!");
                break;
            case QNetworkReply::ProtocolFailure:
                errstr = tr("Protocol failure!");
                break;
            case QNetworkReply::ContentNotFoundError:
                errstr = tr("Content not found!");
                break;
            default:
                //qDebug() << "unknown error: " << reply->error();
                break;

        }

        emit networkError(errstr);
        return;

    }

//#ifdef DEBUG
    QFile temp("tmp.xml");
    temp.open(QIODevice::WriteOnly);
    temp.write(reply->readAll());
    temp.close();
//#endif

    emit requestFinished();

}

void HttpGet::errorReply(QNetworkReply::NetworkError error)
{
    qDebug() << "Error!" << error;
}
