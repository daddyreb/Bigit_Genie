#include "QLaunchFileDiscription.h"
#include <QtCore>

#define ANALYZE_STRATEGY 2

QLaunchFileDiscription::QLaunchFileDiscription(QString para)
#if ANALYZE_STRATEGY ==1
    :mSettings(para,QSettings::IniFormat)
#endif
{
#if ANALYZE_STRATEGY==2
    mSettingMap.clear();
    QFile f(para);
    if(f.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&f);
        QRegExp rxline("(.*)(\\s*)=(\\s*)(.*)");

        while (!stream.atEnd())
        {
            QString line = stream.readLine().trimmed();// line of text excluding '\n'
            int pos = rxline.indexIn(line);

            if (pos > -1)
            {
                QString key=rxline.cap(1);
                QString value = rxline.cap(4);
                value =value.replace("\\n","\n");
                mSettingMap.insert(key, value);
            }
            else
            {
                qDebug("\033[32mQRegExp Error!");
                continue;
            }
        }
    }
    f.close();
#endif
}

QLaunchFileDiscription::~QLaunchFileDiscription()
{

}

QString QLaunchFileDiscription::getPath()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/filename","").toString();
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("filename");
#endif
}

QString QLaunchFileDiscription::getReleaseNote()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/releasenote","").toString().replace("\\n","\n");
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("releasenote");
#endif
}

QString QLaunchFileDiscription::getMd5()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/md5","").toString();
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("md5");
#endif

}

QString QLaunchFileDiscription::getVersionNum()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/version","").toString();
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("version");
#endif


}

QString QLaunchFileDiscription::getHost()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/host","").toString();
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("host");
#endif

}

int QLaunchFileDiscription::getPort()
{
#if ANALYZE_STRATEGY==1
    return mSettings.value("update/port","").toInt();
#elif ANALYZE_STRATEGY ==2
    return mSettingMap.value("port").toInt();
#endif


}
