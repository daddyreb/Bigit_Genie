#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QStringList>
#include <QNetworkReply>

class ExtenderUtil
{
public:
    ExtenderUtil();

    static void GetAPInfo(QString &strResponse, QMap<int, QStringList> &APListMap);
    static QString GetNodeInfo(QString s, QString nodeName);

    static QString wepKeyGenerater(const QString &passphrase, int keyLen, int delta);

    //bool isNum(QString str);

    static void setExtenderSleep(int msec);

    static QString encoding(QNetworkReply* reply);

    static QString loadLocalLanguage();

};

#endif // UTIL_H
