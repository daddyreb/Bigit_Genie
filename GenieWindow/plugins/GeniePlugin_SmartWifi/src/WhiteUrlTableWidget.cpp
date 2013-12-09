#include "WhiteUrlTableWidget.h"
#include <QHeaderView>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QMessageBox>
#include <QLabel>
#include "global.h"

WhiteUrlTableWidget::WhiteUrlTableWidget(QWidget *parent):
	QTableWidget(parent),
	m_hover_item(NULL)
{
	this->setFocusPolicy(Qt::TabFocus);
	this->verticalHeader()->setVisible(false);
	this->horizontalHeader()->setVisible(false);
	this->setSortingEnabled(true);
	this->setShowGrid(false);
	this->setColumnCount(3);
	this->setColumnWidth(0, COLUMN_0_WIDTH);
	this->setColumnWidth(1, COLUMN_1_WIDTH);
	this->setColumnWidth(2, COLUMN_2_WIDTH);
	this->setMouseTracking(true);
	this->setIconSize(QSize(18, 18));
	m_delete_icon = new QIcon(":/smartvpn/Resources/delete.png");
	m_delete_hover_icon = new QIcon(":/smartvpn/Resources/delete_hover.png");
	m_explore_icon = new QIcon(":/smartvpn/Resources/explore.png");
	m_explore_hover_icon = new QIcon(":/smartvpn/Resources/explore_hover.png");

	connect(this, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(OnClickItem(QTableWidgetItem *)));
}

WhiteUrlTableWidget::~WhiteUrlTableWidget()
{

}

void WhiteUrlTableWidget::SetUrl(QUrl& url)
{
	m_server_url = url;
}

void WhiteUrlTableWidget::Clear()
{
	this->clear();
	this->setRowCount(0);
	m_hover_item = NULL;
}

void WhiteUrlTableWidget::AddUrl(QString& url)
{
	QStringList list;
	list << url;
	AddPrivateUrlList(list);
}

void WhiteUrlTableWidget::AddPrivateUrlList(const QStringList& list)
{
	foreach(QString url, list)
	{
		this->setRowCount(this->rowCount() + 1);
		int n = this->rowCount();

		this->setRowHeight(n - 1, 25);
		
		QTableWidgetItem *item1 = new QTableWidgetItem(url);
		QTableWidgetItem *item2 = new QTableWidgetItem(*m_explore_icon, "");
		QTableWidgetItem *item3 = new QTableWidgetItem(*m_delete_icon, "");

		Qt::ItemFlags flags = item1->flags();
		flags ^= Qt::ItemIsEditable;
		item1->setFlags(flags);
	
		flags = item2->flags();
		flags ^= Qt::ItemIsSelectable;
		flags ^= Qt::ItemIsEditable;
		item2->setFlags(flags);
		item3->setFlags(flags);	

		this->setItem(n - 1, 0, item1);
		this->setItem(n - 1, 1, item2);
		this->setItem(n - 1, 2, item3);
	}
}

void WhiteUrlTableWidget::AddPublicUrlList(const QStringList& list)
{
	foreach(QString url, list)
	{
		this->setRowCount(this->rowCount() + 1);
		int n = this->rowCount();

		this->setRowHeight(n - 1, 25);

		QTableWidgetItem *item1 = new QTableWidgetItem(url);
		QTableWidgetItem *item2 = new QTableWidgetItem(*m_explore_icon, "");

		Qt::ItemFlags flags = item1->flags();
		flags ^= Qt::ItemIsEditable;
		item1->setFlags(flags);

		flags = item2->flags();
		flags ^= Qt::ItemIsSelectable;
		flags ^= Qt::ItemIsEditable;
		item2->setFlags(flags);

		this->setItem(n - 1, 0, item1);
		this->setItem(n - 1, 1, item2);
	}
}

void WhiteUrlTableWidget::ResetHoverItem()
{
	//reset m_hover_item
	if (! m_hover_item)
		return;

	int col = m_hover_item->column();

	if (col == 1)
	{
		m_hover_item->setIcon(*m_explore_icon);
		m_hover_item = NULL;
	}
	else if(col == 2)
	{
		m_hover_item->setIcon(*m_delete_icon);
		m_hover_item = NULL;
	}
}

void WhiteUrlTableWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (event->type() == QEvent::Leave)
	{
		ResetHoverItem();
		return;
	}
	
	QPointF pos = event->localPos();
	QTableWidgetItem *item = this->itemAt(QPoint(pos.x(), pos.y()));
	if (! item)
	{
		ResetHoverItem();
		return;
	}

	int col = item->column();
	ResetHoverItem();

	if (col == 1 || col == 2)
	{
		//set cur item
		if (col == 1)
		{
			//落到explore按钮上
				
			item->setIcon(*m_explore_hover_icon);
			m_hover_item = item;
		}
		else if (col == 2)
		{
			//落到delete按钮上
			item->setIcon(*m_delete_hover_icon);
			m_hover_item = item;
		}
	}
}

void WhiteUrlTableWidget::OnClickItem(QTableWidgetItem * item)
{
	int col = item->column();
	int row = item->row();

	if (col == 1)
	{
		QTableWidgetItem *item_url = this->item(row, 0);
		if (! item_url)
			return;
		OpenUrl(item_url->text());
	}
	else if (col == 2)
	{
//		DeleteItem(row);
	}
}

void WhiteUrlTableWidget::OpenUrl(QString& url)
{
	QString s = "http://" + url;
	QDesktopServices::openUrl(s);
}

//void WhiteUrlTableWidget::DeleteItem(int row)
//{
//	m_parent->SetDisableUI();
//
//	QString error;
//	SmartVPNConnector con;
//	QString url = this->item(row, 0)->text();
//	if (! con.DelWhiteUrl(m_server_url, url, error))
//	{
//		QMessageBox::warning(this, APPLICATION_TITLE, QStringLiteral("删除失败: ") + error);
//		m_parent->SetEnableUI();
//		return;
//	}
//
//	if (m_hover_item && (m_hover_item->row() == row))
//		m_hover_item = NULL;
//	this->removeRow(row);
//
//	m_parent->SetEnableUI();
//}
