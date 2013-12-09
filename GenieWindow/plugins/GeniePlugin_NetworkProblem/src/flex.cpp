#include "flex.h"
#include "mainwindow.h"

Flex::Flex(const QString &lbl1,const QString &lbl2, const QString &mark, QWidget* parent)
{
    Mark = mark;
    this->setProperty("stylehintplugin",5);

    gridLayout_3 = new QGridLayout(this);
    gridLayout_3->setContentsMargins(-1, 0, -1, -1);
    gridLayout_3->setSpacing(0);

    frame = new QFrame(this);
    frame->setMinimumSize(QSize(0, 20));
    frame->setMaximumSize(QSize(16777215, 60));
    frame->setFrameShape(QFrame::NoFrame);
    frame->setFrameShadow(QFrame::Plain);
    frame->setAttribute(Qt::WA_TranslucentBackground,true);

    gridLayout = new QGridLayout(frame);
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(9, 0, 6, 0);

    label = new Flexlabel(frame);
    label->setMinimumSize(QSize(0, 20));
    label->setMaximumSize(QSize(16777215, 60));
    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    font.setWeight(75);
    label->setFont(font);
    label->setWordWrap(true);
    label->setText(lbl1);
    label->setAttribute(Qt::WA_TranslucentBackground,true);
    label->setProperty("stylehintplugin",88);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    pushButton = new PushButton(frame);
    pushButton->setAutoFillBackground(true);
    pushButton->setIcon(QIcon(":/Images/xt3.png"));
    pushButton->setFocusPolicy(Qt::NoFocus);
    pushButton->setFlat(true);
    pushButton->setAttribute(Qt::WA_TranslucentBackground,true);
	pushButton->setProperty("flexBut", 1);

    gridLayout->addWidget(pushButton, 0, 1, 1, 1);

	QSpacerItem *horizontalSpacer = new QSpacerItem(199, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	gridLayout->addItem(horizontalSpacer, 0, 0, 1, 1);

    ////
    //QString filepath =":/Images/xt5.png";
    //twidget = new TWidget(frame,filepath);
    //gridLayout->addWidget(twidget, 1, 0, 1, 2);

    gridLayout_3->addWidget(frame, 0, 0, 1, 1);





    frame_2 = new QFrame(this);
    frame_2->setFrameShape(QFrame::NoFrame);
    frame_2->setFrameShadow(QFrame::Plain);
    frame_2->setAttribute(Qt::WA_TranslucentBackground,true);

    gridLayout_2 = new QGridLayout(frame_2);
    gridLayout_2->setSpacing(0);
    gridLayout_2->setContentsMargins(-1, 0, 20, 0);

    label_2 = new QLabel(frame_2);
    label_2->setMinimumSize(QSize(20, 0));
    label_2->setMaximumSize(QSize(20, 16777215));
    label_2->setAttribute(Qt::WA_TranslucentBackground,true);

    gridLayout_2->addWidget(label_2, 0, 0, 1, 1);

    label_3 = new QLabel(frame_2);
    QFont font1;
    font1.setPointSize(10);
    label_3->setFont(font1);
    label_3->setText(lbl2);
    label_3->setWordWrap(true);
    label_3->setAttribute(Qt::WA_TranslucentBackground,true);
    label_3->setProperty("stylehintplugin",99);

    gridLayout_2->addWidget(label_3, 0, 1, 1, 1);

    label_space = new QLabel(frame_2);
    label_space->setMinimumSize(QSize(0, 10));
    label_space->setMaximumSize(QSize(16777215, 10));
    label_space->setAttribute(Qt::WA_TranslucentBackground,true);
    gridLayout_2->addWidget(label_space, 1, 0, 1, 2);


    gridLayout_3->addWidget(frame_2, 1, 0, 1, 1);

    frame_2->hide();

    QString filepath =":/Images/xt5.png";
    twidget = new TWidget(this, filepath);
    gridLayout_3->addWidget(twidget, 2, 0, 1, 1);



    label_4 = new QLabel;
    label_4->setText("+");

    connect(pushButton,SIGNAL(clicked()),this,SLOT(btnchang()));

    connect(label,SIGNAL(linkActivated(QString)),this,SLOT(openUrl(QString)));
    connect(label,SIGNAL(labelclick()),this,SLOT(labelflex()));
    connect(label_3,SIGNAL(linkActivated(QString)),this,SLOT(openUrl(QString)));

}

Flex::~Flex()
{

}

void Flex::btnchang()
{
    if(label_4->text() =="+")
    {
        //////told server to const
        //QString mOper = "NQ1";
//        IClickManager* clickmanage = MainWindow::sHandler->getClickManager();
//        clickmanage->click(Mark);
        GENIE2_STATISTICS(Mark);

        pushButton->setCheckable(true);
        pushButton->setChecked(true);
        frame_2->show();
        label_4->setText("-");
        pushButton->setIcon(QIcon(":/Images/xt2.png"));
    }
    else
    {
        if(label_4->text() =="-")
        {
            pushButton->setCheckable(false);
            pushButton->setChecked(false);
            pushButton->setIcon(QIcon(":/Images/xt3.png"));
            label_4->setText("+");
            frame_2->hide();
        }
    }
}

void Flex::labelchange(QString text,QString text_2)
{
    label->setText(text);
    label_3->setText(text_2);
}

void Flex::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void Flex::labelflex()
{
    btnchang();
}
