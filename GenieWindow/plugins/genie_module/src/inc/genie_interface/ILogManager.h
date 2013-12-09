#ifndef ILOGMANAGER_H
#define ILOGMANAGER_H
class QString;
struct QUuid;
class ILogManager
{
public:
    virtual void writeLog(QString s,QUuid uuid)=0;
};

#endif // ILOGMANAGER_H
