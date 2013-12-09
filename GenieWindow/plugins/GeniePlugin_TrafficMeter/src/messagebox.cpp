#include "messagebox.h"
#include "mainwindow.h"
#include "inc/plugin_ui_def/plugin_ui_def.h"

MessageBox::MessageBox(QString& message,QString& title)
{
    this->resize(228, 117);
    this->setModal(true);
    this->setWindowFlags(Qt::Dialog| Qt::FramelessWindowHint | Qt::ToolTip);
    this->setAttribute(Qt::WA_TranslucentBackground);

    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    gridLayout_2 = new QGridLayout(frame);

    horizontalLayout = new QHBoxLayout();

    label = new QLabel(frame);
    label->setMinimumSize(QSize(38, 0));
    label->setMaximumSize(QSize(38, 16777215));

    horizontalLayout->addWidget(label);

    label_2 = new QLabel(frame);
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    label_2->setFont(font);
    label_2->setText(title);
    label_2->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
    label_2->setProperty("overstriking",1);

    horizontalLayout->addWidget(label_2);

    gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 3);

    label_3 = new QLabel(frame);
    label_3->setWordWrap(true);
    label_3->setText(message);
    label_3->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    gridLayout_2->addWidget(label_3, 1, 0, 1, 3);

    horizontalSpacer = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer, 2, 0, 1, 1);

    pushButton = new QPushButton(frame);
    pushButton->setFocusPolicy(Qt::NoFocus);
    pushButton->setText(QString(tr("OK")));
    SET_STYLE_PROPERTY(NORMAL_BUTTON ,pushButton);

    gridLayout_2->addWidget(pushButton, 2, 1, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_2, 2, 2, 1, 1);

    gridLayout->addWidget(frame, 0, 0, 1, 1);

    QPixmap pix;
    pix.load(":/Images/gai1.png",0,Qt::AvoidDither | Qt::ThresholdDither | Qt::ThresholdAlphaDither);
    int ew = this->size().width();
    int eh = this->size().height();
    QSize size(ew,eh);
    QPixmap pixmap(pix.scaled(size));
    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(pixmap));
    frame->setPalette(p);

    uuid ="{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}";

    connect(pushButton,SIGNAL(clicked()),this,SLOT(close()));

    retranslateUi();
}
MessageBox::~MessageBox()
{

}
void MessageBox::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    switch(event->type())
    {
        case QEvent::LanguageChange:
            retranslateUi();
            break;
        default:
                break;
    }
}
void MessageBox::retranslateUi()
{
//    lanmanager=MainWindow::sHandler->getLanguageManager();
//    pushButton->setText(lanmanager->getText(L_ROUTERCONFIGURATION_BOX_OK,uuid));
}
void MessageBox::resizeEvent(QResizeEvent *event)
{
    frame->setAutoFillBackground(true);
    QPixmap pix;
    pix.load(":/Images/gai1.png",0,Qt::AvoidDither | Qt::ThresholdDither | Qt::ThresholdAlphaDither);
    int ew = this->size().width();
    int eh = this->size().height();
    QSize size(ew,eh);
    QPixmap pixmap(pix.scaled(size));
    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(pixmap));
    frame->setPalette(p);
}
void MessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        QPoint newpos = event->globalPos();
        QPoint upleft = pos0 + newpos - last;
        this->move(upleft);
    }
}
void MessageBox::mousePressEvent(QMouseEvent *event)
{
    last = event->globalPos();
    pos0 = event->globalPos() - event->pos();
}
void MessageBox::labelchange(QString message, QString title)
{
    label_2->setText(title);

    label_3->setText(message);
}
//void MessageBox::changwidget()
//{
//    if(pushButton->isChecked())
//    {
//        emit changePage();
//    }
//}
