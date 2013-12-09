#ifndef UPDATEWIZARD_H
#define UPDATEWIZARD_H

#include <QWidget>
#include "wizardpage.h"

class QPushButton;
class QRadioButton;
class QButtonGroup;

class UpdateWizard : public WizardPage
{
    Q_OBJECT
public:
    UpdateWizard(QWidget *parent=0);
    ~UpdateWizard();

    int isTypeCheck();
    void SetInfo(const QString &Info);

    static IManagerHandler *sHandler;

protected:
    void changeEvent(QEvent *e);

private:
    QWidget *m_Widget;
    QLabel *viewTitle;
    QLabel *viewPath;
    QButtonGroup *TypeGroup;
    QRadioButton *SoapType;
    QRadioButton *WebType;


    QPushButton *mNextButton;

};




#endif // UPDATEWIZARD_H
