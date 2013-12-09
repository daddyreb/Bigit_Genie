#include "QGenieInputPasswordWidget.h"
#include "QGenieDialogContainerFrame.h"
#include "GeniePlugin_Wireless.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "genie2_interface.h"

QGenieInputPasswordWidget::QGenieInputPasswordWidget(QWidget *parent,bool bIndependent)
    :QGenieDialogInnerWidget(parent),
    m_bIndependent(bIndependent),
    m_strAuth(""),
    m_strCipher("")
{
    init_widget();
}

QGenieInputPasswordWidget::QGenieInputPasswordWidget(const QString &auth,const QString &cipher,
                                                     QWidget *parent,bool bIndependent)
                                                         :QGenieDialogInnerWidget(parent),m_bIndependent(bIndependent),m_strAuth(auth),m_strCipher(cipher)
{
    init_widget();
}

void QGenieInputPasswordWidget::init_widget()
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    input_pwd_le		=new QLineEdit(this);
    //confirm_pwd_le		=new QLineEdit(this);
    show_plaintext_cb           =new QCheckBox(this);
    prompt_lb                   =new QLabel(this);
    input_lb			=new QLabel(this);
    //confirm_lb			=new QLabel(this);
    prompt_lb->setWordWrap(true);
    this->setFocusProxy(input_pwd_le);

    this->setTabOrder(input_pwd_le,show_plaintext_cb);

    show_plaintext_cb->setCheckState(Qt::Checked);
    input_pwd_le->setEchoMode(QLineEdit::Normal);
    input_pwd_le->setFocusPolicy(Qt::StrongFocus);
    GENIE2_SET_FOCUS(input_pwd_le);

    //confirm_pwd_le->setEchoMode(QLineEdit::Password);

    QGridLayout *grid_layout=new QGridLayout();
    grid_layout->addWidget(prompt_lb,0,0,1,2);
    grid_layout->addWidget(show_plaintext_cb,1,0,1,2);
    grid_layout->addWidget(input_lb,2,0,1,1);
    grid_layout->addWidget(input_pwd_le,2,1,1,1);
    //grid_layout->addWidget(confirm_lb,3,0,1,1);
    //grid_layout->addWidget(confirm_pwd_le,3,1,1,1);

    if (m_bIndependent)
    {
        ok_btn				=new QPushButton(this);
        cancel_btn			=new QPushButton(this);
        //ok_btn->setFocusPolicy(Qt::NoFocus);
        //cancel_btn->setFocusPolicy(Qt::NoFocus);

        opt_result_lb		=new QLabel(this);
        opt_result_lb->setVisible(false);
        QSpacerItem *spitem=new QSpacerItem(0,0,QSizePolicy::Expanding);
        QHBoxLayout *hor_layout=new QHBoxLayout();
        hor_layout->addItem(spitem);
        hor_layout->addWidget(ok_btn);
        hor_layout->addWidget(cancel_btn);

        QVBoxLayout *ver_layout=new QVBoxLayout();
        ver_layout->addLayout(grid_layout);
        ver_layout->addWidget(opt_result_lb);
        ver_layout->addLayout(hor_layout);
        this->setLayout(ver_layout);

        connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
        connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));
    }
    else
    {
        ok_btn			=0;
        cancel_btn		=0;
        opt_result_lb           =0;
        grid_layout->setMargin(0);
        this->setLayout(grid_layout);
    }

    retranslateUi();

    connect(show_plaintext_cb,SIGNAL(stateChanged(int)),this,SLOT(cb_stateChanged(int)));
}

void QGenieInputPasswordWidget::retranslateUi()
{
    if (m_bIndependent)
    {
        ok_btn->setText(/*tr("Ok")*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_OK_BTN_TEXT));
        cancel_btn->setText(/*tr("Cancel")*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_CANCEL_BTN_TEXT));
        opt_result_lb->setText(/*tr("The password is invalid please try again.")*/
                GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_OPTRESULT_LB_TEXT));
    }

    prompt_lb->setText(/*tr("Connect to this network require a password.")*/
            GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PROMPT_LB_TEXT));
    show_plaintext_cb->setText(/*tr("Show clearly")*/
            GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PLAINTEXT_CB_TEXT));
    input_lb->setText(/*tr("Password:")*/
            GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PWD_LB_TEXT));
    //confirm_lb->setText(tr("Confirm:"));
}

void QGenieInputPasswordWidget::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
    /*QDialog*/QGenieDialogInnerWidget::changeEvent(event);
}

bool QGenieInputPasswordWidget::input_password_ok()
{
    if ((input_pwd_le->text().isEmpty()==false)/* && input_pwd_le->text()==confirm_pwd_le->text()*/
        &&valid_password(m_strAuth,m_strCipher,input_pwd_le->text()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString QGenieInputPasswordWidget::password()
{
    if (input_password_ok())
    {
        return input_pwd_le->text();
    }
    else
    {
        return QString();
    }
}

void QGenieInputPasswordWidget::cb_stateChanged(int state)
{
    if (Qt::Checked==state)
    {
        input_pwd_le->setEchoMode(QLineEdit::Normal);
        //confirm_pwd_le->setEchoMode(QLineEdit::Normal);
    }
    else
    {
        input_pwd_le->setEchoMode(QLineEdit::Password);
        //confirm_pwd_le->setEchoMode(QLineEdit::Password);
    }
}

void QGenieInputPasswordWidget::set_prompt_text(const QString& str_prompt)
{
    prompt_lb->setText(str_prompt);
}

void QGenieInputPasswordWidget::ok_btn_clicked(bool checked)
{
    if (input_password_ok())
    {
        accept();
    }
    else
    {
        //opt_result_lb->setVisible(true);
        QString str_arg;
        if(m_strCipher== QString("WEP"))
        {
            str_arg = QString("5 , 13");
        }
        else if((m_strAuth== QString("WPAPSK")) || (m_strAuth== QString("WPA2PSK")))
        {
            str_arg = QString("8-63");
        }
        QGenieDialogContainerFrame::passwordInvalidBox(this,GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PWDINVALID_PROMPT_TITLE),
                                                       GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_PWDINVALID_PROMPT_TEXT).arg(str_arg)
                                                       ,QString(":/image/pwd_invalid.png"));
    }
}

void QGenieInputPasswordWidget::cancel_btn_clicked(bool checked)
{
    reject();
}

void QGenieInputPasswordWidget::set_show_password(const QString &pwd)
{
    input_pwd_le->setText(pwd);
}

bool QGenieInputPasswordWidget::valid_password(const QString &auth,const QString &cipher,const QString &key)
{
    // Network key
    // WEP keys are of 5 or 13 characters (ASCII) or of 10 or 26 characters (hexadecimal)
    // WPAPSK and WPA2PSK keys are 8-63 characters (ASCII) or 64 characters (hexadecimal)
    if ( ( (cipher== QString("WEP")) && ( key.length()!=5 ) && ( key.length()!=13 ) && ( key.length()!=10 ) && ( key.length()!=26 ) )
        ||
                ( ( (auth==QString("WPAPSK")) || (auth==QString("WPA2PSK"))) && ( ( (key.length() < 8 ) || (key.length() > 64 ) ) ) ) )

    {
        return false;
    }
    else
    {
        return true;
    }
}

QString QGenieInputPasswordWidget::get_dlg_title()
{
    return GeniePlugin_Wireless::translateText(L_WIRELESS_IPW_DLG_TITLE);
}
