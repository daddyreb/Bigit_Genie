#ifndef ROUTERPASSWORDWIDGET_H
#define ROUTERPASSWORDWIDGET_H

#include "QGenieDialogInnerWidget.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QEvent>
#include <QPushButton>

class RouterPasswordWidget : public /*QDialog*/QGenieDialogInnerWidget
{
    Q_OBJECT
public:
    RouterPasswordWidget(QWidget *parent=0);
    RouterPasswordWidget(QString &username, QString &password, QWidget *parent=0);

    virtual QString get_dlg_title();
    QString password();
    QString username();

protected:
    virtual void changeEvent(QEvent *event);

protected slots:
    void ok_btn_clicked(bool checked);
    void cancel_btn_clicked(bool checked);

private:
    void retranslateUi();
    void init_widget();

private:
    QString m_user;
    QString m_pwd;

    QLabel *explainLabel;

    QLabel *userLabel;
    QLineEdit *userEdit;
    QLabel *pwdLabel;
    QLineEdit *pwdEdit;
    QCheckBox *saveCheck;

    QPushButton *ok_btn;
    QPushButton *cancel_btn;


};

#endif // ROUTERPASSWORDWIDGET_H
