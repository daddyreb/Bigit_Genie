#ifndef QGENIEHANDLERINF_H
#define QGENIEHANDLERINF_H


class QGenieHandlerInf : public IHandlerInf
{
public:
    QGenieHandlerInf();
    QVariant handler_command(int cmd,int uuid_index,const QVariant &paras);
};

#endif // QGENIEHANDLERINF_H
