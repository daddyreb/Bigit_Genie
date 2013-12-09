#ifndef COMMON_H
#define COMMON_H

#include <QtCore>
#include <QtCore/qglobal.h>
#include <QtCore/QString>
#include <QDebug>

#if defined(DECL_EXPORT)
#  define EXPORT Q_DECL_EXPORT
#else
#  define EXPORT Q_DECL_IMPORT
#endif

#endif // COMMON_H
