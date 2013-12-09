#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <QtCore>
#include <QtNetwork>
#include <QtXml>
#include <stdio.h>
#ifdef Q_OS_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include "SVTUtils.h"
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> /* inet_aton, sockaddr_in, etc */
#include <sys/ioctl.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/route.h>
    #ifndef SOCKET
    #define SOCKET int
    #endif
#endif
#include "../src/Singleton.h"
#include "../src/defines.h"
#include "../src/SvtReferences.h"
#include "SvtNetworkTool.h"
#include "../src/SvtQtUtils.h"
#include "../src/SvtReferences.h"
#include "../src/SvtAvlTree.h"
#include "../src/SvtList.h"
#include "../src/SvtCore.h"
#include "../src/SvtProgardThread.h"
#include "../src/SvtControlProxy.h"
#include "../src/SvtRunnables.h"
#include "../src/SvtNetbiosMethod.h"
#include "../src/SvtRdpMethod.h"
#include "../src/SvtPuttyMethod.h"
#include "../src/SvtAfpMethod.h"
#include "../src/SvtNetscanMethod.h"
#include "../src/SvtUpnpMethod.h"
#include "../src/netgear_info/SvtNetgearInfo.h"
#include "../src/SvtThreadMother.h"
//#include "../src/soap/qtsoap.h"
//#include "../src/soap/netgearsoapapi.h"
//#include "../src/soap/netgearsoapschedule.h"

#include "../src/SvtNetgearSpecificMethod.h"
//#include "../src/SvtNetgearSpecificHelper.h"
#include "../src/SvtSeedFactory.h"
#include "../src/SvtUnixProcessHelper.h"
#include "../src/SvtUpnpHelper.h"
#endif

