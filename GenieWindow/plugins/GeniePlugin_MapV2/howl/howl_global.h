#ifndef HOWL_GLOBAL_H
#define HOWL_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef HOWL_LIB
# define HOWL_EXPORT Q_DECL_EXPORT
#else
# define HOWL_EXPORT Q_DECL_IMPORT
#endif

#endif // HOWL_GLOBAL_H
