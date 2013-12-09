#ifndef LANGUAGE_I18N_H
#define LANGUAGE_I18N_H

#include "genie2_interface.h"
#include "language_Map.h"

#ifdef RUN_IN_MAINFRAME
#define I18N_TEXT(object, method, id) ((object)->set##method(GENIE2_GET_TEXT(id)))
#else
#define I18N_TEXT(object, method, id)
#endif

#endif // LANGUAGE_I18N_H
