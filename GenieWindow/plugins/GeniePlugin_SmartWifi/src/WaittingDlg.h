#ifndef _WAITTING_DLG_
#define _WAITTING_DLG_
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QPushButton>

class WaittingDlg : public QDialog
{
	Q_OBJECT
public:
	WaittingDlg();
	~WaittingDlg(){};
	void SetText(const QString& text);
	void StartWaitting(const QString& text);
	void Finish(const QString& text);
private:
	QFrame *m_frame;
	QLabel *m_note;
	QPushButton *m_ok;
};
#endif