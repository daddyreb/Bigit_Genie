#ifndef QGENIEDISCLAIMERWIDGET_H
#define QGENIEDISCLAIMERWIDGET_H

#include "QGenieDialogInnerWidget.h"
#include <QTextEdit>
#include <QMovie>
#include <QPushButton>
#include <QEvent>
#include <QResizeEvent>

class QGenieDisclaimerWidget : public QGenieDialogInnerWidget
{
        Q_OBJECT
public:
    QGenieDisclaimerWidget(QWidget *parent=0);
        ~QGenieDisclaimerWidget();
protected:
        QPushButton		*ok_btn;
        QTextEdit		*prompt_label_te;
protected:
        void retranslateUi();
protected:
        virtual void changeEvent(QEvent *event);
        virtual void resizeEvent(QResizeEvent *event);
protected slots:
        void ok_btn_clicked(bool checked);
public:
         virtual QString get_dlg_title();
};

#endif // QGENIEDISCLAIMERWIDGET_H
