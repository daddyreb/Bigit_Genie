#ifndef __PUTTY_FUNCS__
#define __PUTTY_FUNCS__
#include <QtCore>

enum PUTTY_OPEN_TYPE
{
    POT_SESSION
    ,POT_IP
    ,POT_NUM
};
bool puttyExeExists();

bool winscpExeExists();

void startPutty(int starttype,const QString &para);
void startWSCP(int starttype,const QString &para);

QVariant getPuttyHint(const QString &sIp,const QString &sName);
QVariant GetWinSCPHint(const QString &sIp,const QString &sName);
#endif
