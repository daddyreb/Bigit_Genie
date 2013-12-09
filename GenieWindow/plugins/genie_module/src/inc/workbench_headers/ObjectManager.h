#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <QString>
#include <QMap>

#include  "IExtensionPoint.h"

class ObjectManager
{
public:
	ObjectManager();
	virtual ~ObjectManager();

	QMap<QString, IExtensionPoint*> mapExtensionPoint;

	virtual void regiter(QString strID, IExtensionPoint* pExtensionPoint);

	IExtensionPoint* getExtensionPoint(QString strID)
	{
		return mapExtensionPoint.value(strID);
	}

};


#endif