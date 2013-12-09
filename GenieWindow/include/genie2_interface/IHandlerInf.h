#ifndef IHANDLERINF_H
#define IHANDLERINF_H
#include <QtCore>
class IHandlerInf
{
public:
    virtual QVariant handler_command(int cmd,int uuid_idx,const QVariant &paras=QVariant())=0;
};

#endif // IHANDLERINF_H
