#ifndef ISVPSHELL_H
#define ISVPSHELL_H

#include "IService.h"
#include <QString>

using namespace svp::framework;

class ISvpShell:public IService
{
public:
	virtual void setProductsMenuTitle(const QString& title)=0;
};
#endif