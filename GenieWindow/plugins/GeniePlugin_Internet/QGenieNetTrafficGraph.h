#ifndef __NetTrafficGraph_h__
#define __NetTrafficGraph_h__

#include <QWidget>
#include <QTreeView>
#include <QTableView>
#include <QStandardItem>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QPointer>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QStackedLayout>
#include <deque>
#include "QGenieNetMonitor.h"

class BaseNetMonitorItem
	: public QStandardItem
{
public:
	BaseNetMonitorItem();
	BaseNetMonitorItem(const QString& text);

	void notifyChanged();
	void updateFullRow();
};

class NetMonitorSpeedItem
	: public BaseNetMonitorItem
{
public:
	NetMonitorSpeedItem(double speed);
	void setSpeed(double speed);
	virtual bool operator<(const QStandardItem& other) const;

private:
	double m_speed;
};

class NetMonitorSizeItem
	: public BaseNetMonitorItem
{
public:
	NetMonitorSizeItem(quint64 size);
	void setSize(quint64 size);
	virtual bool operator<(const QStandardItem& other) const;

private:
	quint64 m_size;
};

class NetMonitorNumericItem
	: public BaseNetMonitorItem
{
public:
	NetMonitorNumericItem(int value);
	void setValue(int value);
	virtual bool operator<(const QStandardItem& other) const;

private:
	int m_value;
};

class NetMonitorItem
	: public BaseNetMonitorItem
{
public:
	NetMonitorItem();

	void scheduleDelete(QTimer *timer);
	void scheduleNew(QTimer *timer);
	void flagAsNormal();

	bool deleteFlag;
	bool newFlag;
	QTimer *deleteTimer;
	QTimer *newTimer;
};

class NetMonitorAppItem
	: public NetMonitorItem
{
public:
	quint32 pid;
	QString fullPath;
};

class NetMonitorDetailItem
	: public NetMonitorItem
{
public:
	quint32 localAddr;
	quint32 localPort;
	quint32 remoteAddr;
	quint32 remotePort;
};

class NetMonitorModel
	: public QStandardItemModel
{
	Q_OBJECT

public:
	NetMonitorModel(QObject *parent = NULL);
	virtual ~NetMonitorModel();

	void resort();

	void beginUpdate();
	void endUpdate();
	void updateAppData(const NM_APP_DETAIL *appData, const quint8 *buffer0, NetMonitor *monitor);
	void updateDetailRows(NetMonitorAppItem *pAppItem, const NM_APP_DETAIL *appData, const quint8 *buffer0);
	void updateTcpRow(NetMonitorAppItem *pAppItem, const NM_CONNECTION_TCP *detailData, const quint8 *buffer0);
	void updateUdpRow(NetMonitorAppItem *pAppItem, const NM_CONNECTION_UDP *detailData, const quint8 *buffer0);

	void retranslateUi();

	virtual void sort(int column, Qt::SortOrder order);

private slots:
	void deferredDeleteItems();
	void deferredNewItems();

private:
	int m_sortColumn;
	Qt::SortOrder m_sortOrder;
};

class NetMonitorItemDelegate
	: public QStyledItemDelegate
{
	Q_OBJECT

public:
	NetMonitorItemDelegate(QObject *parent = NULL);

protected:
	virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	QBrush m_deleteBG;
	QBrush m_newBG;
};

class NetMonitorDisplay
	: public QTreeView
{
	Q_OBJECT

public:
	NetMonitorDisplay(QWidget *parent = NULL);
	virtual ~NetMonitorDisplay();

	void setMonitor(NetMonitor *monitor);
	void retranslateUi();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *ev);

private slots:
	void refresh();

private:
	void parseAndUpdate(const quint8 *buffer);

private:
	QPointer<NetMonitor> m_monitor;
	QTimer m_refreshTimer;
	NetMonitorModel m_model;
	NetMonitorItemDelegate m_delegate;
};

template <class T, int N>
class RingBufferT
{
public:
	RingBufferT()
	: m_buffer(N), m_pos(0), m_length(0)
	{
	}

	void push_back(const T& v)
	{
		if (m_length < N) {
			size_t nextPos = m_pos + m_length;
			if (nextPos >= N) {
				nextPos -= N;
			}
			m_buffer[nextPos] = v;
			++m_length;
		} else {
			m_buffer[m_pos] = v;
			if (++m_pos == N) {
				m_pos = 0;
			}
		}
	}

	size_t size() const
	{
		return m_length;
	}

	const T *at(size_t pos) const
	{
		if (pos >= m_length) {
			return NULL;
		}
		pos += m_pos;
		if (pos >= N) {
			pos -= N;
		}
		return &m_buffer[pos];
	}

private:
	std::vector<T> m_buffer;
	size_t m_pos;
	size_t m_length;
};

class NetTrafficGraph
	: public QWidget
{
	Q_OBJECT

	enum { Outcoming, Incoming };

public:
	NetTrafficGraph(QWidget *parent = NULL);
	virtual ~NetTrafficGraph();

	void setMonitor(NetMonitor *monitor);
	void setShowOutcoming(bool show);
	bool showOutcoming() const;
	void setShowIncoming(bool show);
	bool showIncoming() const;
	void setTrafficThreshold(quint32 threshold);

signals:
	void newValue(quint32 deltaOut, quint32 deltaIn);
	void peekValueChanged(quint32 value);
	void trafficAlert(bool bAlert);

protected:
	virtual void paintEvent(QPaintEvent *evt);

private slots:
	void refresh();

private:
	void parseAndUpdate(const quint8 *buffer);
	void inputData(quint64 deltaOut, quint64 deltaIn);
	void drawBars(QPainter *painter, size_t columns);

private:
	struct History
	{
		quint32 out;
		quint32 in;
	};

	enum { HISTORY_CAPACITY = 300 };

	QPointer<NetMonitor> m_monitor;
	QTimer m_refreshTimer;
	RingBufferT<History, HISTORY_CAPACITY> m_history;
	quint32 m_maxValue;
	bool m_drawIncoming;
	bool m_drawOutcoming;
	quint32 m_trafficLimit;
	std::deque<quint32> m_avg;
	bool m_trafficFlag;
};

class RotatedLabel
	: public QWidget
{
	Q_OBJECT

public:
	RotatedLabel(const QString& text, QWidget *parent = NULL);
	virtual ~RotatedLabel();

	void setText(const QString& text);
	QString text() const;
	void setDisplay(bool display);

protected:
	virtual void paintEvent(QPaintEvent *evt);
	virtual QSize sizeHint() const;

private:
	void updateLabel();

private:
	QString m_text;
	QSize m_contentSize;
	bool m_display;
};

class NetTrafficView
	: public QWidget
{
	Q_OBJECT

public:
	NetTrafficView(QWidget *parent = NULL);
	virtual ~NetTrafficView();

	void setMonitor(NetMonitor *monitor);
	void restoreStackPanel();

signals:
	void trafficAlert(bool bAlert);

protected:
	virtual void changeEvent(QEvent *evt);

private slots:
	void checkDownloadChanged(int state);
	void checkUploadChanged(int state);
	void newValue(quint32 deltaOut, quint32 deltaIn);
	void peekValueChanged(quint32 value);
	void updateAlertValue(int value);
	void updateAlertCheck(int state);
	void detailClicked();
    void detailCheckFlashTimeout();

	void retranslateUi();

private:
	void switchDetail(bool detail);

private:
	NetTrafficGraph *m_graph;
	NetMonitorDisplay *m_display;
	RotatedLabel *m_peekValue;
	QLabel *m_uploadValue;
	QLabel *m_downloadValue;
	QSlider *m_alertSlider;
	QLabel *m_alertText;
	QCheckBox *m_alertCheck;
	QCheckBox *m_checkDownload;
	QCheckBox *m_checkUpload;
	QPushButton *m_checkDetail;
	QStackedLayout *m_layoutVW;
	quint32 m_alertLimit;
    QTimer *m_flashTimer;
    int m_flashIconIdx;
	bool m_detailFlag;
};


#endif // __NetTrafficGraph_h__
