#ifndef ICHANNELMESSAGEPROCESSOR_H
#define ICHANNELMESSAGEPROCESSOR_H

#include <QStringList>

class IChannelMessageProcessor
{
public:
    virtual int process_message(int sender,int code,int flag,const QStringList &str_list)=0;
};

#endif // ICHANNELMESSAGEPROCESSOR_H
