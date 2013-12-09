#ifndef UPDATETABSEVENT_H
#define UPDATETABSEVENT_H

#include <QEvent>
#include <QSize>

#define  UPDATETABSEVENT_TYPE (QEvent::Type(QEvent::User+100))

class UpdateTabsEvent : public QEvent
{
public:
	enum UPDATE_RESEAON{RESIZE,ADDTAB,REMOVETAB,CURPAGE_CHANGED};
public:
	UpdateTabsEvent(UPDATE_RESEAON ur,QSize sz,QSize oldSz);
	~UpdateTabsEvent();

private:
	QSize curSize;
	QSize oldsize;
public:
	QSize size();
	QSize oldSize();
protected:
	UPDATE_RESEAON res;
public:
	UPDATE_RESEAON reason();
};

#endif // UPDATETABSEVENT_H
