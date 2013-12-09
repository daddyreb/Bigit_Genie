#ifndef QGENIEBUTTONSTACKVIEWCONTROLLER_H
#define QGENIEBUTTONSTACKVIEWCONTROLLER_H

class QGeniePagePluginProxy;
#ifndef IN_STABLE_H
#include <QObject>
#include <QPushButton>
#endif
#define  LINK_PAGE_HINT "linkpage"

enum CONTROLER_TYPE
{
    CT_LEFT_BUTTON,
    CT_BIG_BUTTON
};

class QGenieButtonStackViewController:public QPushButton
{
    Q_OBJECT
public:
    QGenieButtonStackViewController(QWidget *parent=NULL);
    void setLink(QWidget *w,QGenieButtonStackViewController *leftbutton=NULL);
    void checkThis();
//    void setNotifyPara(QString s);
    void parsePara(QString s);
    QString mNotifyPara;
    int mLeftPanIdx;
    void changeLan();
protected:
   // QGeniePagePluginProxy *m
    QWidget *mLinkWidget;
    QGenieButtonStackViewController *mLeftButton;
    virtual void click();
    CONTROLER_TYPE mControllerType;

    int mStrIdx;
    QString mStr;

    void changeEvent(QEvent* event);

protected slots:
    void slot_Click ();
};

#endif // QGENIEBUTTONSTACKVIEWCONTROLLER_H
