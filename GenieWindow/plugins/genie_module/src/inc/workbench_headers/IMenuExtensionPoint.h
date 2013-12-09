#ifndef IMENUEXTENSIONPOINT_H
#define IMENUEXTENSIONPOINT_H

#include "IExtensionPoint.h"

class IMenuExtensionPoint : public IExtensionPoint
{
public:
	virtual QString getLabel()
	{
		return "menus";
	}
};

REGISTER_EXTENSIONPOINT_CLASS("menus", IMenuExtensionPoint);

#endif