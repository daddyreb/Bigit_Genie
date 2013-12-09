#include "stable.h"

QGenieInerTextManager::QGenieInerTextManager()
{
}
QGenieInerTextManager::~QGenieInerTextManager()
{
    while(!mTextInfo.isEmpty())
    {
        inner_text_info *iti=mTextInfo.takeFirst();
        SAFE_DELETE(iti);
    }
}

void QGenieInerTextManager::parsePara(QString para, QUuid parent)
{
    QStringList slst=para.split(";");
    foreach(QString line,slst)
    {
        QStringList sublst=line.split(":");

        if(sublst.size()>=2)
        {
            inner_text_info *iti=new inner_text_info();
            iti->uuid=QUuid(sublst.at(0));
            iti->texFile=sublst.at(1);
            iti->curLan=ELT_ERROR;
            iti->parentUuid=parent;
            mTextInfo.append(iti);
            mUuidIdx.insert(iti->uuid,iti);
        }
    }
}

QString QGenieInerTextManager::getText(int idx, QUuid uuid, bool *b)
{
    inner_text_info *iti=mUuidIdx.value(uuid);
    if(!iti)
    {
        if(b) *b=false;
        return QString("inner[%1]").arg(QString::number(idx));
    }
    QGenieLanguageManager *lanmanager=QGenieLanguageManager::GetSingletonPtr();
    ASSERT(lanmanager);
    E_LANGUAGE_TYPE curlan=lanmanager->mCurrentType;
    if(iti->curLan == ELT_ERROR || iti->curLan != curlan)
    {
        lanmanager->prepareMap(iti->texFile,(iti->strMap),curlan);
        iti->curLan=curlan;
    }

    QString dft=QString("inner[%1]").arg(QString::number(idx));
    return iti->strMap.value(idx,dft);

}

void QGenieInerTextManager::unloadPi(QUuid uuid)
{
    foreach(inner_text_info *iti,mTextInfo)
    {
        if(iti->parentUuid==uuid)
        {
            iti->curLan=ELT_ERROR;
            iti->strMap.clear();
        }
    }
}
