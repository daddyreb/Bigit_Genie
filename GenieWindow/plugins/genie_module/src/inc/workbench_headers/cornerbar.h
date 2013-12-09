#ifndef CORNERBAR_H
#define CORNERBAR_H

#include <QToolBar>

class CornerBar : public QToolBar
{
	Q_OBJECT

public:
	explicit CornerBar(QWidget *parent);
	~CornerBar();

private:
protected:
	virtual void paintEvent ( QPaintEvent * event );
};

#endif // CORNERBAR_H
