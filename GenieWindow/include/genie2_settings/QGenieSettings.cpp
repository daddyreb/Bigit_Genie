#include "QGenieSettings.h"
#include "QGenieSettingHelper.h"



QGenieSettings::QGenieSettings()
    :mSettingHelper(new QGenieSettingHelper())

{
//    qDebug()<<"QGenieSettings::QGenieSettings";
}

QGenieSettings::~QGenieSettings()
{
    delete mSettingHelper;
}

QVariant QGenieSettings::value ( const QString & key, const QVariant & defaultValue )
{
    QVariant v;
    mMutex.lock();

    if(mMap.contains(key))
    {
       v=mMap.value(key,defaultValue);
    }
    else
    {
        v=mSettingHelper->value(key,defaultValue);

        mMap.insert(key,v);
    }
    mMutex.unlock();
    return v;
}

void QGenieSettings::setValue ( const QString & key, const QVariant & value )
{
    mMutex.lock();
    mMap.insert(key,value);
    mSettingHelper->setValue(key,value);
    mMutex.unlock();
}

void QGenieSettings::setValueToDisk(const QString & key, const QVariant & value )
{
    mSettingHelper->setValue(key,value);
}

void QGenieSettings::writeToDisk()
{
    mMutex.lock();
    QMapIterator<QString, QVariant> i(mMap);
     while (i.hasNext()) {
         i.next();
        mSettingHelper->setValue(i.key(),i.value());
     }
     mMutex.unlock();
}

void QGenieSettings::setingCmd(const QVariant &para_list)
{
    QList<QVariant> list=para_list.toList();
    if(list.size()>=2)
    {
        setValue(list[0].toString(),list[1]);
    }
}

