#ifndef QGENIESWITCHONWLANNICRADIOWIDGET_H
#define QGENIESWITCHONWLANNICRADIOWIDGET_H

#include "QGenieDialogInnerWidget.h"
//#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QEvent>
#include <QResizeEvent>

class QGenieSwitchonWlannicRadioWidget : public /*QDialog*/QGenieDialogInnerWidget
{
    Q_OBJECT
public:
    QGenieSwitchonWlannicRadioWidget(QWidget *parent=0);
    ~QGenieSwitchonWlannicRadioWidget();
protected:
    QMovie			*gif_movie;
    QLabel			*gif_label;
    QPushButton		*ok_btn;
    QPushButton		*cancel_btn;
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
};

#endif // QGENIESWITCHONWLANNICRADIOWIDGET_H
