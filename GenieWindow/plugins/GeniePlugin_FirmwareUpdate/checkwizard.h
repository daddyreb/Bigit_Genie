#ifndef CHECKWIZARD_H
#define CHECKWIZARD_H

#include <QWidget>
#include "wizardpage.h"

//#include "IManagerHandler.h"

class CheckWizard : public WizardPage
{
    Q_OBJECT
public:
    CheckWizard(QWidget *parent=0);
    ~CheckWizard();

    void SetInfo(const QStringList &Info);

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);


//private slots:
//    void TurnNext();
//
//signals:
//    void ToNext();

//public:
    //FirmwareCheck *m_CheckVersion;

private:
    QWidget *m_Widget;

    QLabel *modelLab;
    QLabel *modelTitleLab;
    QLabel *nowVersionLab;
    QLabel *nowTitleLab;
    QLabel *newVersionLab;
    QLabel *newTitleLab;

};

#endif // CHECKWIZARD_H
