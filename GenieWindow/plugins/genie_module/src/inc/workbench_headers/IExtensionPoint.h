#ifndef IEXTENSIONPOINT_H
#define IEXTENSIONPOINT_H

#include "IExtension.h" 

class IExtensionPoint
{
public:
	
	QMap<QString, IExtension*> mapExtension;

	virtual void registry(QString strID, IExtension* pExtension)
	{
		mapExtension.insert(strID, pExtension);
	}


	virtual QString getLabel() = 0;
	
	virtual QList<IExtension*> getExtensions()
	{
		return mapExtension.values();
	}

	
};

#endif