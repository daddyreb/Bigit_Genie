#ifndef POWERLINESECURITY_H
#define POWERLINESECURITY_H

#include <QDialog>
#include "PLDevice.h"

#include "PowerlineSecurity_PWD.h"

namespace Ui {
class PowerlineSecurity;
}

class PowerlineSecurity : public QDialog
{
    Q_OBJECT

public:
    explicit PowerlineSecurity(QWidget *parent = 0);
    ~PowerlineSecurity();


    void initializeValue(QList<PLDevice *> devList);
private:
    void setHeaderStyle();
    void slotConnect();
    void clearData();
	void initLanguage();
    void wipeOutRepeatItem(QList<QStringList> &data);


	bool chkAndDeal(const QList<QStringList> &);
	QList<QPair<QString, QString> >	m_lstData;

    enum RADIO_SEL
    {
        RS_DEFAULT,
        RS_NEW
    };

    RADIO_SEL rdoCheck();
    bool keyCheck();


    QList<QPair<QString, QString> >     m_data;
    PowerlineSecurity_PWD				m_pwdDlg;
    QList<QStringList>					m_devInfo;
    QList<QStringList>                  m_chk_devInfo;
    QString                             m_key;
	QString								m_strMac;
	QString								m_model;
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void changeEvent(QEvent *);


	void retranslateUi();


	bool    m_bMove;
	QPoint  m_position;
private slots:
    void slot_BtnClose();
    void slot_BtnRefresh();
    void slot_BtnApply();
    void slot_Progress(int num);
    void slot_chkState(int, int);
    void slot_retPwdDlg(const QList<QPair<QString, QString> >&);

    void slot_rdo_default();
    void slot_rdo_new();
signals:
    void signRefresh();
    void signApply(const QString &,const QList<QPair<QString, QString> >&);
private:
    Ui::PowerlineSecurity *ui;
};

#endif // POWERLINESECURITY_H
