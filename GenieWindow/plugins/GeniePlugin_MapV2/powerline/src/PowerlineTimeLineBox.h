#ifndef POWERLINETIMELINEBOX_H
#define POWERLINETIMELINEBOX_H

#include <QWidget>
#include <QTimeLine>

namespace Ui {
    class PowerlineTimeLineBox;
}

class PowerlineTimeLineBox : public QWidget
{
    Q_OBJECT

public:
    explicit PowerlineTimeLineBox(QWidget *parent = 0);
    ~PowerlineTimeLineBox();

private:
    Ui::PowerlineTimeLineBox *ui;

private:
	QTimeLine		m_TimerLine;
	unsigned int	m_applyWaitTime;
	bool			m_isEmit;
	int				m_titleID;
	int				m_contentID;
private slots:
	void slot_TimeLineOut(int);
	void slot_TimeLineFinish();
signals:
	void dlgOver(int);
public:
	void showTimeLineDlg(int titleID, int contentID, unsigned int timeNum);
private:
	void init();
	void initLanguage();
	void retranslateUi();
protected:
	virtual void changeEvent(QEvent *);
};

#endif // POWERLINETIMELINEBOX_H
