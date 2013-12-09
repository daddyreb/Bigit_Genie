#ifndef QGENIECHOOSELOCATIONWIDGET_H
#define QGENIECHOOSELOCATIONWIDGET_H

#include "QGenieDialogInnerWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QResizeEvent>
#include <QComboBox>
//#include "plugin_ui_def.h"


class QGenieChooseLocationWidget : public QGenieDialogInnerWidget
{
    Q_OBJECT
public:
    QGenieChooseLocationWidget(QWidget *parent=0);
    ~QGenieChooseLocationWidget();
protected:
    QComboBox			*locations_list;
    QPushButton                 *ok_btn;
    QPushButton                 *cancel_btn;
    QLabel			*prompt_label;
protected:
    void retranslateUi();
protected:
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
protected slots:
    void ok_btn_clicked(bool checked);
    void cancel_btn_clicked(bool checked);
public:
    virtual QString get_dlg_title();
public:
    void init_location_list(const QStringList &sets);
    QString get_location();
};

#endif // QGENIECHOOSELOCATIONWIDGET_H
