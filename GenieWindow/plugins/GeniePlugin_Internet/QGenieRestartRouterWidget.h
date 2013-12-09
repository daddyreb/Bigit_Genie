#ifndef QGENIERESTARTROUTERWIDGET_H
#define QGENIERESTARTROUTERWIDGET_H

#include "QGenieDialogInnerWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QResizeEvent>
#include "QGenieProgressPanel.h"

class QGenieRestartRouterWidget : public QGenieDialogInnerWidget
{
    Q_OBJECT
public:
    QGenieRestartRouterWidget(QWidget *parent=0);
    ~QGenieRestartRouterWidget();
protected:
    QPushButton                 *ok_btn;
    QPushButton                 *cancel_btn;
    QLabel			*prompt_label;
    QGenieProgressPanel         *m_progress_panel;
    int                         prompt_text_idx;
protected:
    void retranslateUi();
protected:
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
signals:
    void show_restartrouter_dlg_result(int result);
protected slots:
    void ok_btn_clicked(bool checked);
    void cancel_btn_clicked(bool checked);
    void end_showdlg();
public:
    virtual QString get_dlg_title();
};

#endif // QGENIERESTARTROUTERWIDGET_H
