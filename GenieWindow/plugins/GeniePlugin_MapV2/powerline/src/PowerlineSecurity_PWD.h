#ifndef POWERLINESECURITY_PWD_H
#define POWERLINESECURITY_PWD_H

#include <QDialog>

namespace Ui {
class PowerlineSecurity_PWD;
}

class PowerlineSecurity_PWD : public QDialog
{
    Q_OBJECT

public:
    explicit PowerlineSecurity_PWD(QWidget *parent = 0);
    ~PowerlineSecurity_PWD();
public:
    void showDlg();
    void initializedTable(QList<QStringList>);
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);


	void retranslateUi();

    bool    m_bMove;
    QPoint  m_position;
private:
    void setHeaderStyle();
    void slotConnect();
    void setPic(const QString &);
    void init();
	void initLanguage();

    bool valueChk(const QString &);
	void wipeOutRepeatItem(QList<QPair<QString, QString> > &);


    QList<QPair<QString, QString> >		m_lstData;
	QString	m_strLocalDevice;
	int m_dealItemNum;
	int m_Dealed;

	bool	m_bOk;

signals:
    void signBtnOk(const QList<QPair<QString, QString> > &);
private slots:
    void slot_close();
    void slot_ok();
    void slot_dealCellChg(int, int);

	void OnEdited(const QString&);
	void OnChanged(const QString &);
private:
    Ui::PowerlineSecurity_PWD *ui;

protected:
	bool eventFilter ( QObject * watched, QEvent * event );
};

#endif // POWERLINESECURITY_PWD_H
