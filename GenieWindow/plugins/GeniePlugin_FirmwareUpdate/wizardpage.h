#ifndef WIZARDPAGE_H
#define WIZARDPAGE_H

#include <QWidget>

#include "IManagerHandler.h"
#include "route_animation/QGenieTopoNodePaintStrategy.h"

class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QLabel;

class WizardPage : public QWidget
{
    Q_OBJECT
public:
    //WizardPage();
    WizardPage(QWidget *parent=0);
    ~WizardPage();

    //static void changePic(QString mName);

    static IManagerHandler *sHandler;

public:
    void setTitle(QString mTitle);
    void setExplain(QString mExplain);
    void setImage(QPixmap mImage);
    void setButtonEnabled(int index);
    void setImageView(QString strA, QString strB);
    void stopTimer();
    QWidget* isSetArea();

    void ReloadFlash();

    QLabel *l_Image;

protected slots:
    void TurnNext();
    void TurnBack();
    void TurnCancel();
    void ChangeImage();

signals:
    void ToNext();
    void ToBack();
    void ToCancel();

protected:
    QPushButton *BackButton;
    QPushButton *NextButton;
    QPushButton *CancelButton;
    QLabel *SpaceButt;
    //QLabel *l_Image;

    QLabel *m_Title;
    QLabel *m_Intro;

    //void changeEvent(QEvent *e);
    //void setImageView(QString strA, QString strB);
    //void stopTimer();

private:
    QWidget *LeftWidget;
    QVBoxLayout *l_WidgetLayout;
    QGenieRouteWidget_B3 *FlashWidget;
    //QLabel *l_Image;

    bool mFlag;
    QString strPicA;
    QString strPicB;

    int intTime;
    QTimer *changeTimer;

    QWidget *RightWidget;
    QVBoxLayout *r_WidgetLayout;
    QWidget *m_setWidget;


    QWidget *BelowWidget;
    QHBoxLayout *ButtonLayout;


    QHBoxLayout *TopLayout;
    QVBoxLayout *WholeLayout;

};

#endif // WIZARDPAGE_H
