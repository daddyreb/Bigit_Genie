#ifndef FIRMWAREUPDATE_H
#define FIRMWAREUPDATE_H

#include <QWidget>
#include <QMap>
#include "IManagerHandler.h"

#include "introwizard.h"
#include "checkwizard.h"
#include "downloadwizard.h"
//#include "updatewizard.h"
#include "completewizard.h"
#include "finishwizard.h"

class QStackedWidget;
class QVBoxLayout;

class InfoFirmware;
class CheckFirmware;

class FirmwareUpdate : public QWidget
{
    Q_OBJECT

public:
    FirmwareUpdate(QWidget *parent = 0);
    ~FirmwareUpdate();

    QString getProductInfo(const QString &per);
    QString getUpgradeStat();
    void changePagePic();
    void changePagePic(QString &mpic);
    void changePagePic(QString &pathA, QString &pathB);
    void turnFirstPage();
    void setFirstPageTitle(QString &type);
    void turnUpgradePage(QString model, QString twoVer, QString url, QString twoComp);

public slots:
    void PageToHome();

private slots:
    void PageToNext();
    void PageToBack();

    void FinishToHome();
    void UpgradeComplete();

    void SetNetgear();
    void NotNetgear();
    void CheckResult(bool err);
    void DownloadOver(bool err);

    void viewGetNewTimeout(int stat);

signals:
    void GetFinish(QString Info);

public:
    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private:
    void GetInfoFromPlugin();
    bool CheckUpgrade(QString mOld, QString mNew);
    void TurnToDownload();
    bool isChecked;
    bool isUpgrade;
    bool isStatus;
    bool isNetgearFlag;
    bool isIntroNextBtn;
    bool isFrameTurn;
    bool isDownloading;
    void popWidget();
    void turnPagebyChangeModel();

    void TranslateUi();

    QVBoxLayout *WholeLayout;
    QStackedWidget *WizardLayout;

    InfoFirmware *NetgearProduct;
    CheckFirmware *ProductCheck;

    QMap<QString, QString> *ProductInfo;
    QMap<QString, QString> *CheckInfo;
    QMap<QString, QString> VersionInfo;
    //QString *rError;
    QList<int> *intErr;
    QList<int> *intCheckErr;
    QString *DownInfo;
    QList<int> *intDownloadErr;
    QString mProductInfo;
    QString mRegionTag;
    QString mCurrentModel;

    QString currentType;
    QString currentVersion;

    QStringList mDownPara;
    QString UpdateUrl;


    IntroWizard *IntroPage;
    CheckWizard *CheckPage;
    DownloadWizard *DownloadPage;
    //UpdateWizard *UpdatePage;
    CompleteWizard *CompletePage;
    FinishWizard *FinishPage;

    IUiManager *mUimanager;
    IInfoManager *mInfoManager;

};



#endif // FIRMWAREUPDATE_H
