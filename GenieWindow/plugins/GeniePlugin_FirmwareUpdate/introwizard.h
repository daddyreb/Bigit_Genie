#ifndef INTROWIZARD_H
#define INTROWIZARD_H

#include <QWidget>
#include "wizardpage.h"
#include "QProgressIndicator.h"

//#include "IManagerHandler.h"

class IntroWizard : public WizardPage
{
    Q_OBJECT
public:
    IntroWizard(QWidget *parent=0);
    ~IntroWizard();

    void SetInfo(const QString &Info);
    void SetInfo(const int &intInfo);
    void ClearInfo();
    void SetTitle(QString &type, bool setbtn);
    void isCancelBtn(bool isFlag);

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

public slots:
    void openView();
    void closeView();

//signals:
//    void IntroToNext();

private:
    void FirstSetForm();
    ILanguageManager *lanmanager;
    int mExplain;

    int intTitle;
    int intExplain;
    QString routerType;

    QWidget *m_Widget;
    QLabel *vPrompt;
    QLabel *vTimeOut;

    QProgressIndicator* mProgress;

};

#endif // INTROWIZARD_H
