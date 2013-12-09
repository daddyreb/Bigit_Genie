#ifndef IMOBILEMANAGER_H
#define IMOBILEMANAGER_H

class IMobileManager
{
public:
    virtual void * get(const QString &name)=0;
	virtual void set(const QString &name,void *)=0;
};

#endif 
