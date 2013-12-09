#ifndef _WHITE_URL_TABLE_WIDGET_H_
#define _WHITE_URL_TABLE_WIDGET_H_
#include <QTableWidget>
#include <QUrl>
#include <QStyleOptionViewItem>

#define COLUMN_0_WIDTH 300
#define COLUMN_1_WIDTH 30
#define COLUMN_2_WIDTH 30
#include <QWidget>

//白名单列表
class WhiteUrlTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	WhiteUrlTableWidget(QWidget *parent);
	virtual ~WhiteUrlTableWidget();

	void SetUrl(QUrl& url);
	void AddUrl(QString& url);
	void AddPrivateUrlList(const QStringList& list);
	void AddPublicUrlList(const QStringList& list);
	void Clear();
private slots:
	void OnClickItem(QTableWidgetItem * item);
	
private:
	void OpenUrl(QString& url);
	void mouseMoveEvent(QMouseEvent * event);
	void ResetHoverItem();
	//
	QIcon *m_delete_icon;
	QIcon *m_delete_hover_icon;
	QIcon *m_explore_icon;
	QIcon *m_explore_hover_icon;

	QUrl m_server_url;
	QTableWidgetItem *m_hover_item;
};
#endif