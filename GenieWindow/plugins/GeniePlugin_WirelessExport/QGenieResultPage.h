#ifndef QGENIERESULTPAGE_H
#define QGENIERESULTPAGE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEvent>


class QGenieResultPage : public QFrame
{
Q_OBJECT
public:
    explicit QGenieResultPage(QWidget *parent = 0);
protected:
    QLabel                      *result_image_lb;
    QLabel                      *result_text_lb;
    QPushButton          *end_btn;
    bool                            m_bsuccess;
protected:
    virtual void changeEvent(QEvent *event);
    void retranslateUi();
public:
    void init(bool bsuccess);
signals:
    void resultpage_end_btn_clicked();
public slots:

};

#endif // QGENIERESULTPAGE_H
