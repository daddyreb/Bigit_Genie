#ifndef QGENIEINERTEXTMANAGER_H
#define QGENIEINERTEXTMANAGER_H


class inner_text_info
{
public:
    inner_text_info():curLan(ELT_ERROR){};
    QUuid uuid;
    QString texFile;
    E_LANGUAGE_TYPE curLan;
    QMap<int,QString> strMap;
    QUuid parentUuid;
};


class QGenieInerTextManager:public Singleton<QGenieInerTextManager>
{
public:
    QGenieInerTextManager();
    ~QGenieInerTextManager();
    void parsePara(QString para,QUuid parent);
    void unloadPi(QUuid uuid);
    QString getText(int idx,QUuid uuid,bool * b);
protected:
    QList<inner_text_info *> mTextInfo;
    QMap<QUuid,inner_text_info *> mUuidIdx;
};

#endif // QGENIEINERTEXTMANAGER_H
