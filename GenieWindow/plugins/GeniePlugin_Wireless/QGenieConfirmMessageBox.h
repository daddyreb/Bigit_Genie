#ifndef QGENIECONFIRMMESSAGEBOX_H
#define QGENIECONFIRMMESSAGEBOX_H

#include "geniepopupdialog.h"
#include <QPushButton>
#include <QLabel>
#include <QEvent>

class QGenieConfirmMessageBox : public GeniePopupDialog
{
    Q_OBJECT
public:
    explicit QGenieConfirmMessageBox(QWidget *parent = 0);
private:
    QPushButton     *m_ok_btn;
    QPushButton     *m_no_btn;
    QLabel          *m_prompt_lb;

protected:
    void retranslateUi();
protected:
    virtual void changeEvent(QEvent *event);
signals:

public slots:

};

#endif // QGENIECONFIRMMESSAGEBOX_H
