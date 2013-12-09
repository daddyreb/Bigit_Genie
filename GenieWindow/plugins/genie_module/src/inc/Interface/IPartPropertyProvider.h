#ifndef IPARTPROPERTYPROVIDER_H
#define IPARTPROPERTYPROVIDER_H

#include "IWorkbenchPart.h"
#include <QImage>
#include <QString>
#include  "IPropertyListener.h";

class IEditorInput;

class IPartPropertyProvider
{
public :
	virtual void addPropertyListener(IWorkbenchPart* part, IPropertyListener* l)=0;
	virtual void removePropertyListener(IWorkbenchPart* part, IPropertyListener* l)=0;
	virtual QString getTitleToolTip()=0;
	virtual QImage* getTitleImage()=0;
	virtual QString getPartName()=0;
	virtual QString getTitle()=0;
	virtual QString getContentDescription()=0;
	virtual IEditorInput* getEditorInput()=0;
	virtual bool isDirty()=0;
};
#endif 