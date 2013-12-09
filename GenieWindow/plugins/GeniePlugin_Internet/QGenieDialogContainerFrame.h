#ifndef QGENIEDIALOGCONTAINERFRAME_H
#define QGENIEDIALOGCONTAINERFRAME_H

#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
//#ifdef Q_OS_MACX
//#include <QEventLoop>
//#endif
#include "QGenieDialogInnerWidget.h"


class QGenieNetworkDiagnose;
class QGenieVisibleNetworkWidget;

class QGenieDialogContainerFrame : public QDialog
{
Q_OBJECT
public:
    enum Dlg_Size_Model{Large,Medium,Normal,Small};
public:
    explicit QGenieDialogContainerFrame(Dlg_Size_Model size_model,QGenieDialogInnerWidget* inner,QWidget *parent = 0);
protected:
    QFrame                                                  *m_framecontainer;
    QLabel                                                  *m_dlgtitle_lb;
    QGenieDialogInnerWidget                                 *m_inner_widget;
//#ifdef Q_OS_MACX
//    QEventLoop                                              *event_loop;
//#endif
protected:
    QPoint pos0;
    QPoint last;
    bool   m_bDrag;
protected:
//    int                             m_dlg_width;
//    int                             m_dlg_height;
//    int                             m_dlgtitle_lb_height;
//    int                             m_dlgtitle_lb_spaceitemw;
//    int                             m_frame_lr_margin;
//    int                             m_frame_tb_margin;

//    QString                         m_framecontainer_style;
protected:
//    void init_dim(Dlg_Size_Model size_model);
//    void init_dlgtitle(Dlg_Size_Model size_model);
    void retranslateUi();
protected:
    void changeEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
public:
    static int messageBox(QWidget *parent,const QString &title,const QString &text,bool byesorno);
    static void passwordInvalidBox(QWidget *parent,const QString &title,const QString &text,const QString &imagepath);
private:
    friend class QGenieNetworkDiagnose;
    friend class QGenieVisibleNetworkWidget;
    static QList<QGenieDialogContainerFrame*>     m_dcfs_list;
signals:

public slots:
    int exec();
//#ifdef Q_OS_MACX
//    virtual void accept();
//    virtual void reject();
//    int exec();
//#endif
};

//inner widget for messagebox
class QGenieInnerMsgBox : public QGenieDialogInnerWidget
{
        Q_OBJECT
public:
    QGenieInnerMsgBox(QWidget *parent=0,bool yesorno=false,const QString &imagepath=QString());
        ~QGenieInnerMsgBox();
protected:
        QPushButton		*ok_btn;
        QPushButton		*cancel_btn;
        QLabel			*prompt_label;
        QLabel                     *image_lb;
protected:
        QString                     m_strtitle;
        QString                     m_strtext;
        bool                          m_byesorno;
protected:
        void retranslateUi();
protected:
        virtual void changeEvent(QEvent *event);
protected slots:
        void ok_btn_clicked(bool checked);
        void cancel_btn_clicked(bool checked);
public:
         virtual QString get_dlg_title();
public:
         void set_title(const QString &title);
         void set_text(const QString &text);
};

#endif // QGENIEDIALOGCONTAINERFRAME_H
