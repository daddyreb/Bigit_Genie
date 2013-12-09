#ifndef QGENIEWELCOMPAGE_H
#define QGENIEWELCOMPAGE_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QEvent>

class QGenieWelcomePage : public QFrame
{
Q_OBJECT
public:
    explicit QGenieWelcomePage(QWidget *parent = 0);

protected:
    QLabel          *welcome_image_lb;
    QLabel          *welcome_text_lb;
    QLabel          *introduction_lb;
    QLabel          *opt_prompt_lb;
    QPushButton  *opt_next_btn;
    QPushButton  *opt_cancel_btn;
protected:
    virtual  void changeEvent(QEvent *event);
    void  retranslateUi();
signals:
    void welcomepage_next_btn_clicked();
    void welcomepage_cancel_btn_clicked();
public slots:
};

#endif // QGENIEWELCOMPAGE_H
