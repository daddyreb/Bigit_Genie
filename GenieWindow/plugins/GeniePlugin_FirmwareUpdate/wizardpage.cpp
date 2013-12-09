#include <QtGui>
#include "wizardpage.h"

#include "plugin_ui_def.h"

#include "IInfoManager.h"
#include "info_defs.h"
#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"


IManagerHandler *WizardPage::sHandler = NULL;

WizardPage::WizardPage(QWidget *parent) :
        QWidget(parent)
//WizardPage::WizardPage()
{
    mFlag = true;
    intTime = 500;
    //////Use set router image
    //changeTimer = new QTimer();
    //changeTimer->start(intTime);
    //connect(changeTimer, SIGNAL(timeout()), this, SLOT(ChangeImage()));

    TopLayout = new QHBoxLayout();

    LeftWidget = new QWidget();
    LeftWidget->setFixedSize(206, 202);
    //LeftWidget->setAutoFillBackground(false);
    l_WidgetLayout = new QVBoxLayout(LeftWidget);


    //////2010-10-13 Jansion
    ////Setting view router Images
//    l_Image = new QLabel(LeftWidget);
//    //l_Image->setFixedSize(206, 202);
//    l_Image->setAutoFillBackground(false);
//    l_Image->setAlignment(Qt::AlignCenter);
//
//    //l_Image->setPixmap(QPixmap(":/images/firmware.png"));
//    //l_Image->setPixmap(QPixmap(iconPath));
//
//    strPicA = "background-image:url(:/route/WNDR3700big_3_1.png);background-repeat:none;background-position:center;";
//    strPicB = "background-image:url(:/route/WNDR3700big_3_2.png);background-repeat:none;background-position:center;";
//    //strPicA = "background-image:url(:/route/" + iconPathA + ");";
//    //strPicB = "background-image:url(:/route/" + iconPathB + ");";
//    //l_Image->setStyleSheet(strPicA);

    ////Setting view router Flash
    //FlashWidget = new QGenieRouteWidget_B3();
    FlashWidget = new QGenieRouteWidget_B3(LeftWidget);
    FlashWidget->setInfoManager(sHandler->getInfoManager());
    //FlashWidget->setFixedSize(206, 202);

    //l_WidgetLayout->addStretch();
    //l_WidgetLayout->addWidget(l_Image);
    l_WidgetLayout->addWidget(FlashWidget);
    //l_WidgetLayout->addStretch();


    //QSpacerItem *SplitLine1 = new QSpacerItem(1,100,QSizePolicy::Minimum,QSizePolicy::Expanding);
    QFrame *SplitLine1 = new QFrame();
    //SplitLine1->setGeometry(QRect(143, 70, 10, 210));
    SplitLine1->setFixedSize(2, 210);
    SplitLine1->setFrameShape(QFrame::VLine);
    SplitLine1->setFrameShadow(QFrame::Sunken);
    SplitLine1->setObjectName("Splitline1");


    RightWidget = new QWidget();
    RightWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    r_WidgetLayout = new QVBoxLayout(RightWidget);
    m_Title = new QLabel(RightWidget);
    m_Title->setObjectName("UpdateTitle");
    //m_Title->setText(tr("Title"));
    QSpacerItem *SplitItem = new QSpacerItem(0,2,QSizePolicy::Expanding,QSizePolicy::Maximum);
    m_Intro = new QLabel(RightWidget);
    m_Intro->setObjectName("UpdateIntro");
    //m_Intro->setText(tr("Explain"));
    m_Intro->setWordWrap(true);

    m_setWidget = new QWidget(RightWidget);
    m_setWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    r_WidgetLayout->addStretch();
    r_WidgetLayout->addWidget(m_Title);
    r_WidgetLayout->addItem(SplitItem);
    r_WidgetLayout->addWidget(m_Intro);
    r_WidgetLayout->addWidget(m_setWidget);
    r_WidgetLayout->addStretch();


    TopLayout->addWidget(LeftWidget);
    //TopLayout->addWidget(FlashWidget);
    //TopLayout->addItem(SplitLine1);
    TopLayout->addWidget(SplitLine1);
    TopLayout->addWidget(RightWidget);


    //QSpacerItem *SplitLine2 = new QSpacerItem(100,1,QSizePolicy::Fixed,QSizePolicy::Fixed);
    QFrame *SplitLine2 = new QFrame();
    //SplitLine2->setGeometry(QRect(143, 70, 10, 141));
    SplitLine2->setFixedHeight(2);
    SplitLine2->setFrameShape(QFrame::HLine);
    SplitLine2->setFrameShadow(QFrame::Sunken);
    SplitLine2->setObjectName("Splitline2");


    BelowWidget = new QWidget();
    //BelowWidget->setStyleSheet("background-color: red");
    BelowWidget->setMinimumHeight(33);
    BelowWidget->setContentsMargins(0, 0, 0, 0);
    ButtonLayout = new QHBoxLayout(BelowWidget);
    //ButtonLayout = new QHBoxLayout();
    //ButtonLayout->setGeometry(QRect(140, 900, 175, 23));

    BackButton = new QPushButton(BelowWidget);
    //BackButton->setGeometry(QRect(330, 10, 75, 23));
    //BackButton->setFocusPolicy(Qt::TabFocus);
    BackButton->setFocusPolicy(Qt::NoFocus);
    //SET_FIX_SIZE(NORMAL_BUTTON, BackButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON, BackButton);
    BackButton->setHidden(true);
    //BackButton->setFixedWidth(60);
    //BackButton->setText(tr("Back"));

    NextButton = new QPushButton(BelowWidget);
    //NextButton->setGeometry(QRect(425, 10, 75, 23));
    //NextButton->setFocusPolicy(Qt::TabFocus);
    NextButton->setFocusPolicy(Qt::NoFocus);
    //SET_FIX_SIZE(NORMAL_BUTTON, NextButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON, NextButton);
    //NextButton->setFixedWidth(60);
    //NextButton->setText(tr("Next"));

    CancelButton = new QPushButton(BelowWidget);
    //CancelButton->setGeometry(QRect(520, 10, 75, 23));
    //CancelButton->setFocusPolicy(Qt::TabFocus);
    CancelButton->setFocusPolicy(Qt::NoFocus);
    //SET_FIX_SIZE(NORMAL_BUTTON, CancelButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON, CancelButton);
    //CancelButton->setFixedWidth(60);
    //CancelButton->setText(tr("Cancel"));

    SpaceButt = new QLabel(BelowWidget);
    //SpaceButt->setGeometry(QRect(140, 290, 75, 23));
    SpaceButt->setObjectName("SpaceButton");
    //SpaceButt->setText(tr("None"));
    //SpaceButt->setFixedWidth(60);
    SpaceButt->setHidden(true);

    ButtonLayout->addStretch();
//    ButtonLayout->addWidget(BackButton, 1, Qt::AlignCenter);
//    ButtonLayout->addWidget(NextButton, 2, Qt::AlignCenter);
//    ButtonLayout->addWidget(CancelButton, 3, Qt::AlignCenter);
    ButtonLayout->addWidget(BackButton);
    ButtonLayout->addWidget(NextButton);
    ButtonLayout->addWidget(CancelButton);
    ButtonLayout->addWidget(SpaceButt);


    WholeLayout = new QVBoxLayout();
    WholeLayout->addLayout(TopLayout);
    //WholeLayout->addItem(SplitLine2);
    WholeLayout->addWidget(SplitLine2);
    WholeLayout->addWidget(BelowWidget);
    //WholeLayout->addLayout(ButtonLayout);
    WholeLayout->setContentsMargins(0, 0, 0, 0);

    connect(BackButton, SIGNAL(clicked()), this, SLOT(TurnBack()));
    connect(NextButton, SIGNAL(clicked()), this, SLOT(TurnNext()));
    connect(CancelButton, SIGNAL(clicked()), this, SLOT(TurnCancel()));


    setLayout(WholeLayout);

}

WizardPage::~WizardPage()
{

}

void WizardPage::setTitle(QString mTitle)
{
    m_Title->setText(mTitle);
}

void WizardPage::setExplain(QString mExplain)
{
    m_Intro->setText(mExplain);
}

void WizardPage::setImage(QPixmap mImage)
{
    l_Image->setPixmap(mImage);
}

QWidget* WizardPage::isSetArea()
{
    //if (m_setWidget!=NULL)
        return m_setWidget;
}

void WizardPage::TurnNext()
{
    NextButton->setDisabled(true);
    emit ToNext();
}

void WizardPage::TurnBack()
{
    //BackButton->setDisabled(true);
    emit ToBack();
}

void WizardPage::TurnCancel()
{
    //IUiManager *mUimanager = FirmwareUpdate::sHandler->getUiManager();
    //mUimanager->changeStackIdx(0);

    //CancelButton->setDisabled(true);
    emit ToCancel();
}

void WizardPage::setButtonEnabled(int index)
{
    if (index==0){
        //BackButton->setDisabled(true);
        NextButton->setDisabled(false);
    }else if (index==4){
        //BackButton->setDisabled(false);
        NextButton->setDisabled(false);
    }else {
        //BackButton->setDisabled(false);
        NextButton->setDisabled(false);
        //CancelButton->setDisabled(false);
    }
}

void WizardPage::ChangeImage()
{
//    if (mFlag){
//        l_Image->setStyleSheet(strPicB);
//        mFlag = false;
//    }else {
//        l_Image->setStyleSheet(strPicA);
//        mFlag = true;
//    }

}

void WizardPage::setImageView(QString strA, QString strB)
{
    strPicA = "background-image:url(" + strA + ");background-repeat:none;background-position:center;";
    strPicB = "background-image:url(" + strB + ");background-repeat:none;background-position:center;";
}

void WizardPage::stopTimer()
{
    //changeTimer->stop();
}

//void WizardPage::changePic(QString mName)
//{
//    QString picName;
//    picName = mName;
//    qDebug() << "Change pictrue name: " << picName;
//
//    //l_Image->setPixmap(QPixmap(":/images/firmware.png"));
//    //l_Image->clear();
//
//}

void WizardPage::ReloadFlash()
{
    FlashWidget->reloadImage();
    //FlashWidget->setLogManager();
}

