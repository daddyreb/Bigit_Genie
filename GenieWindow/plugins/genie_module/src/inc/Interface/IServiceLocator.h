#ifndef ISERVICELOCATOR_H
#define ISERVICELOCATOR_H

#include <QObject>
#include <QString>

class IServiceLocator
{
public:
	virtual QObject* getService(QString api)=0;

	virtual bool hasService(QString api)=0;

};

#endif