#include "QGenieNetTrafficGraph.h"
#include <QHostAddress>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include "GeniePlugin_Internet.h"
#include <numeric>
#include <genie_daemon_intf.h>

//add for porting to new interfaces by duanlin.yi
#include "genie2_interface.h"
#define GET_SETTING_BOOL(key) (GENIE2_GET_SETTING(key).toBool())
#define GET_SETTING_INT(key) (GENIE2_GET_SETTING(key).toInt())
#define SET_SETTING_VALUE(key,value) GENIE2_SET_SETTING(key,value)
//end

//add for statistics by duanlin.yi
#include "GeniePlugin_Internet.h"
//end

#define DETAILCHECK_INTERVAL 200 // 200 ms
#define DETAILSCHECK_KEY "Internet/TrafficFlow"

#define __INTERNAL_NM_SIMPLE_VIEW 1

QString addressTextIPv4(quint32 addr, quint16 port)
{
    return QString("%1:%2").arg(QHostAddress(addr).toString()).arg(port);
}

QString speedText(double speed)
{
    QString text;
    if (speed >= 1073741824) {
        text = QString("%1 GB/s").arg(speed / 1073741824, 0, 'f', 2);
    } else if (speed >= 1048576) {
        text = QString("%1 MB/s").arg(speed / 1048576, 0, 'f', 2);
    } else if (speed >= 1024) {
        text = QString("%1 KB/s").arg(speed / 1024, 0, 'f', 2);
    } else if (speed != 0) {
        text = QString("%1 B/s").arg(speed, 0, 'f', 2);
    }
    return text;
}

QString sizeText(quint64 size)
{
    if (size >= 0x10000000000LL) {
        return QString("%1 TB").arg(static_cast<double>(size) / (0x10000000000LL), 0, 'f', 2);
    } else if (size >= 0x40000000LL) {
        return QString("%1 GB").arg(static_cast<double>(size) / 0x40000000, 0, 'f', 2);
    } else if (size >= 0x100000) {
        return QString("%1 MB").arg(static_cast<double>(size) / 0x100000, 0, 'f', 2);
    } else if (size >= 0x400) {
        return QString("%1 KB").arg(static_cast<double>(size) / 0x400, 0, 'f', 2);
    } else if (size != 0) {
        return QString("%1 B").arg(size);
    } else {
        return QString("0");
    }
}

BaseNetMonitorItem::BaseNetMonitorItem()
{
}

BaseNetMonitorItem::BaseNetMonitorItem(const QString& text)
    : QStandardItem(text)
{
}

void BaseNetMonitorItem::notifyChanged()
{
    emitDataChanged();
}

void BaseNetMonitorItem::updateFullRow()
{
    QStandardItem *parentItem = parent();
    if (parentItem) {
        int cc = parentItem->columnCount();
        for (int i = 0; i < cc; i++) {
            QStandardItem *item = parentItem->child(row(), i);
            if (item) {
                static_cast<BaseNetMonitorItem*>(item)->notifyChanged();
            }
        }
    } else {
        QStandardItemModel *parentModel = model();
        int cc = parentModel->columnCount();
        for (int i = 0; i < cc; i++) {
            QStandardItem *item = parentModel->item(row(), i);
            if (item) {
                static_cast<BaseNetMonitorItem*>(item)->notifyChanged();
            }
        }
    }
}

NetMonitorSpeedItem::NetMonitorSpeedItem(double speed)
    : m_speed(speed)
{
    setTextAlignment(Qt::AlignRight);
    setText(speedText(m_speed));
}

void NetMonitorSpeedItem::setSpeed(double speed)
{
    if (m_speed != speed) {
        m_speed = speed;
        setText(speedText(m_speed));
    }
}

bool NetMonitorSpeedItem::operator<(const QStandardItem & other) const
{
    const NetMonitorSpeedItem *otherItem = dynamic_cast<const NetMonitorSpeedItem*>(&other);
    if (otherItem) {
        return m_speed < otherItem->m_speed;
    }
    return BaseNetMonitorItem::operator<(other);
}

NetMonitorSizeItem::NetMonitorSizeItem(quint64 size)
    : m_size(size)
{
    setTextAlignment(Qt::AlignRight);
    setText(sizeText(m_size));
}

void NetMonitorSizeItem::setSize(quint64 size)
{
    if (m_size != size) {
        m_size = size;
        setText(sizeText(m_size));
    }
}

bool NetMonitorSizeItem::operator<(const QStandardItem& other) const
{
    const NetMonitorSizeItem *otherItem = dynamic_cast<const NetMonitorSizeItem*>(&other);
    if (otherItem) {
        return m_size < otherItem->m_size;
    }
    return BaseNetMonitorItem::operator<(other);
}

NetMonitorNumericItem::NetMonitorNumericItem(int value)
    : m_value(value)
{
    setTextAlignment(Qt::AlignRight);
    setText(QString("%1").arg(m_value));
}

void NetMonitorNumericItem::setValue(int value)
{
    if (m_value != value) {
        m_value = value;
        setText(QString("%1").arg(m_value));
    }
}

bool NetMonitorNumericItem::operator<(const QStandardItem& other) const
{
    const NetMonitorNumericItem *otherItem = dynamic_cast<const NetMonitorNumericItem*>(&other);
    if (otherItem) {
        return m_value < otherItem->m_value;
    }
    return BaseNetMonitorItem::operator<(other);
}

NetMonitorItem::NetMonitorItem()
    : deleteFlag(false), newFlag(true), deleteTimer(NULL), newTimer(NULL)
{
}

void NetMonitorItem::scheduleDelete(QTimer *timer)
{
    deleteTimer = timer;
    updateFullRow();
}

void NetMonitorItem::scheduleNew(QTimer *timer)
{
    newTimer = timer;
}

void NetMonitorItem::flagAsNormal()
{
    newFlag = false;
    newTimer = NULL;
    updateFullRow();
}

NetMonitorModel::NetMonitorModel(QObject *parent)
    : QStandardItemModel(parent), m_sortColumn(-1)
{
    retranslateUi();
}

NetMonitorModel::~NetMonitorModel()
{
}

void NetMonitorModel::resort()
{
    if (m_sortColumn != -1) {
        sort(m_sortColumn, m_sortOrder);
    }
}

void NetMonitorModel::sort(int column, Qt::SortOrder order)
{
    m_sortColumn = column;
    m_sortOrder = order;
    QStandardItemModel::sort(column, order);
}

void NetMonitorModel::retranslateUi()
{
    QStringList headers;
    headers << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_APP_NAME)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_LOCAL_ADDRESS)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_REMOTE_ADDRESS)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_UPLOAD_SPEED)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_DOWNLOAD_SPEED)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_TOTAL_UPLOAD)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_TOTAL_DOWNLOAD)
            << GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_COL_NB_CONN);

    setHorizontalHeaderLabels(headers);

    for (int i = 0; i < headers.length(); i++) {
        QStandardItem *headerItem = horizontalHeaderItem(i);
        if (headerItem) {
            headerItem->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void NetMonitorModel::deferredDeleteItems()
{
    QTimer *timer = static_cast<QTimer*>(sender());
    timer->deleteLater();

    int appCount = rowCount();
    for (int i = appCount - 1; i >= 0; i--) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        if (pAppItem->deleteTimer == timer) {
            removeRow(i);
        } else {
            int detailCount = pAppItem->rowCount();
            for (int j = detailCount - 1; j >= 0; j--) {
                NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(j));
                if (pDetailItem->deleteTimer == timer) {
                    pAppItem->removeRow(j);
                }
            }
        }
    }
}

void NetMonitorModel::deferredNewItems()
{
    QTimer *timer = static_cast<QTimer*>(sender());
    timer->deleteLater();

    int appCount = rowCount();
    for (int i = appCount - 1; i >= 0; i--) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        if (pAppItem->newTimer == timer) {
            pAppItem->flagAsNormal();
        } else {
            int detailCount = pAppItem->rowCount();
            for (int j = detailCount - 1; j >= 0; j--) {
                NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(j));
                if (pDetailItem->newTimer == timer) {
                    pDetailItem->flagAsNormal();
                }
            }
        }
    }
}

void NetMonitorModel::beginUpdate()
{
    int appCount = rowCount();
    for (int i = 0; i < appCount; i++) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        pAppItem->deleteFlag = true;
        int detailCount = pAppItem->rowCount();
        for (int j = 0; j < detailCount; j++) {
            NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(j));
            pDetailItem->deleteFlag = true;
        }
    }
}

void NetMonitorModel::endUpdate()
{
    QTimer *deleteTimer = new QTimer(this);
    bool deleteEmpty = true;

    int appCount = rowCount();
    for (int i = 0; i < appCount; i++) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        if (pAppItem->deleteFlag && pAppItem->deleteTimer == NULL) {
            deleteEmpty = false;
            pAppItem->scheduleDelete(deleteTimer);
        } else {
            int detailCount = pAppItem->rowCount();
            for (int j = 0; j < detailCount; j++) {
                NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(j));
                if (pDetailItem->deleteFlag && pDetailItem->deleteTimer == NULL) {
                    deleteEmpty = false;
                    pDetailItem->scheduleDelete(deleteTimer);
                }
            }
        }
    }

    if (deleteEmpty) {
        delete deleteTimer;
    } else {
        connect(deleteTimer, SIGNAL(timeout()), SLOT(deferredDeleteItems()));
        deleteTimer->setSingleShot(true);
        deleteTimer->start(2000);
    }

    QTimer *newTimer = new QTimer(this);
    bool newEmpty = true;

    appCount = rowCount();
    for (int i = 0; i < appCount; i++) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        if (pAppItem->newFlag && pAppItem->newTimer == NULL) {
            newEmpty = false;
            pAppItem->scheduleNew(newTimer);
        } else {
            int detailCount = pAppItem->rowCount();
            for (int j = 0; j < detailCount; j++) {
                NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(j));
                if (pDetailItem->newFlag && pDetailItem->newTimer == NULL) {
                    newEmpty = false;
                    pDetailItem->scheduleNew(newTimer);
                }
            }
        }
    }

    if (newEmpty) {
        delete newTimer;
    } else {
        connect(newTimer, SIGNAL(timeout()), SLOT(deferredNewItems()));
        newTimer->setSingleShot(true);
        newTimer->start(2000);
    }
}

void NetMonitorModel::updateAppData(const NM_APP_DETAIL *appData, const quint8 *buffer0, NetMonitor *monitor)
{
    const NM_SUMMARY *summary = reinterpret_cast<const NM_SUMMARY*>(buffer0);
    int appCount = rowCount();
    for (int i = 0; i < appCount; i++) {
        NetMonitorAppItem *pAppItem = static_cast<NetMonitorAppItem*>(item(i));
        if (pAppItem->deleteTimer == NULL && pAppItem->pid == appData->pid) {
            pAppItem->deleteFlag = false;
            // TODO:
            static_cast<NetMonitorSpeedItem*>(item(i, 3))->setSpeed(appData->deltaOut * 1000.0 / summary->deltaTime);
            static_cast<NetMonitorSpeedItem*>(item(i, 4))->setSpeed(appData->deltaIn * 1000.0 / summary->deltaTime);
            static_cast<NetMonitorSizeItem*>(item(i, 5))->setSize(appData->totalOut);
            static_cast<NetMonitorSizeItem*>(item(i, 6))->setSize(appData->totalIn);
            static_cast<NetMonitorNumericItem*>(item(i, 7))->setValue(appData->tcpCount + appData->udpCount);

            updateDetailRows(pAppItem, appData, buffer0);
            return;
        }
    }

    QList<QStandardItem*> rowItems;

    QString appName;
    if (appData->moduleNameLength > 0) {
        appName = QString::fromUtf8(reinterpret_cast<const char*>(buffer0 + appData->moduleNameOffset), appData->moduleNameLength);
    }

    if (/*appName.toLower() == "netgeargenie.exe" || */appName.isEmpty()) {
        return;
    }

    QString appPath;
    if (appData->moduleFullPathLength > 0) {
        appPath = QString::fromUtf8(reinterpret_cast<const char*>(buffer0 + appData->moduleFullPathOffset), appData->moduleFullPathLength);
    }

    NetMonitorAppItem *pAppItem = new NetMonitorAppItem();
    pAppItem->pid = appData->pid;
    pAppItem->fullPath = appPath;

#ifdef __INTERNAL_NM_SIMPLE_VIEW
    pAppItem->setText(QString("%1").arg(appName));
#else
    pAppItem->setText(QString("%1:%2").arg(appName).arg(pAppItem->pid));
#endif // __INTERNAL_NM_SIMPLE_VIEW
    pAppItem->setIcon(monitor->getAppIcon(appData->pid, appPath));

    rowItems << pAppItem;
    rowItems << new BaseNetMonitorItem();
    rowItems << new BaseNetMonitorItem();
    rowItems << new NetMonitorSpeedItem(appData->deltaOut * 1000.0 / summary->deltaTime);
    rowItems << new NetMonitorSpeedItem(appData->deltaIn * 1000.0 / summary->deltaTime);
    rowItems << new NetMonitorSizeItem(appData->totalOut);
    rowItems << new NetMonitorSizeItem(appData->totalIn);
    rowItems << new NetMonitorNumericItem(appData->tcpCount + appData->udpCount);

    appendRow(rowItems);
    updateDetailRows(pAppItem, appData, buffer0);
}

void NetMonitorModel::updateDetailRows(NetMonitorAppItem *pAppItem, const NM_APP_DETAIL *appData, const quint8 *buffer0)
{
#ifdef __INTERNAL_NM_SIMPLE_VIEW
    return;
#else
    const NM_CONNECTION_TCP *tcpData = reinterpret_cast<const NM_CONNECTION_TCP*>(buffer0 + appData->tcpOffset);
    for (quint32 i = 0; i < appData->tcpCount; i++, ++tcpData) {
        updateTcpRow(pAppItem, tcpData, buffer0);
    }
    const NM_CONNECTION_UDP *udpData = reinterpret_cast<const NM_CONNECTION_UDP*>(buffer0 + appData->udpOffset);
    for (quint32 i = 0; i < appData->udpCount; i++, ++udpData) {
        updateUdpRow(pAppItem, udpData, buffer0);
    }
#endif // __INTERNAL_NM_SIMPLE_VIEW
}

void NetMonitorModel::updateTcpRow(NetMonitorAppItem *pAppItem, const NM_CONNECTION_TCP *detailData, const quint8 *buffer0)
{
    const NM_SUMMARY *summary = reinterpret_cast<const NM_SUMMARY*>(buffer0);
    int detailCount = pAppItem->rowCount();
    for (int i = 0; i < detailCount; i++) {
        NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(i));
        if (pDetailItem->localAddr == detailData->localAddr && pDetailItem->localPort == detailData->localPort &&
            pDetailItem->remoteAddr == detailData->remoteAddr && pDetailItem->remotePort == detailData->remotePort)
        {
            pDetailItem->deleteFlag = false;
            pAppItem->child(i, 3)->setText(speedText(detailData->deltaOut * 1000.0 / summary->deltaTime));
            pAppItem->child(i, 4)->setText(speedText(detailData->deltaIn * 1000.0 / summary->deltaTime));
            pAppItem->child(i, 5)->setText(sizeText(detailData->totalOut));
            pAppItem->child(i, 6)->setText(sizeText(detailData->totalIn));
            return;
        }
    }

    QList<QStandardItem*> rowItems;

    NetMonitorDetailItem *pDetailItem = new NetMonitorDetailItem();
    pDetailItem->localAddr = detailData->localAddr;
    pDetailItem->localPort = detailData->localPort;
    pDetailItem->remoteAddr = detailData->remoteAddr;
    pDetailItem->remotePort = detailData->remotePort;
    pDetailItem->setText("[TCP]");

    rowItems << pDetailItem;
    rowItems << new BaseNetMonitorItem(addressTextIPv4(detailData->localAddr, detailData->localPort));
    rowItems << new BaseNetMonitorItem(addressTextIPv4(detailData->remoteAddr, detailData->remotePort));
    rowItems << new BaseNetMonitorItem(speedText(detailData->deltaOut * 1000.0 / summary->deltaTime));
    rowItems << new BaseNetMonitorItem(speedText(detailData->deltaIn * 1000.0 / summary->deltaTime));
    rowItems << new BaseNetMonitorItem(sizeText(detailData->totalOut));
    rowItems << new BaseNetMonitorItem(sizeText(detailData->totalIn));
    rowItems << new BaseNetMonitorItem();

    pAppItem->appendRow(rowItems);
}

void NetMonitorModel::updateUdpRow(NetMonitorAppItem *pAppItem, const NM_CONNECTION_UDP *detailData, const quint8 *buffer0)
{
    const NM_SUMMARY *summary = reinterpret_cast<const NM_SUMMARY*>(buffer0);
    int detailCount = pAppItem->rowCount();
    for (int i = 0; i < detailCount; i++) {
        NetMonitorDetailItem *pDetailItem = static_cast<NetMonitorDetailItem*>(pAppItem->child(i));
        if (pDetailItem->localAddr == detailData->localAddr && pDetailItem->localPort == detailData->localPort &&
            pDetailItem->remoteAddr == detailData->remoteAddr && pDetailItem->remotePort == detailData->remotePort)
        {
            pDetailItem->deleteFlag = false;
            pAppItem->child(i, 3)->setText(speedText(detailData->deltaOut * 1000.0 / summary->deltaTime));
            pAppItem->child(i, 4)->setText(speedText(detailData->deltaIn * 1000.0 / summary->deltaTime));
            pAppItem->child(i, 5)->setText(sizeText(detailData->totalOut));
            pAppItem->child(i, 6)->setText(sizeText(detailData->totalIn));
            return;
        }
    }

    QList<QStandardItem*> rowItems;

    NetMonitorDetailItem *pDetailItem = new NetMonitorDetailItem();
    pDetailItem->localAddr = detailData->localAddr;
    pDetailItem->localPort = detailData->localPort;
    pDetailItem->remoteAddr = detailData->remoteAddr;
    pDetailItem->remotePort = detailData->remotePort;
    pDetailItem->setText("[UDP]");

    rowItems << pDetailItem;
    rowItems << new BaseNetMonitorItem(addressTextIPv4(detailData->localAddr, detailData->localPort));
    rowItems << new BaseNetMonitorItem(addressTextIPv4(detailData->remoteAddr, detailData->remotePort));
    rowItems << new BaseNetMonitorItem(speedText(detailData->deltaOut * 1000.0 / summary->deltaTime));
    rowItems << new BaseNetMonitorItem(speedText(detailData->deltaIn * 1000.0 / summary->deltaTime));
    rowItems << new BaseNetMonitorItem(sizeText(detailData->totalOut));
    rowItems << new BaseNetMonitorItem(sizeText(detailData->totalIn));
    rowItems << new BaseNetMonitorItem();

    pAppItem->appendRow(rowItems);
}

NetMonitorItemDelegate::NetMonitorItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_deleteBG(QColor(242, 242, 242))
    , m_newBG(QColor(159, 157, 230))
{
}

void NetMonitorItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    const QStandardItemModel *model = qobject_cast<const QStandardItemModel*>(index.model());
    QStandardItem *item = model->itemFromIndex(index);
    QStandardItem *parentItem = item->parent();
    NetMonitorItem *rowItem;
    if (parentItem) {
        rowItem = static_cast<NetMonitorItem*>(parentItem->child(item->row()));
    } else {
        rowItem = static_cast<NetMonitorItem*>(model->item(item->row()));
    }
    if (rowItem->deleteFlag) {
        opt.backgroundBrush = m_deleteBG;
    } else if (rowItem->newFlag) {
        opt.backgroundBrush = m_newBG;
    }

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
}

NetMonitorDisplay::NetMonitorDisplay(QWidget *parent)
    : QTreeView(parent)
{
    connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(refresh()));
    setModel(&m_model);
    setItemDelegate(&m_delegate);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSortingEnabled(true);
#ifdef __INTERNAL_NM_SIMPLE_VIEW
    setColumnWidth(0, 160);
    setColumnWidth(3, 100);
    setColumnWidth(4, 120);
    setColumnWidth(5, 100);
    setColumnWidth(6, 100);
    setColumnHidden(1, true);
    setColumnHidden(2, true);
    setColumnHidden(7, true);
    setIndentation(0);
    sortByColumn(6, Qt::DescendingOrder);
#endif // __INTERNAL_NM_SIMPLE_VIEW
}

NetMonitorDisplay::~NetMonitorDisplay()
{
}

void NetMonitorDisplay::retranslateUi()
{
    m_model.retranslateUi();
}

void NetMonitorDisplay::setMonitor(NetMonitor *monitor)
{
    m_monitor = monitor;
    if (m_monitor) {
        m_refreshTimer.start(1000);
    } else {
        m_refreshTimer.stop();
    }
}

void NetMonitorDisplay::refresh()
{
    if (m_monitor) {
        std::vector<quint8> buffer;
        m_monitor->snapshot(buffer);
        if (!buffer.empty()) {
            parseAndUpdate(&buffer[0]);
        }
    }
}

void NetMonitorDisplay::parseAndUpdate(const quint8 *buffer)
{
    const NM_SUMMARY *summary = reinterpret_cast<const NM_SUMMARY*>(buffer);
    if (summary->version == 0x00010001) {
        m_model.beginUpdate();

        const NM_APP_DETAIL *appData = reinterpret_cast<const NM_APP_DETAIL*>(buffer + summary->appDetailOffset);
        for (quint32 i = 0; i < summary->appDetailCount; i++, ++appData) {
            m_model.updateAppData(appData, buffer, m_monitor);
        }

        m_model.endUpdate();
        m_model.resort();
    }
}

#ifdef Q_OS_WIN32

#include <windows.h>
#include <shellapi.h>

bool killProcess_platform(quint32 pid, const QString& path)
{
    int retval = GDF::killProcess(pid);
    return retval == GDF::Status_Ok ? true : false;
}

bool navigateToProcessFolder_platform(quint32 pid, const QString& path)
{
    std::wstring param(L"/e, /select, \"");
    param += path.toStdWString();
    param.push_back(L'"');

    BOOL retval;

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    {
        retval = reinterpret_cast<int>(ShellExecuteW(NULL, L"open", L"explorer.exe", param.c_str(), NULL, SW_SHOW)) > 32 ? TRUE : FALSE;
    }
    CoUninitialize();
    return retval ? true : false;
}

bool openProcessPropertyPage_platform(quint32 pid, const QString& path)
{
    std::wstring wPath = path.toStdWString();

    SHELLEXECUTEINFOW execInfo;
    memset(&execInfo, 0, sizeof(execInfo));
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_UNICODE;
    execInfo.lpVerb = L"properties";
    execInfo.lpFile = wPath.c_str();
    execInfo.nShow = SW_NORMAL;

    BOOL retval;

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    {
        retval = ShellExecuteExW(&execInfo);
    }
    CoUninitialize();
    return retval ? true : false;
}

#endif

void NetMonitorDisplay::contextMenuEvent(QContextMenuEvent *ev)
{
#ifdef Q_OS_WIN32

    QModelIndex currSel = selectionModel()->currentIndex();
    if (!currSel.isValid()) {
        return;
    }

    NetMonitorAppItem *appItem = dynamic_cast<NetMonitorAppItem*>(m_model.itemFromIndex(currSel));
    if (!appItem) {
        return;
    }

    // build context menu
    QMenu contextMenu;

    QAction *actKill = NULL;
    QAction *actFind = NULL;
    QAction *actProp = NULL;

    if (!appItem->fullPath.isEmpty() && appItem->pid != GetCurrentProcessId()) {
        actKill = contextMenu.addAction(QIcon(":images/kill.png"), GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_CMENU_KILL_PROCESS));
        actFind = contextMenu.addAction(QIcon(":images/find.png"), GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_CMENU_FIND_PROCESS));
        actProp = contextMenu.addAction(QIcon(":images/property.png"), GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_CMENU_PROP_PAGE));
    }

    if (contextMenu.isEmpty()) {
        return;
    }

    QAction *actionSelected = contextMenu.exec(ev->globalPos());
    if (actionSelected == NULL) {
        // nothing to do
    } else if (actionSelected == actKill) {
//        QString prompt;
//        prompt += GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_CMENU_KILL_CONFIRM_TITLE);
//        prompt.push_back(QChar(0));
//        prompt += GeniePlugin_Internet::translateText(L_INTERNET_NETMON_DETAIL_CMENU_KILL_CONFIRM);

        QVariantList v;
		v<<GENIE2_GET_TEXT(L_INTERNET_NETMON_DETAIL_CMENU_KILL_CONFIRM_TITLE)
                   <<GENIE2_GET_TEXT(L_INTERNET_NETMON_DETAIL_CMENU_KILL_CONFIRM);
        if(GENIE2_UI_COMMAND(UICMD_DIALOG_MODAL,v).toBool())
        {
            killProcess_platform(appItem->pid, appItem->fullPath);
        }
        /*if (0 == g_uimanager->command(UI_CMD_DIALOG, prompt)) {
                                        }*/
    } else if (actionSelected == actFind) {
        navigateToProcessFolder_platform(appItem->pid, appItem->fullPath);
    } else if (actionSelected == actProp) {
        openProcessPropertyPage_platform(appItem->pid, appItem->fullPath);
    }

#endif
}

NetTrafficGraph::NetTrafficGraph(QWidget *parent)
    : QWidget(parent), m_maxValue(0), m_drawIncoming(true), m_drawOutcoming(true)
    , m_trafficLimit(0), m_trafficFlag(false)
{
    connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(refresh()));
}

NetTrafficGraph::~NetTrafficGraph()
{
}

void NetTrafficGraph::setMonitor(NetMonitor *monitor)
{
    m_monitor = monitor;
    if (m_monitor) {
        m_refreshTimer.start(1000);
    } else {
        m_refreshTimer.stop();
    }
}

void NetTrafficGraph::setShowOutcoming(bool show)
{
    if (m_drawOutcoming != show) {
        m_drawOutcoming = show;
        update();
    }
}

bool NetTrafficGraph::showOutcoming() const
{
    return m_drawOutcoming;
}

void NetTrafficGraph::setShowIncoming(bool show)
{
    if (m_drawIncoming != show) {
        m_drawIncoming = show;
        update();
    }
}

bool NetTrafficGraph::showIncoming() const
{
    return m_drawIncoming;
}

void NetTrafficGraph::setTrafficThreshold(quint32 threshold)
{
    m_trafficLimit = threshold;
    if (threshold == 0 && m_trafficFlag) {
        m_avg.clear();
        m_trafficFlag = false;
        emit trafficAlert(false);
    }
}

void NetTrafficGraph::refresh()
{
    if (m_monitor) {
        std::vector<quint8> buffer;
        m_monitor->snapshot(buffer);
        if (!buffer.empty()) {
            parseAndUpdate(&buffer[0]);
        } else {
            inputData(0, 0);
            update();
        }
    }
}

void NetTrafficGraph::parseAndUpdate(const quint8 *buffer)
{
    const NM_SUMMARY *summary = reinterpret_cast<const NM_SUMMARY*>(buffer);
    if (summary->version == 0x00010001) {
        inputData(summary->deltaTcpOut + summary->deltaUdpOut, summary->deltaTcpIn + summary->deltaUdpIn);
        update();
    }
}

static const int NUM_TRAFFIC_SAMPLES = 10;
static const int NUM_TRAFFIC_LIMIT = 6;

void NetTrafficGraph::inputData(quint64 deltaOut, quint64 deltaIn)
{
    if (deltaIn < 1320 && deltaOut < 1320) {
        deltaIn = 0;
        deltaOut = 0;
    }
    History item;
    item.in = deltaIn;
    item.out = deltaOut;
    m_history.push_back(item);
    emit newValue(deltaOut, deltaIn);
    if (m_trafficLimit != 0) {
        quint32 traffic = deltaIn + deltaOut;
        m_avg.push_back(traffic);
        if (m_avg.size() > NUM_TRAFFIC_SAMPLES) {
            m_avg.pop_front();
        }

        if (m_avg.size() == NUM_TRAFFIC_SAMPLES) {

            int cnt = 0;

            {
                std::deque<quint32>::const_iterator it = m_avg.begin();
                std::deque<quint32>::const_iterator ie = m_avg.end();
                for (; it != ie; ++it) {
                    if (m_trafficFlag) {
                        if (*it < m_trafficLimit) {
                            ++cnt;
                        }
                    } else {
                        if (*it >= m_trafficLimit) {
                            ++cnt;
                        }
                    }
                }
            }

            if (cnt >= NUM_TRAFFIC_LIMIT) {
                m_trafficFlag = !m_trafficFlag;
                m_avg.clear();
                emit trafficAlert(m_trafficFlag);
            }
            /*
//			quint64 total = 0;
//			total = std::accumulate(m_avg.begin(), m_avg.end(), 0);
//			quint64 threshold = m_trafficLimit * NUM_TRAFFIC_LIMIT;

			if (m_trafficFlag) {
				if (total < threshold) {
					m_trafficFlag = false;
					m_avg.clear();
					emit trafficAlert(false);
				}
			} else {
				if (total >= threshold) {
					m_trafficFlag = true;
					m_avg.clear();
					emit trafficAlert(true);
				}
			}*/
        }
    }
}

void NetTrafficGraph::paintEvent(QPaintEvent *evt)
{
    QPainter painter(this);
    size_t columns = size().width() / 2;
    if (columns > HISTORY_CAPACITY) {
        columns = HISTORY_CAPACITY;
    }
    drawBars(&painter, columns);
}

void NetTrafficGraph::drawBars(QPainter *painter, size_t columns)
{
    //Q_ASSERT(columns <= HISTORY_CAPACITY);
    size_t cnt = m_history.size();
    //qDebug("cc %d ? %d", cnt, columns);
    if (columns < cnt) {
        cnt = columns;
    }
    size_t startIndex = m_history.size() - cnt;
    size_t endIndex = startIndex + cnt;
    size_t offset = columns - cnt;

    //qDebug("dd %d %d %d", startIndex, endIndex, offset);

    quint32 maxValue = 0;

    if (m_drawIncoming) {
        for (size_t i = startIndex; i < endIndex; i++) {
            const History *history = m_history.at(i);
            if (maxValue < history->in) {
                maxValue = history->in;
            }
        }
    }

    if (m_drawOutcoming) {
        for (size_t i = startIndex; i < endIndex; i++) {
            const History *history = m_history.at(i);
            if (maxValue < history->out) {
                maxValue = history->out;
            }
        }
    }

    QRect vp = painter->viewport();

    painter->fillRect(vp, QBrush(QColor(242, 242, 242)));

    painter->setWindow(0, 0, columns * 10, vp.height());

    bool rtl = QApplication::isRightToLeft();

    if (maxValue > 0) {

        if (m_drawIncoming) {
            QBrush brIn(QColor(0, 113, 215));

            for (size_t i = startIndex; i < endIndex; i++) {
                const History *history = m_history.at(i);
                int barHeight = history->in * (vp.height() - 30) / maxValue;
                if (barHeight == 1) {
                    barHeight = 2;
                }
                if (rtl) {
                    QRect rc((columns - 1 - (offset + i - startIndex)) * 10, vp.height() - barHeight, 10, barHeight);
                    painter->fillRect(rc, brIn);
                } else {
                    QRect rc((offset + i - startIndex) * 10, vp.height() - barHeight, 10, barHeight);
                    painter->fillRect(rc, brIn);
                }
            }
        }

        if (m_drawOutcoming) {
            QBrush brOut(QColor(0, 153, 0));

            for (size_t i = startIndex; i < endIndex; i++) {
                const History *history = m_history.at(i);
                int barHeight = history->out * (vp.height() - 30) / maxValue;
                if (barHeight == 1) {
                    barHeight = 2;
                }
                if (rtl) {
                    QRect rc((columns - 1 - (offset + i - startIndex)) * 10, vp.height() - barHeight, 10, barHeight);
                    painter->fillRect(rc, brOut);
                } else {
                    QRect rc((offset + i - startIndex) * 10, vp.height() - barHeight, 10, barHeight);
                    painter->fillRect(rc, brOut);
                }
            }
        }

    }

    painter->setWindow(vp);
    painter->setPen(QColor(178, 168, 168));
    painter->drawRect(vp.x(), vp.y(), vp.width() - 1, vp.height() - 1);

    if (m_maxValue != maxValue) {
        m_maxValue = maxValue;
        emit peekValueChanged(m_maxValue);
    }
}

RotatedLabel::RotatedLabel(const QString& text, QWidget *parent)
    : QWidget(parent), m_display(true)
{
    setText(text);
}

RotatedLabel::~RotatedLabel()
{
}

void RotatedLabel::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        updateLabel();
    }
}

void RotatedLabel::setDisplay(bool display)
{
    m_display = display;
    update();
}

QString RotatedLabel::text() const
{
    return m_text;
}

void RotatedLabel::updateLabel()
{
    const QFontMetrics& fm = fontMetrics();
    m_contentSize = QSize(fm.height() + 5, fontMetrics().width(m_text));
    updateGeometry();
    update(contentsRect());
}

void RotatedLabel::paintEvent(QPaintEvent *evt)
{
    if (!m_display) {
        return;
    }
    QRect cr = contentsRect();
    QPainter painter(this);
    if (QApplication::isRightToLeft()) {
        painter.translate(0, cr.height());
        painter.rotate(-90);
        painter.drawText(QRect(0, 0, cr.height() - 30, cr.width()), Qt::AlignLeft, m_text);
    } else {
        painter.translate(0, cr.height());
        painter.rotate(-90);
        painter.drawText(QRect(0, 0, cr.height() - 30, cr.width()), Qt::AlignRight/*|Qt::AlignBottom*/, m_text);
    }
}

QSize RotatedLabel::sizeHint() const
{
    //return m_contentSize;
    return QSize(18, 200);
}

QString deltaText(quint64 value)
{
    double v = static_cast<double>(value) / (1024 * 128);
    int prec;
    if (v >= 1000) {
        prec = 0;
    } else if (v >= 100) {
        prec = 1;
    } else {
        prec = 2;
    }
    QString s = QString("%1Mbps").arg(v, 5, 'f', prec);
    if (s.trimmed() == "0.00Mbps") {
        s = "0Mbps";
    }
    return s;
}

template <class Func>
        void doForAll(QWidget *widget, Func func)
{
    func(widget);

    QObjectList::const_iterator it = widget->children().begin();
    QObjectList::const_iterator ie = widget->children().end();
    for (; it != ie; ++it) {
        QObject *child = *it;
        if (child->isWidgetType()) {
            doForAll(qobject_cast<QWidget*>(child), func);
        }
    }
}

class DisableFocusF
{
public:
    void operator()(QWidget *widget)
    {
        widget->setFocusPolicy(Qt::NoFocus);
    }
};

NetTrafficView::NetTrafficView(QWidget *parent)
    : QWidget(parent), m_alertLimit(0),m_flashTimer(0),m_flashIconIdx(0)
{
   // setStyleSheet("font:75 13px \"Arial\";");

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->setContentsMargins(1, 1, 1, 1);
    layout0->setSpacing(0);
    setLayout(layout0);

    m_peekValue = new RotatedLabel(deltaText(0), this);
    m_peekValue->setContentsMargins(0, 0, 0, 0);
    layout0->addWidget(m_peekValue);

    QVBoxLayout *layout1 = new QVBoxLayout();
    layout1->setSpacing(2);
    layout1->setContentsMargins(0, 0, 0, 0);
    layout0->addLayout(layout1);
    layout0->addSpacing(17);

    m_layoutVW = new QStackedLayout();
    layout1->addLayout(m_layoutVW, 1);

    m_graph = new NetTrafficGraph(this);
    m_layoutVW->addWidget(m_graph);

    m_display = new NetMonitorDisplay(this);
    m_layoutVW->addWidget(m_display);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setContentsMargins(0, 0, 0, 0);
    layout1->addLayout(layout2);


    m_alertSlider = new QSlider(Qt::Horizontal, this);
    m_alertSlider->setRange(1, 20);
    m_alertSlider->setMaximumWidth(60);
    layout2->addWidget(m_alertSlider);
    m_alertText = new QLabel(this);
    m_alertText->setMinimumWidth(60);
    layout2->addWidget(m_alertText);
    /*
	QHBoxLayout *layoutCC = new QHBoxLayout();
	layout2->addLayout(layoutCC, 1);

	m_alertCheck = new QCheckBox(this);
	layoutCC->addWidget(m_alertCheck);
	m_alertCheckLabel = new QLabel(this);
	m_alertCheckLabel->setWordWrap(true);
	layoutCC->addWidget(m_alertCheckLabel);
	layoutCC->addStretch();
*/

    m_alertCheck = new QCheckBox(this);
    layout2->addWidget(m_alertCheck, 1);

    QLabel *uploadLogo = new QLabel(this);
    uploadLogo->setPixmap(QPixmap(":/image/upload_big.png"));
    layout2->addWidget(uploadLogo);

    m_uploadValue = new QLabel(deltaText(0), this);
    m_uploadValue->setMinimumWidth(70);
    m_uploadValue->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    layout2->addWidget(m_uploadValue);

    QLabel *downloadLogo = new QLabel(this);
    downloadLogo->setPixmap(QPixmap(":/image/download_big.png"));
    downloadLogo->setContentsMargins(10, 0, 0, 0);
    layout2->addWidget(downloadLogo);

    m_downloadValue = new QLabel(deltaText(0), this);
    m_downloadValue->setMinimumWidth(70);
    m_downloadValue->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    layout2->addWidget(m_downloadValue);

    m_checkDetail = new QPushButton(this);
	m_checkDetail->setFlat(true);
    m_checkDetail->setContentsMargins(10, 0, 0, 0);
    m_checkDetail->setCursor(Qt::PointingHandCursor);
    m_checkDetail->setIcon(QPixmap(":/image/netmon_detail.png"));
    m_checkDetail->setStyleSheet("*{background-color:none;border:none;min-width:25px;}\nQCheckBox::indicator { width: 0px; height 0px; }");
    layout2->addWidget(m_checkDetail);

    QVBoxLayout *layoutGG = new QVBoxLayout();
    m_graph->setLayout(layoutGG);
    QHBoxLayout *layoutRR = new QHBoxLayout();
    layoutGG->addLayout(layoutRR);
    layoutGG->addStretch();

    QColor colorIn(0, 113, 215);
    QColor colorOut(0, 153, 0);

    QPixmap downloadLegend(16, 16);
    {
        QPainter painter(&downloadLegend);
        painter.fillRect(downloadLegend.rect(), QBrush(colorIn));
    }

    QPixmap uploadLegend(16, 16);
    {
        QPainter painter(&uploadLegend);
        painter.fillRect(uploadLegend.rect(), QBrush(colorOut));
    }

    m_checkDownload = new QCheckBox(this);
    m_checkDownload->setStyleSheet("color: rgb(0, 113, 215)");
    m_checkDownload->setIcon(downloadLegend);
    m_checkDownload->setMinimumWidth(128);
    layoutRR->addWidget(m_checkDownload);

    m_checkUpload = new QCheckBox(this);
    m_checkUpload->setStyleSheet("color: rgb(0, 153, 0)");
    m_checkUpload->setIcon(uploadLegend);
    m_checkUpload->setMinimumWidth(128);
    layoutRR->addWidget(m_checkUpload);

    layoutRR->addStretch();

    m_alertCheck->setChecked(GET_SETTING_BOOL(PLUGIN_TRAFFIC_WARRING));
    m_alertSlider->setValue(GET_SETTING_INT(PLUGIN_TRAFFIC_THRESHOLD));
    updateAlertValue(m_alertSlider->value());

    connect(m_checkDownload, SIGNAL(stateChanged(int)), SLOT(checkDownloadChanged(int)));
    connect(m_checkUpload, SIGNAL(stateChanged(int)), SLOT(checkUploadChanged(int)));
    connect(m_graph, SIGNAL(newValue(quint32,quint32)), SLOT(newValue(quint32,quint32)));
    connect(m_graph, SIGNAL(peekValueChanged(quint32)), SLOT(peekValueChanged(quint32)));
    connect(m_graph, SIGNAL(trafficAlert(bool)), SIGNAL(trafficAlert(bool)));
    connect(m_alertSlider, SIGNAL(valueChanged(int)), SLOT(updateAlertValue(int)));
    connect(m_alertCheck, SIGNAL(stateChanged(int)), SLOT(updateAlertCheck(int)));
	connect(m_checkDetail, SIGNAL(clicked()), SLOT(detailClicked()));
	m_detailFlag = false;

    m_checkUpload->setCheckState(Qt::Checked);
    m_checkDownload->setCheckState(Qt::Checked);
    //m_alertSlider->setValue(1);
    //updateAlertValue(1);

    {
        QString value = GENIE2_GET_SETTING(DETAILSCHECK_KEY).toString();
        if(value.compare(QString("1")) != 0){
            m_flashTimer = new QTimer(this);
            connect(m_flashTimer,SIGNAL(timeout()),SLOT(detailCheckFlashTimeout()));
            m_flashTimer->start(DETAILCHECK_INTERVAL);
        }
    }



    retranslateUi();

    doForAll(this, DisableFocusF());
}

NetTrafficView::~NetTrafficView()
{
    if(m_flashTimer){
        m_flashTimer->stop();
    }
}

void NetTrafficView::restoreStackPanel()
{
	if (m_detailFlag) {
		m_checkDetail->click();
	}
}

void NetTrafficView::retranslateUi()
{
    m_checkDownload->setText(GeniePlugin_Internet::translateText(L_NETTRAFFIC_DOWNLOAD));
    m_checkUpload->setText(GeniePlugin_Internet::translateText(L_NETTRAFFIC_UPLOAD));
    m_alertCheck->setText(GeniePlugin_Internet::translateText(L_INTERNET_TRAFFIC_ALERTSETTING_CB_TEXT));
    m_display->retranslateUi();

    m_checkDetail->setToolTip(m_checkDetail->isChecked() ? QString() : GeniePlugin_Internet::translateText(L_INTERNET_FIX_SHOWDETAILS_CB_TEXT));

}

void NetTrafficView::changeEvent(QEvent *evt)
{
    if (evt->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(evt);
}

void NetTrafficView::setMonitor(NetMonitor *monitor)
{
    m_graph->setMonitor(monitor);
    m_display->setMonitor(monitor);
}

void NetTrafficView::checkDownloadChanged(int state)
{
    m_graph->setShowIncoming(state == Qt::Checked);
}

void NetTrafficView::checkUploadChanged(int state)
{
    m_graph->setShowOutcoming(state == Qt::Checked);
}

void NetTrafficView::newValue(quint32 deltaOut, quint32 deltaIn)
{
    m_uploadValue->setText(deltaText(deltaOut));
    m_downloadValue->setText(deltaText(deltaIn));
}

void NetTrafficView::peekValueChanged(quint32 value)
{
    m_peekValue->setText(deltaText(value));
}

void NetTrafficView::updateAlertValue(int value)
{
    m_alertText->setText(QString("%1Mbps").arg(value));
    m_alertLimit = value * 1024 * 128;
    if (m_alertCheck->checkState() == Qt::Checked) {
        m_graph->setTrafficThreshold(m_alertLimit);
    }

    SET_SETTING_VALUE(PLUGIN_TRAFFIC_WARRING, m_alertCheck->checkState() == Qt::Checked);
    SET_SETTING_VALUE(PLUGIN_TRAFFIC_THRESHOLD, m_alertSlider->value());
}

void NetTrafficView::updateAlertCheck(int state)
{
    if (state == Qt::Checked) {
        m_graph->setTrafficThreshold(m_alertLimit);
    } else {
        m_graph->setTrafficThreshold(0);
    }

    SET_SETTING_VALUE(PLUGIN_TRAFFIC_WARRING, m_alertCheck->checkState() == Qt::Checked);
    SET_SETTING_VALUE(PLUGIN_TRAFFIC_THRESHOLD, m_alertSlider->value());
}

void NetTrafficView::detailClicked()
{
	m_detailFlag = !m_detailFlag;
	switchDetail(m_detailFlag);

    //add for statistics by duanlin.yi
    if(m_detailFlag)
    {
        GeniePlugin_Internet::function_usage_statistics("I3");
    }
    //end

    if(m_flashTimer){
        m_flashTimer->stop();
        delete m_flashTimer;
        m_flashTimer = 0;

        GENIE2_SET_SETTING(DETAILSCHECK_KEY,QString("1"));
    }
}

void NetTrafficView::detailCheckFlashTimeout()
{
    if(!m_flashTimer){
        return;
    }

    static const char* icons[] = {
        ":/image/netmon_detail.png",
        ":/image/netmon_detail2.png"
    };

    m_checkDetail->setIcon(QPixmap(icons[m_flashIconIdx]));
    m_flashIconIdx = (++m_flashIconIdx) % (sizeof(icons) / sizeof(icons[0]));
}

void NetTrafficView::switchDetail(bool detail)
{
    if (detail) {
        m_layoutVW->setCurrentWidget(m_display);
        m_peekValue->setDisplay(false);
        m_checkDetail->setIcon(QPixmap(":/image/netmon_barchart.png"));
    } else {
        m_layoutVW->setCurrentWidget(m_graph);
        m_peekValue->setDisplay(true);
        m_checkDetail->setIcon(QPixmap(":/image/netmon_detail.png"));
    }

    m_checkDetail->setToolTip(detail ? QString() : GeniePlugin_Internet::translateText(L_INTERNET_FIX_SHOWDETAILS_CB_TEXT));

}
