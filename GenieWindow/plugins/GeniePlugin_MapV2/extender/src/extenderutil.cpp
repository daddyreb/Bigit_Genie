#include <QDebug.h>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTime>
#include <QCoreApplication>
//#include <QSystemLocale>  // This head not in 5.1?
#include <QVariant>

#include "extenderutil.h"

ExtenderUtil::ExtenderUtil()
{
}

bool isNum(QString str)
{
    int count = 0;
    int strlen;
    strlen=str.length();
    for(int i=0;i<strlen;i++)
        if(str[i]<'0' || str[i]>'9')
            count++;
    if(count > 0)
        return false;
    else
        return true;
}

void ExtenderUtil::GetAPInfo(QString &strResponse, QMap<int, QStringList> &APListMap)
{
    // Get the APList string
    int APListStart = strResponse.indexOf("<APList>");
    int APListEnd = strResponse.lastIndexOf("</APList>");
    QString APListStr = strResponse.mid(APListStart + QString("<APList>").length(), APListEnd - APListStart - QString("</APList>").length() + 1);

    // Get the APList string except "n@" and first "@1"
    int iFirst_at =  APListStr.indexOf("@");
    QString firstLayerString = APListStr.remove(0, iFirst_at + QString("@1").length());

    // Get the first layer stringlist from APList by "@n;"
    QStringList firstLayerStringList = firstLayerString.split(QRegExp("@\\d{1,}"));

    // Get the second layer stringlist
    int mapIndx = 0;
    foreach (QString firstLayerItem, firstLayerStringList) {
        // Remove the begin ";" and the end one
        firstLayerItem = firstLayerItem.remove(0, 1);
        firstLayerItem.remove(firstLayerItem.length() - 1, 1);

        // Get SSID by ";OFF" or "WEP" ...;
        QStringList secondLayerList;
        int securityTypeIndex;
        if (firstLayerItem.contains(";OFF")) {
            securityTypeIndex = firstLayerItem.indexOf(";OFF");
        } else if (firstLayerItem.contains(";WEP")) {
            securityTypeIndex = firstLayerItem.indexOf(";WEP");
        } else if (firstLayerItem.contains(";WPA-PSK-TKIP")) {
            securityTypeIndex = firstLayerItem.indexOf(";WPA-PSK-TKIP");
        } else if (firstLayerItem.contains(";WPA-PSK-AES")) {
            securityTypeIndex = firstLayerItem.indexOf(";WPA-PSK-AES");
        } else if (firstLayerItem.contains(";WPA2-PSK-TKIP")) {
            securityTypeIndex = firstLayerItem.indexOf(";WPA2-PSK-TKIP");
        } else if (firstLayerItem.contains(";WPA2-PSK-AES")) {
            securityTypeIndex = firstLayerItem.indexOf(";WPA2-PSK-AES");
        } else if (firstLayerItem.contains(";WPA-PSK-TKIP|WPA2-PSK-AES")) {
            securityTypeIndex = firstLayerItem.indexOf(";WPA-PSK-TKIP|WPA2-PSK-AES");
        }
        QString SSID = firstLayerItem.left(securityTypeIndex);
        secondLayerList.append(SSID);
        firstLayerItem.remove(0, securityTypeIndex + 1);

        // Split the second layer stringlist
        QStringList listTemp = firstLayerItem.split(";");
        if (listTemp.size() != 4) {
        } else {
            foreach (QString secondLayerItem, listTemp) {
                secondLayerList.append(secondLayerItem);
            }
        }

        APListMap.insert(mapIndx, secondLayerList);
        mapIndx ++;
    }
}

QString ExtenderUtil::GetNodeInfo(QString s, QString nodeName)
{
    int start = s.indexOf("<" + nodeName + ">");
    int end = s.lastIndexOf("</" + nodeName + ">");

    QString result;
    result = s.mid(start + nodeName.length() + 2, end - start - nodeName.length() - 2);
    return result;
    //qDebug() << result;
}

QString ExtenderUtil::wepKeyGenerater(const QString &passphrase, int keyLen, int delta)
{
    QString key;
    int idx = 0 ;
    int seed = 0;
    unsigned char randByte = 0;

    const int count = passphrase.length();

    do{

        if(idx < count){
            if(idx == (keyLen / 2 - 1)){
                seed = 0;
                for(int k = idx ; k < count ; ++k){
                    seed += (passphrase[k].unicode() + (k + 1) * delta);
                }
            }else{
                seed = (passphrase[idx].unicode() + (idx + 1) * delta);
            }
        }else{
            seed = (passphrase[passphrase.length() - 1].unicode() + (idx + 1) * delta);
        }

        qsrand(seed);

        randByte = (unsigned char)qrand();
        char buffer[16];
        sprintf(buffer, "%02X", randByte);
        key += buffer;

    }while(++idx < (keyLen / 2));

    return key;
}

void ExtenderUtil::setExtenderSleep(int msec)
{
    QTime t;
    t.start();
    while(t.elapsed() < msec)
        QCoreApplication::processEvents();
}

QString ExtenderUtil::encoding(QNetworkReply* reply)
{
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    return codec->toUnicode(reply->readAll());
}

QString ExtenderUtil::loadLocalLanguage()
{
//    QSystemLocale locale;
//    QVariant var(QLocale::UnitedStates);
//    var = locale.query(QSystemLocale::CountryId, var);

//    if(var.toInt() == QLocale::China) {
//        return "cn";
//    } else {
//        return "en";
//    }
    return "en";
}

