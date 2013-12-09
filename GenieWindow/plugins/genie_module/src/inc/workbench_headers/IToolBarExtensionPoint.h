#ifndef ITOOLBAREXTENSIONPOINT_H
#define ITOOLBAREXTENSIONPOINT_H

#include "IExtensionPoint.h"

class IToolBarExtensionPoint : public IExtensionPoint
{
public:
	virtual QString getLabel()
	{
		return "toolbars";
	}
};

REGISTER_EXTENSIONPOINT_CLASS("toolbars", IToolBarExtensionPoint);

#endif