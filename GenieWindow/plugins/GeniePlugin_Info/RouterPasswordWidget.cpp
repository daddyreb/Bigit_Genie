#include "RouterPasswordWidget.h"
#include "QGenieDialogContainerFrame.h"
#include "plugin_ui_def.h"
#include "GeniePlugin_Info.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

RouterPasswordWidget::RouterPasswordWidget(QWidget *parent)
    :/*QDialog*/QGenieDialogInnerWidget(parent),m_user(""),m_pwd("")
{
    init_widget();
}

RouterPasswordWidget::RouterPasswordWidget(QString &username, QString &password, QWidget *parent)
    :/*QDialog*/QGenieDialogInnerWidget(parent),m_user(username),m_pwd(password)
{
    init_widget();
}

void RouterPasswordWidget::init_widget()
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    explainLabel = new QLabel(this);
    //explainLabel->setText(tr("The user's name or password is worng when genie was get Firmware information."));
    explainLabel->setWordWrap(true);

    userLabel = new QLabel(this);
    //userLabel->setText(tr("User"));
    userEdit = new QLineEdit(this);
    userEdit->setFixedWidth(180);
    userEdit->setDisabled(true);
    userEdit->setText(m_user);
    pwdLabel = new QLabel(this);
    //pwdLabel->setText(tr("Password"));
    pwdEdit = new QLineEdit(this);
    pwdEdit->setFixedWidth(180);
    pwdEdit->setEchoMode(QLineEdit::Normal);
    //pwdEdit->setEchoMode(QLineEdit::Password);
    pwdEdit->setText(m_pwd);

    saveCheck = new QCheckBox(this);
    //saveCheck->setText(tr("Save password"));
    saveCheck->setCheckState(Qt::Checked);

    QGridLayout *grid_layout=new QGridLayout();
    grid_layout->addWidget(explainLabel,0,0,1,2);
    grid_layout->addWidget(userLabel,1,0,1,2);
    grid_layout->addWidget(userEdit,1,1,1,2);
    grid_layout->addWidget(pwdLabel,2,0,1,1);
    grid_layout->addWidget(pwdEdit,2,1,1,1);
    grid_layout->addWidget(saveCheck,3,0,1,1);


    ok_btn = new QPushButton(this);
    //ok_btn->setText(tr("Ok"));
    cancel_btn = new QPushButton(this);
    //cancel_btn->setText(tr("Cancel"));
    //ok_btn->setFocusPolicy(Qt::NoFocus);
    //cancel_btn->setFocusPolicy(Qt::NoFocus);
    SET_FIX_SIZE(NORMAL_BUTTON,ok_btn);
    SET_FIX_SIZE(NORMAL_BUTTON,cancel_btn);

    //opt_result_lb = new QLabel(this);
    //opt_result_lb->setVisible(false);

    QSpacerItem *spitem=new QSpacerItem(0,0,QSizePolicy::Expanding);
    QHBoxLayout *hor_layout=new QHBoxLayout();
    hor_layout->addItem(spitem);
    hor_layout->addWidget(ok_btn);
    hor_layout->addWidget(cancel_btn);

    QVBoxLayout *ver_layout=new QVBoxLayout();
    ver_layout->addLayout(grid_layout);
    //ver_layout->addWidget(opt_result_lb);
    ver_layout->addLayout(hor_layout);
    this->setLayout(ver_layout);

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));


    retranslateUi();

}

void RouterPasswordWidget::retranslateUi()
{
    explainLabel->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_EXPLAIN));
    userLabel->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_USER));
    pwdLabel->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_PWD));
    saveCheck->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_SAVE));

    ok_btn->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_BUTOK));
    cancel_btn->setText(GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_BUTCANCEL));


}

void RouterPasswordWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    /*QDialog*/QGenieDialogInnerWidget::changeEvent(event);
}


QString RouterPasswordWidget::password()
{
    return pwdEdit->text();
}

QString RouterPasswordWidget::username()
{
    return userEdit->text();
}

void RouterPasswordWidget::ok_btn_clicked(bool checked)
{
    accept();
}

void RouterPasswordWidget::cancel_btn_clicked(bool checked)
{
    reject();
}

QString RouterPasswordWidget::get_dlg_title()
{
    return GeniePlugin_Info::translateText(L_INFO_MESSAGEBOX_TITLE);
}
