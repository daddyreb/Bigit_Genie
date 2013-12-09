#ifndef QTNETSTAT_H
#define QTNETSTAT_H

#include "Globals.h"
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QGridLayout>
#include <QtCore/QTimer>

#include "stdio.h"
#include "windows.h"
#include <QtCore>
#include <QString>
#include <QStringList>
#include <QMap>
#include "IManagerHandler.h"

class QGenie_qtNetTraffic;

class QGenie_qtNetStat : public QWidget
{
    Q_OBJECT
public:
    QGenie_qtNetStat();

    QTreeWidget* qTree;
    QPushButton* btnRefresh;
    QPushButton* btnClose;

    QGenie_qtNetTraffic* qtNet;
    QTreeWidgetItem* headItem;

    int		m_nListNum;		//List¿Ø¼þµÄí—¿‚”µ
    DWORD  m_dNumEntries;	//TCPºÍUDPs
    //afx_msg void OnBnClickedGetlist();
    void GetAddressName(unsigned long raddr,char *rbuf, unsigned long rbuffsize,unsigned char isSrcAddress);

    void listAllConnections();
    void closeEvent ( QCloseEvent * event );
    void changeEvent(QEvent *e);
    void retranslateUi();

protected slots:
    void closeWidget(bool checked = false);
    void refreshWidget(bool checked = false);


};

#endif // QTNETSTAT_H
