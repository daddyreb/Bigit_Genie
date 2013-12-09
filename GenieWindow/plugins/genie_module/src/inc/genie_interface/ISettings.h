#ifndef ISETTINGS_H
#define ISETTINGS_H
#include <QString>
#include <QIcon>
#include <QRect>
#ifdef INFO_WRITE_BACK
#include <QUuid>
#endif
#include "info_defs.h"
#include <QVariant>

class ISettings
{
public:
	virtual QVariant	value ( const QString & key, const QVariant & defaultValue = QVariant() ) =0;
    virtual void	setValue ( const QString & key, const QVariant & value )=0;

};
#endif
