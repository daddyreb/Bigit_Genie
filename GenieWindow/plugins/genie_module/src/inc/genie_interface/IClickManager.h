#ifndef ICLICKMANAGER_H
#define ICLICKMANAGER_H
#include <QString>
class IClickManager
{
public:
    virtual void click(const QString &key)=0;	//when button from plugin clicked,can this to let
    virtual void registMaxValue(const QString &key,int v)=0;
    virtual void rememberSearchStr(const QString &s)=0;
};
#endif
