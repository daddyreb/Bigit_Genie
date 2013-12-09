#include <QtGui>

#include "completewizard.h"

#include "plugin_ui_def.h"

#include "IUiManager.h"
#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *CompleteWizard::sHandler = NULL;

CompleteWizard::CompleteWizard(QWidget *parent) :
        WizardPage(parent)
{

    setTitle(tr("Firmware Downloaded"));
    setExplain(tr("The lastest router firmware is downloaded to:"));
    //NextButton->setText(tr("Update"));

//    setExplain(tr("The New Firmware!"));
    m_Widget = isSetArea();
    QVBoxLayout *mLayout = new QVBoxLayout(m_Widget);

    //viewTitle = new QLabel(m_Widget);
    //viewTitle->setAlignment(Qt::AlignTop);
    //viewTitle->setText(tr("Download Path:"));
    viewPath = new QLabel(m_Widget);
//    viewPath->setFixedSize(100, 60);
//    viewPath->setMaximumHeight(100);
//    viewPath->setMinimumHeight(100);
    viewPath->setAlignment(Qt::AlignTop);
    //viewPath->setFixedWidth(60);
    viewPath->setWordWrap(true);


    QHBoxLayout *BntLayout = new QHBoxLayout(m_Widget);
    copyButton = new QPushButton(m_Widget);
    copyButton->setText("Copy Path");
    //copyButton->setFocusPolicy(Qt::TabFocus);
    copyButton->setFocusPolicy(Qt::NoFocus);
    copyButton->setHidden(true);
    //SET_FIX_SIZE(NORMAL_BUTTON, copyButton);
    SET_STYLE_PROPERTY(FLEX_BUTTON, copyButton);
    connect(copyButton, SIGNAL(clicked()), this, SLOT(CopyFilePath()));
    BntLayout->addStretch();
    BntLayout->addWidget(copyButton);


    //mLayout->addWidget(viewTitle);
    mLayout->addWidget(viewPath);
    mLayout->addLayout(BntLayout);
    mLayout->addStretch();


    //CancelButton->setHidden(true);
    //SpaceButt->setHidden(false);

}

CompleteWizard::~CompleteWizard()
{

}

void CompleteWizard::SetInfo(const QString &Info)
{
    strPath = Info;
    if (!Info.isEmpty())
        copyButton->setHidden(false);
    else
        copyButton->setHidden(true);

    QString strView = Info;
    int numStr = strView.size();
    qDebug() << "String size: " << numStr;
    if ( numStr > 40 )
        strView.insert(40, QString("\n   "));
    for (int i=77; i<numStr; ){
        strView.insert(i, QString("\n   "));
        i += 37;
    }
    viewPath->setText(strView);

    //viewPath->setText(Info);

//    ViewInfo->setHidden(false);
//    m_Widget->setHidden(false);

}

void CompleteWizard::SetFinishButton()
{
    copyButton->setHidden(true);
}

void CompleteWizard::CopyFilePath()
{
    IUiManager* uimanager = sHandler->getUiManager();
    uimanager->setClipBoardString(strPath);
}


void CompleteWizard::changeEvent(QEvent *e)
{
    ILanguageManager *lanmanager = CompleteWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        CompleteWizard::m_Title->setText(lanmanager->getText(L_UPGRADE_COMPLETE_TITLE, PLUGIN_ID));
        CompleteWizard::m_Intro->setText(lanmanager->getText(L_UPGRADE_COMPLETE_PATH, PLUGIN_ID));
        //CompleteWizard::m_Intro->setText(lanmanager->getText(L_UPGRADE_COMPLETE_EXPLAIN, PLUGIN_ID));
        copyButton->setText(lanmanager->getText(L_UPGRADE_FINISH_COPYBTN, PLUGIN_ID));

        WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
        WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT, PLUGIN_ID));
        //WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT_FINISH, PLUGIN_ID));
        WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
        break;
    default:
        break;
    }

}


