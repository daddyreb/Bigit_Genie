#ifndef PCPFORM_H
#define PCPFORM_H

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QFrame>

#include <QResizeEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QPushButton>
#include "IManagerHandler.h"
#include "ILogManager.h"
//#include "labelbtn.h"

#define INSTALL_FILE_NAME "NETGEARManagementUtility-2.1.3-install.exe"

class PCPForm : public QWidget
{
    Q_OBJECT
public:
    PCPForm(QWidget *parent);
    static IManagerHandler *sHandler;
    void changeEvent(QEvent *e);
    void focusInEvent ( QFocusEvent * event );

private:
    void setupUi();
    void retranslateUi();
    void checkPCtrlInstall();
    void execInstall();
    void Genie_Log(QString s);
    void showEvent ( QShowEvent * event );

     bool bLog;

     QFrame *frameAll;
    QWidget workWidget;
    QWidget *upWidget ;
    QWidget *bottomWidget;
    QLabel *l_Title;
    QLabel *l_Live;
    QLabel *l_One;
    QLabel *l_Two;
    QLabel *l_Three;

    //LabelBtn *l_Info;
    //LabelBtn *l_Btn;
    QPushButton *l_Info;
    bool bInstall;
    QString filePath;

    QString filePath32;
    QString filePath64;

    bool pathFlag;
    bool bEnabled;

private slots:
    void BtnClicked( bool checked = false );

protected:
    void resizeEvent(QResizeEvent *);
};

#endif // PCPFORM_H
