#ifndef MYPING_H
#define MYPING_H

#include "common.h"

class EXPORT Ping
{
public:
    Ping(){};
    Ping(QString &u):url(u){};
    void setURL(QString &url);
    bool runping();
private:
    QString url;
};

#endif//MYPING_H
