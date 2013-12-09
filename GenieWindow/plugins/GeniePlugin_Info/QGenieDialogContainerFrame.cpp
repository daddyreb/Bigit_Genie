#include "QGenieDialogContainerFrame.h"
//#include "GeniePlugin_Wireless.h"
#include "plugin_ui_def.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <QtGui>

QGenieDialogContainerFrame::QGenieDialogContainerFrame(Dlg_Size_Model size_model,QGenieDialogInnerWidget* inner,QWidget *parent) :
        QDialog(parent),m_inner_widget(inner)
{

    m_inner_widget->set_container_dlg(this);
    init_dim(size_model);

    //this->setModal(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::ToolTip);
    this->setAttribute(Qt::WA_TranslucentBackground);

    m_framecontainer = new QFrame(this);
    m_framecontainer->setObjectName(QString("frame_container"));
    //m_framecontainer->setFrameShape(QFrame::StyledPanel);
    //m_framecontainer->setFrameShadow(QFrame::Raised);
    m_framecontainer->setAutoFillBackground(true);
    m_framecontainer->setStyleSheet(m_framecontainer_style);

    QVBoxLayout  *dlg_layout = new QVBoxLayout(this);
    dlg_layout->setContentsMargins(0,0,0,0);
    dlg_layout->addWidget(m_framecontainer);

    m_dlgtitle_lb = new QLabel(m_framecontainer);
    init_dlgtitle(size_model);


    QSpacerItem  *title_spitem = new QSpacerItem(m_dlgtitle_lb_spaceitemw,0,QSizePolicy::Fixed);
    QHBoxLayout *title_layout  = new QHBoxLayout();
    title_layout->addItem(title_spitem);
    title_layout->addWidget(m_dlgtitle_lb);


    QVBoxLayout   *container_layout = new QVBoxLayout(m_framecontainer);
    container_layout->setContentsMargins(m_frame_lr_margin,m_frame_tb_margin,m_frame_lr_margin,m_frame_tb_margin);
    container_layout->setSpacing(1);
    container_layout->addLayout(title_layout);
    m_inner_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    container_layout->addWidget(m_inner_widget);

    container_layout->setStretch(0,0);
    container_layout->setStretch(1,10);

    resize(m_dlg_width,m_dlg_height);


    retranslateUi();

}

void QGenieDialogContainerFrame::init_dim(Dlg_Size_Model size_model)
{
    switch(size_model)
    {
    case Medium:
        m_dlg_width                             =  450;
        m_dlg_height                            = 277;
        m_dlgtitle_lb_height                 = 40;
        m_dlgtitle_lb_spaceitemw        = 60;
        m_frame_lr_margin                   = 14;
        m_frame_tb_margin                   = 13;
        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/images/bg_dialog_L.png);color:rgb(0,0,0);font-size:12px;}");
        break;
    case Normal:
        m_dlg_width                             =  383;
        m_dlg_height                            = 212;
        m_dlgtitle_lb_height                 = 32;
        m_dlgtitle_lb_spaceitemw        = 60;
        m_frame_lr_margin                   = 12;
        m_frame_tb_margin                   = 11;
        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/images/bg_dialog_M.png);color:rgb(0,0,0);font-size:12px;}");
        break;
    case Small:
        m_dlg_width                             =  228;
        m_dlg_height                            = 117;
        m_dlgtitle_lb_height                 = 26;
        m_dlgtitle_lb_spaceitemw        = 45;
        m_frame_lr_margin                   = 10;
        m_frame_tb_margin                   = 11;
        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/images/bg_dialog_S.png);color:rgb(0,0,0);font-size:12px;}");
        break;
    case Large:
    default:
        m_dlg_width                             =  538;
        m_dlg_height                            = 452;
        m_dlgtitle_lb_height                 = 46;
        m_dlgtitle_lb_spaceitemw        = 63;
        m_frame_lr_margin                   = 17;
        m_frame_tb_margin                   = 16;
        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/images/bg_dialog_XL.png);color:rgb(0,0,0);font-size:12px;}");
    }

}

void QGenieDialogContainerFrame::init_dlgtitle(Dlg_Size_Model size_model)
{
    m_dlgtitle_lb->setObjectName(QString("dlg_title_lb"));
    m_dlgtitle_lb->setFixedHeight(m_dlgtitle_lb_height);
    m_dlgtitle_lb->setAlignment(Qt::AlignVCenter);
    QString stylesheet =QString("#dlg_title_lb{color:rgb(255,255,255);%1;font-weight:bold;}");
    switch(size_model)
    {
    case Medium:
        stylesheet=stylesheet.arg("font-size:16px");
        break;
    case Normal:
        stylesheet=stylesheet.arg("font-size:14px");
        break;
    case Small:
        stylesheet=stylesheet.arg("font-size:12px");
        break;
    case Large:
        stylesheet=stylesheet.arg("font-size:18px");
    default:
        stylesheet=stylesheet.arg("font-size:18px");
    }
     m_dlgtitle_lb->setStyleSheet(stylesheet);
}

void QGenieDialogContainerFrame::retranslateUi()
{
    m_dlgtitle_lb->setText(m_inner_widget->get_dlg_title());
}

void QGenieDialogContainerFrame::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
    {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void QGenieDialogContainerFrame::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint newpos = event->globalPos();
        QPoint upleft = pos0 + newpos - last;
        this->move(upleft);
    }
}

void QGenieDialogContainerFrame::mousePressEvent(QMouseEvent *event)
{
    last = event->globalPos();
    pos0 = event->globalPos() - event->pos();
}

int QGenieDialogContainerFrame::messageBox(QWidget *parent,const QString &title,const QString &text,bool byesorno)
{
    QGenieInnerMsgBox *innermsg = new QGenieInnerMsgBox(parent,byesorno);
    innermsg->set_title(title);
    innermsg->set_text(text);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Small,innermsg,parent);
    int iret = dcf.exec();
   // innermsg->deleteLater();
    delete innermsg;
    return iret;
}

void QGenieDialogContainerFrame::passwordInvalidBox(QWidget *parent,const QString &title,
                                                    const QString &text,const QString &imagepath)
{
    QGenieInnerMsgBox *innermsg = new QGenieInnerMsgBox(parent,false,imagepath);
    innermsg->set_title(title);
    innermsg->set_text(text);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,innermsg,parent);
    dcf.exec();
}




//inner widget for message box

QGenieInnerMsgBox::QGenieInnerMsgBox(QWidget *parent,bool yesorno,const QString &imagepath)
    :QGenieDialogInnerWidget(parent),m_byesorno(yesorno)
{
    prompt_label =new QLabel(this);
    image_lb       = new QLabel(this);

    ok_btn		=new QPushButton(this);
    cancel_btn	=new QPushButton(this);

    prompt_label->setAlignment(Qt::AlignCenter);
    prompt_label->setWordWrap(true);
    if(!imagepath.isEmpty())
    {
        image_lb->setPixmap(QPixmap(imagepath));
    }
    else
    {
        image_lb->setVisible(false);
    }
    image_lb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    if(!m_byesorno)
    {
        cancel_btn->setVisible(false);
    }

    //SET_FIX_SIZE(NORMAL_BUTTON,ok_btn);
    //SET_FIX_SIZE(NORMAL_BUTTON,cancel_btn);

    QHBoxLayout	*hor_layout=new QHBoxLayout();
    QSpacerItem       *sp_item=new QSpacerItem(16,0,QSizePolicy::Fixed,QSizePolicy::Fixed);
    hor_layout->setContentsMargins(0,0,0,0);
    hor_layout->addStretch(1);
    hor_layout->addWidget(ok_btn);
    if(m_byesorno)
    {
        hor_layout->addItem(sp_item);
        hor_layout->addWidget(cancel_btn);
    }
    hor_layout->addStretch(1);

    QVBoxLayout *ver_layout=new QVBoxLayout(this);
    ver_layout->setContentsMargins(0,0,0,0);
    QHBoxLayout  *image_text_layout  = new QHBoxLayout();
    image_text_layout->addWidget(image_lb);
    image_text_layout->addWidget(prompt_label);
    ver_layout->addLayout(image_text_layout);
    ver_layout->addLayout(hor_layout);
    ver_layout->setStretch(0,1);
    ver_layout->setStretch(1,0);

    retranslateUi();

    connect(ok_btn,SIGNAL(clicked(bool)),this,SLOT(ok_btn_clicked(bool)));
    connect(cancel_btn,SIGNAL(clicked(bool)),this,SLOT(cancel_btn_clicked(bool)));
}

QGenieInnerMsgBox::~QGenieInnerMsgBox()
{

}

void QGenieInnerMsgBox::retranslateUi()
{
    //this code Fragment is plugin depent
       if(m_byesorno)
       {
           //ok_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_YES_TEXT));
           //cancel_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_NO_TEXT));
       }
       else
       {
           //ok_btn->setText(GeniePlugin_Wireless::translateText(L_WIRELESS_IMB_OK_TEXT));
           //cancel_btn->setText(QString());
       }
       //end
}

void QGenieInnerMsgBox::changeEvent(QEvent *event)
{
    if(event->type()==QEvent::LanguageChange)
        retranslateUi();
   QGenieDialogInnerWidget::changeEvent(event);
}


void QGenieInnerMsgBox::ok_btn_clicked(bool checked)
{
    accept();
}

void QGenieInnerMsgBox::cancel_btn_clicked(bool checked)
{
    reject();
}

QString QGenieInnerMsgBox::get_dlg_title()
{
    return m_strtitle;
}

void QGenieInnerMsgBox::set_title(const QString &title)
{
    m_strtitle = title;
}

void QGenieInnerMsgBox::set_text(const QString &text)
{
    m_strtext = text;
    prompt_label->setText(m_strtext);
}
