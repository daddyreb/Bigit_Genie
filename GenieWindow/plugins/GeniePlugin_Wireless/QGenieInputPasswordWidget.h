#ifndef QGENIEINPUTPASSWORDWIDGET_H
#define QGENIEINPUTPASSWORDWIDGET_H

#include "QGenieDialogInnerWidget.h"
//#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QEvent>
#include <QPushButton>

class QGenieInputPasswordWidget : public /*QDialog*/QGenieDialogInnerWidget
{
	Q_OBJECT
public:
    QGenieInputPasswordWidget(QWidget *parent=0,bool bIndependent=false);
    QGenieInputPasswordWidget(const QString &auth,const QString &cipher,QWidget *parent=0,bool bIndependent=false);
protected:
        QLineEdit                *input_pwd_le;
        //QLineEdit		*confirm_pwd_le;
	QCheckBox		*show_plaintext_cb;
	QLabel			*prompt_lb;
	QLabel			*input_lb;
        //QLabel			*confirm_lb;
	//independent
	QLabel			*opt_result_lb;
	QPushButton		*ok_btn;
	QPushButton		*cancel_btn;
private:
        bool                         m_bIndependent;
        QString                    m_strAuth;
        QString                    m_strCipher;
protected:
	void retranslateUi();
        void init_widget();
        bool valid_password(const QString &auth,const QString &cipher,const QString &key);
protected:
	virtual void changeEvent(QEvent *event);
protected slots:
	void  cb_stateChanged(int state);
public:
	bool input_password_ok();
	QString password();
	void set_prompt_text(const QString& str_prompt);
        void set_show_password(const QString &pwd);
protected slots:
	void ok_btn_clicked(bool checked);
	void cancel_btn_clicked(bool checked);
public:
        virtual QString get_dlg_title();
};

#endif // QGENIEINPUTPASSWORDWIDGET_H
