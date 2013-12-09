#ifndef SVT_NETWORK_TOOL_H___
#define SVT_NETWORK_TOOL_H___


#if defined(SVT_NETWORKTOOL_LIBRARY)
#  define SVT_NETWORKTOOL_EXPORT Q_DECL_EXPORT
#else
#  define SVT_NETWORKTOOL_EXPORT Q_DECL_IMPORT
#endif

#include "SvtBlockedSocket.h"
#include "SvtNetwork.h"
#include "SvtHttp.h"
#include "SvtSoap.h"
#include "SvtHttpDownload.h"
#include "SvtFcml.h"
#endif
