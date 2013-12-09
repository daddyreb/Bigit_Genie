#ifndef DIAGNOSEDLL_H
#define DIAGNOSEDLL_H

#ifndef DIAGNOSEDLL_API
#ifdef DIAGNOSEDLL_EXPORTS
#define DIAGNOSEDLL_API __declspec(dllexport)
#else
#define DIAGNOSEDLL_API __declspec(dllimport)
#endif
#endif

#include "NetcardApi.h"
#include "WiFiApi.h"
#include "CommonApi.h"

#endif
