#ifndef INFOMESSAGEBOX_H
#define INFOMESSAGEBOX_H

#include <QtGui>
#include <QEvent>

#include "IUiManager.h"
#include "ILanguageManager.h"
#include "IManagerHandler.h"

//#include "inc/genie_interface/ILanguageManager.h"
//#include "inc/genie_interface/IPluginInf.h"

class InfoMessageBox : public QDialog
{
    Q_OBJECT
public:
    InfoMessageBox(QString message, QString title);
    ~InfoMessageBox();

    static IManagerHandler *sHandler;

private:
    void retranslateUi();

private:
    QLabel *titleLabel;
    QGridLayout *gridLayout;
    QFrame *mFrame;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;

    QPoint last,pos0;
    QUuid uuid;

    IPluginInf* m_Router_plugin;

private:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void init_dlgtitle();

    QFrame *m_framecontainer;
    QLabel *m_dlgtitle_lb;

    int                             m_dlg_width;
    int                             m_dlg_height;
    int                             m_dlgtitle_lb_height;
    int                             m_dlgtitle_lb_spaceitemw;
    int                             m_frame_lr_margin;
    int                             m_frame_tb_margin;

    QString                     m_framecontainer_style;



public:
    ILanguageManager *lanmanager;
    void labelchange(QString,QString);

private slots:
    void ChangePage();

protected:
    void changeEvent(QEvent* event);
};

#endif // INFOMESSAGEBOX_H
