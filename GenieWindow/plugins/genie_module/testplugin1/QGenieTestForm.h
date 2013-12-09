#ifndef QGENIETESTFORM_H
#define QGENIETESTFORM_H

#include <QWidget>

namespace Ui {
    class QGenieTestForm;
}

class QGenieTestForm : public QWidget
{
    Q_OBJECT

public:
    explicit QGenieTestForm(QWidget *parent = 0);
    ~QGenieTestForm();

private:
    Ui::QGenieTestForm *ui;

private slots:
    void on_cob_model_currentIndexChanged(QString );
    void on_but_test_clicked();
    void on_but_currentsetting_clicked();
    void on_but_routeranim_clicked();
    void on_but_log_clicked();
    void on_but_quit_clicked();
    void on_notify_clicked();
    void on_subscrib_clicked();
    void on_setsetting_clicked();
    void on_getsetting_clicked();
    void on_uicmd_clicked();
};

#endif // QGENIETESTFORM_H
