#ifndef POWERLINEDEVICENAME_H
#define POWERLINEDEVICENAME_H

#include <QDialog>

namespace Ui {
	class PowerlineDeviceName;
}

class PowerlineDeviceName : public QDialog
{
	Q_OBJECT

public:
	explicit PowerlineDeviceName(QWidget *parent = 0);
	~PowerlineDeviceName();

public:
	void InitializedData(const QString &, const QString &);
protected:
	void Init();
	void slotInit();
	void initLanguage();

	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);
	//bool eventFilter (QObject *watched, QEvent * event);


	void retranslateUi();

	bool    m_bMove;
	QPoint  m_position;
	QString m_name;
private:


signals:
	void signPDNok(QString);
private slots:
	void slot_btn_ok();
	void slot_btn_close();
	void slot_text(QString);

private:
	Ui::PowerlineDeviceName *ui;
};

#endif // POWERLINEDEVICENAME_H
