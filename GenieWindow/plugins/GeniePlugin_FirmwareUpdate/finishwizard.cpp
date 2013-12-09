#include <QtGui>

#include "finishwizard.h"
#include "plugin_ui_def.h"

#include "IUiManager.h"
#include "ILanguageManager.h"
#include "LanguageFirmwareUpdate.h"

#define PLUGIN_ID "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}"

IManagerHandler *FinishWizard::sHandler = NULL;

FinishWizard::FinishWizard(QWidget *parent) :
        WizardPage(parent)
{

    //setTitle(tr("Finished"));
    //setExplain(tr("The Router Upgrade is over, and Under Information is upgrade's."));
    lanmanager = FinishWizard::sHandler->getLanguageManager();

    BackButton->setHidden(true);
    //NextButton->setText(tr("Finish"));
    CancelButton->setHidden(true);
    SpaceButt->setHidden(false);

//    setExplain(tr("The New Firmware!"));
//    m_Widget = isSetArea();
    m_Widget = isSetArea();
    QVBoxLayout *mLayout = new QVBoxLayout(m_Widget);

    ViewInfo = new QLabel(m_Widget);
    ViewInfo->setWordWrap(true);
    ViewInfo->setHidden(true);

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

    mLayout->addWidget(ViewInfo);
    mLayout->addLayout(BntLayout);
    m_Widget->setHidden(true);


}

FinishWizard::~FinishWizard()
{

}

//void FinishWizard::SetInfo(const QString &Info)
//{
//    ViewInfo->setText(Info);
//
//    ViewInfo->setHidden(false);
//    m_Widget->setHidden(false);
//
//    //NextButton->setDisabled(false);
//
//}

void FinishWizard::SetInfo(const QList<int> &intInfo, QString mPath)
{
    vInfo = intInfo;
    strPath = mPath;
    mFilePath = mPath;
    
    m_Title->setText(lanmanager->getText(vInfo[0], PLUGIN_ID));
    m_Intro->setText(lanmanager->getText(vInfo[1], PLUGIN_ID));

    QString tempView;
    for(int i=2; i<vInfo.size(); i++){
        tempView += lanmanager->getText(vInfo.value(i), PLUGIN_ID);
    }
    if ( mFilePath!="NULL" ){
        copyButton->setHidden(false);
        int numStr = mFilePath.size();
        if ( numStr > 40 )
            mFilePath.insert(40, QString("\n   "));
        for (int i=77; i<numStr; ){
            mFilePath.insert(i, QString("\n   "));
            i += 37;
        }
        tempView.append("\n");
        tempView += mFilePath;
    }
    ViewInfo->setText(tempView);

    ViewInfo->setHidden(false);
    m_Widget->setHidden(false);

    //NextButton->setDisabled(false);
}

void FinishWizard::CopyFilePath()
{
    IUiManager* uimanager = sHandler->getUiManager();
    uimanager->setClipBoardString(strPath);
}

void FinishWizard::SetFinishButton()
{
    NextButton->setDisabled(false);
    copyButton->setHidden(true);
}

void FinishWizard::changeEvent(QEvent *e)
{
    //ILanguageManager *lanmanager = FinishWizard::sHandler->getLanguageManager();
    QWidget::changeEvent(e);

    switch (e->type()){
    case QEvent::LanguageChange:
        {
            //FinishWizard::m_Title->setText(lanmanager->getText(vInfo[0], PLUGIN_ID));
            //FinishWizard::m_Intro->setText(lanmanager->getText(vInfo[1], PLUGIN_ID));

            QString viewInfo;
            for(int i=2; i<vInfo.size(); i++){
                viewInfo += lanmanager->getText(vInfo.value(i), PLUGIN_ID);
            }
            if ( mFilePath!="NULL" ){
                viewInfo.append("\n");
                viewInfo += mFilePath;
            }
            FinishWizard::ViewInfo->setText(viewInfo);

            m_Title->setText(lanmanager->getText(vInfo.value(0), PLUGIN_ID));
            m_Intro->setText(lanmanager->getText(vInfo.value(1), PLUGIN_ID));

            copyButton->setText(lanmanager->getText(L_UPGRADE_FINISH_COPYBTN, PLUGIN_ID));

            WizardPage::BackButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_BACKBUT, PLUGIN_ID));
            WizardPage::NextButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_NEXTBUT_FINISH, PLUGIN_ID));
            WizardPage::CancelButton->setText(lanmanager->getText(L_UPGRADE_WIZARD_CANCELBUT, PLUGIN_ID));
            break;
        }
    default:
        break;
    }

}
