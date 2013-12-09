#ifndef FINISHWIZARD_H
#define FINISHWIZARD_H

#include <QWidget>
#include "wizardpage.h"


class FinishWizard : public WizardPage
{
    Q_OBJECT
public:
    FinishWizard(QWidget *parent=0);
    ~FinishWizard();

    //void SetInfo(const QString &Info);
    void SetInfo(const QList<int> &intInfo, QString mPath);
    void SetFinishButton();

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private slots:
    void CopyFilePath();

private:
    ILanguageManager *lanmanager;
    QList<int> vInfo;
    QString mFilePath;
    QString strPath;

    QWidget *m_Widget;
    QLabel *ViewInfo;
    QPushButton *copyButton;

};



#endif // FINISHWIZARD_H
