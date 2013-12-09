#ifndef INTERNETDAEMONDEFS_H
#define INTERNETDAEMONDEFS_H

#ifdef Q_OS_WIN
#define INTERNET_DAEMON                 "InternetDaemon.exe"
#elif defined Q_OS_MACX
#define INTERNET_DAEMON                 "InternetDaemon"
#endif

#define INTERNET2DAEMON_CHANNEL         "FROM_INTERNET"
#define DAEMON2INTERNET_CHANNEL         "TO_INTERNET"
#define INTERNET_DAEMON_START_MAGIC     "START_INTERNET_DAEMON_{F9CFA74C-1589-48d9-B328-782F5B84099E}"
/////////////////////message code definition///////////////
#define DAEMON_UPDATE_INTERNET_STATE                 1
#define DAEMON_BEGIN_FIX                             2
#define DAEMON_CANCEL_FIX                            3
#define DAEMON_EXIT_PROCESS                          4
#define DAEMON_INTERNET_KEEPALIVE                    5
#define DAEMON_REPAIRTHREAD_EXIT                     6

#define INTERNET_RESTARROUTER_FINISHED               498
#define INTERNET_SHOW_RESTARTROUTER_DLG              499
#define INTERNET_SHOW_VNDLG                          500
#define INTERNET_SHOW_MESSAGEBOX                     501
#define INTERNET_SHOW_PLUGINCABLE_DLG                502
#define INTERNET_SHOW_WLANRADIOOFF_DLG               503


#define INTERNET_REALTIME_MESSAGE                    504
#define INTERNET_REALTIME_MESSAGE2                   505
#define INTERNET_REPAIR_COMPLETE                     506
//#define INTERNET_SHOW_MESSAGEBOX                     507
#define INTERNET_BEGIN_PCFLASH                       508
#define INTERNET_END_PCFLASH                         509
#define INTERNET_BEGIN_ROUTERFLASH                   510
#define INTERNET_END_ROUTERFLASH                     511
#define INTERNET_BEGIN_INTERNETFLASH                 512
#define INTERNET_END_INTERNETFLASH                   513
#define INTERNET_BEGIN_PCROUTERLINKFLASH             514
#define INTERNET_END_PCROUTERLINKFLASH               515
#define INTERNET_BEGIN_ROUTERINTERNETLINKFLASH       516
#define INTERNET_END_ROUTERINTERNETLINKFLASH         517
#define INTERNET_BEGIN_CONNECT                       518
#define INTERNET_END_CONNECT                         519
#define INTERNET_PCROUTERLINK_OK                     520
#define INTERNET_PCROUTERLINK_FAILED                 521
#define INTERNET_PCROUTERLINK_UNKNOWN                522
#define INTERNET_ROUTERINTERNETLINK_OK               523
#define INTERNET_ROUTERINTERNETLINK_FAILED           524
#define INTERNET_ROUTERINTERNETLINK_UNKNOWN          525
#define INTERNET_SHOW_CHOOSELOCACTION_DLG            526
/////////////////////message code definition//////////////

#endif // INTERNETDAEMONDEFS_H
