#include "QGenieDialogContainerFrame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <assert.h>
#include "genie2_interface.h"
#include "language_Internet.h"
#include "GeniePlugin_Internet.h"

/*const char *cFramelessQss=   "QDialog > QWidget{"
        "border-image: url(:/images/bg_dialog.png);"
"border-width: 44px 16px 6px 59px;"
"padding-top: -44px;"
"padding-right: -16px;"
"padding-bottom: -6px;"
"padding-left: -59px;"

"border-radius: 0px;"
        "}";
*/
		
QList<QGenieDialogContainerFrame*>     QGenieDialogContainerFrame::m_dcfs_list;		

QGenieDialogContainerFrame::QGenieDialogContainerFrame(Dlg_Size_Model size_model,QGenieDialogInnerWidget* inner,QWidget *parent) :
        QDialog(parent)
        ,m_inner_widget(inner)
        ,m_bDrag(false)
{
    m_inner_widget->setParent(this);
    this->setFocusProxy(m_inner_widget);
    m_inner_widget->set_container_dlg(this);
    m_inner_widget->setFocusPolicy(Qt::StrongFocus);

    GENIE2_SET_FOCUS(m_inner_widget);
//    m_inner_widget->setFocus();
//    init_dim(size_model);

//#ifdef Q_OS_WIN
//    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint );
//#elif defined Q_OS_MACX
//    this->setWindowFlags(Qt::Sheet | Qt::FramelessWindowHint );
//    //event_loop = new QEventLoop(this);
//#endif
//    this->setAttribute(Qt::WA_TranslucentBackground);

    m_framecontainer = new QFrame(this);
    m_framecontainer->setObjectName(QString("iframe"));
    m_framecontainer->setAutoFillBackground(true);
   // m_framecontainer->setStyleSheet(cFramelessQss);

    QVBoxLayout  *dlg_layout = new QVBoxLayout(this);
    dlg_layout->setContentsMargins(0,0,0,0);
    dlg_layout->addWidget(m_framecontainer);

    m_dlgtitle_lb = new QLabel(m_framecontainer);
    m_dlgtitle_lb->setObjectName("label_title");
//    init_dlgtitle(size_model);


    QSpacerItem  *title_spitem = new QSpacerItem(40,0,QSizePolicy::Fixed);
    QHBoxLayout *title_layout  = new QHBoxLayout();
//    title_layout->setContentsMargins(0,0,0,0);
    title_layout->addItem(title_spitem);
    title_layout->addWidget(m_dlgtitle_lb);


    QVBoxLayout   *container_layout = new QVBoxLayout(m_framecontainer);
    container_layout->setContentsMargins(14,15,14,11);
    container_layout->setSpacing(1);
    container_layout->addLayout(title_layout);
    m_inner_widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    container_layout->addWidget(m_inner_widget);

    container_layout->setStretch(0,0);
    container_layout->setStretch(1,10);

    const struct _{int w;int h;} cParas[]={
        {538,452}
        ,{450,277}
        ,{383,212}
        ,{228,117}
    };
    resize(cParas[size_model].w,cParas[size_model].h);

    GENIE2_SET_FRAMELESSSTYLE(this);
    retranslateUi();
}

//void QGenieDialogContainerFrame::init_dim(Dlg_Size_Model size_model)
//{
//    switch(size_model)
//    {
//    case Medium:
//        m_dlg_width                         = 450;
//        m_dlg_height                        = 277;
//        m_dlgtitle_lb_height                = 40;
//        m_dlgtitle_lb_spaceitemw            = 60;
//        m_frame_lr_margin                   = 14;
////        m_frame_tb_margin                   = 13;
////#ifdef Q_OS_WIN
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/MediumDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#elif defined Q_OS_MACX
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/Mac_MediumDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#endif
////        m_framecontainer_style=cFramelessQss;
//        break;
//    case Normal:
//        m_dlg_width                             = 383;
//        m_dlg_height                            = 212;
//        m_dlgtitle_lb_height                    = 32;
//        m_dlgtitle_lb_spaceitemw                = 60;
//        m_frame_lr_margin                       = 12;
////        m_frame_tb_margin                       = 11;
////#ifdef Q_OS_WIN
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/NormalDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#elif defined Q_OS_MACX
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/Mac_NormalDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#endif
////        m_framecontainer_style=cFramelessQss;
//        break;
//    case Small:
//        m_dlg_width                     = 228;
//        m_dlg_height                    = 117;
//        m_dlgtitle_lb_height            = 26;
//        m_dlgtitle_lb_spaceitemw        = 45;
//        m_frame_lr_margin               = 10;
////        m_frame_tb_margin               = 11;
////#ifdef Q_OS_WIN
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/SmallDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#elif defined Q_OS_MACX
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/Mac_SmallDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#endif
////        m_framecontainer_style=cFramelessQss;
//        break;
//    case Large:
//    default:
//        m_dlg_width                             = 538;
//        m_dlg_height                            = 452;
//        m_dlgtitle_lb_height                    = 46;
//        m_dlgtitle_lb_spaceitemw                = 63;
//        m_frame_lr_margin                       = 17;
////        m_frame_tb_margin                       = 16;
////#ifdef Q_OS_WIN
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/BigDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#elif defined Q_OS_MACX
////        m_framecontainer_style = QString::fromUtf8("#frame_container{background-image: url(:/image/Mac_BigDlgBkg.png);color:rgb(0,0,0);font-size:12px;}");
////#endif
////        m_framecontainer_style=cFramelessQss;
//    }

//}

//void QGenieDialogContainerFrame::init_dlgtitle(Dlg_Size_Model size_model)
//{
//    m_dlgtitle_lb->setObjectName(QString("dlg_title_lb"));
//    m_dlgtitle_lb->setFixedHeight(m_dlgtitle_lb_height);
//    m_dlgtitle_lb->setAlignment(Qt::AlignVCenter);
//    QString stylesheet =QString("#dlg_title_lb{color:rgb(255,255,255);%1;font-weight:bold;}");
//    switch(size_model)
//    {
//    case Medium:
//        stylesheet=stylesheet.arg("font-size:16px");
//        break;
//    case Normal:
//        stylesheet=stylesheet.arg("font-size:14px");
//        break;
//    case Small:
//        stylesheet=stylesheet.arg("font-size:12px");
//        break;
//    case Large:
//        stylesheet=stylesheet.arg("font-size:18px");
//    default:
//        stylesheet=stylesheet.arg("font-size:18px");
//    }
//    m_dlgtitle_lb->setStyleSheet(stylesheet);
//}

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
//#ifdef Q_OS_WIN
    if((event->buttons() & Qt::LeftButton) && m_bDrag)
    {
        QPoint newpos = event->globalPos();
        QPoint upleft = pos0 + newpos - last;
        this->move(upleft);
    }
//#endif
}

void QGenieDialogContainerFrame::mousePressEvent(QMouseEvent *event)
{
    last = event->globalPos();
    pos0 = event->globalPos() - event->pos();
    m_bDrag = true;
}

void QGenieDialogContainerFrame::mouseReleaseEvent(QMouseEvent *event)
{
    m_bDrag = false;
}

int QGenieDialogContainerFrame::messageBox(QWidget *parent,const QString &title,const QString &text,bool byesorno)
{
    QGenieInnerMsgBox *innermsg = new QGenieInnerMsgBox(parent,byesorno);
    innermsg->set_title(title);
    innermsg->set_text(text);
    QGenieDialogContainerFrame dcf(QGenieDialogContainerFrame::Normal,innermsg,parent);
    m_dcfs_list.append(&dcf);
    int iret = dcf.exec();
    m_dcfs_list.removeAll(&dcf);
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
    m_dcfs_list.append(&dcf);
    dcf.exec();
    m_dcfs_list.removeAll(&dcf);
}

int QGenieDialogContainerFrame::exec()
{
//    assert (g_uimanager);
//    g_uimanager->command(UI_CMD_SHOWNORMAL,"");
    this->activateWindow();
    return QDialog::exec();
}

//#ifdef Q_OS_MACX
//void QGenieDialogContainerFrame::accept()
//{
//    QDialog::accept();
//    event_loop->exit(this->result());
//}

//void QGenieDialogContainerFrame::reject()
//{
//    QDialog::reject();
//    event_loop->exit(this->result());
//}

//int QGenieDialogContainerFrame::exec()
//{
//    this->show();
//    this->activateWindow();
//    return event_loop->exec();
//}
//#endif

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

    //SET_STYLE_PROPERTY(FLEX_BUTTON,ok_btn);
    //SET_STYLE_PROPERTY(FLEX_BUTTON,cancel_btn);

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
        ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_IMB_YES_TEXT));
        cancel_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_IMB_NO_TEXT));
    }
    else
    {
        ok_btn->setText(GeniePlugin_Internet::translateText(L_INTERNET_IMB_OK_TEXT));
        cancel_btn->setText(QString());
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
