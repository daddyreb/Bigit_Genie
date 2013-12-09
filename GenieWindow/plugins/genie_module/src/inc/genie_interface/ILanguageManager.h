#ifndef ILANGUAGEMANAGER_H
#define ILANGUAGEMANAGER_H

#include <QString>
#include <QUuid>



//some interface to control the main frame ui
class ILanguageManager
{
public:
    virtual QString getText(int idx,QUuid uuid)=0;
	
};

#endif
