#ifndef COMPLETEWIZARD_H
#define COMPLETEWIZARD_H

#include <QWidget>
#include "wizardpage.h"


class CompleteWizard : public WizardPage
{
    Q_OBJECT
public:
    CompleteWizard(QWidget *parent=0);
    ~CompleteWizard();

    void SetInfo(const QString &Info);
    void SetFinishButton();

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private slots:
    void CopyFilePath();

private:
    QWidget *m_Widget;
    QLabel *viewTitle;
    QLabel *viewPath;
    QString strPath;

    QPushButton *copyButton;

};




#endif // COMPLETEWIZARD_H
