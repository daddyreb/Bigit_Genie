#ifndef QSpyWindow_H
#define QSpyWindow_H

#include <QWidget>

namespace Ui {
    class QSpyWindow;
}

class QSpyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit QSpyWindow(QWidget *parent = 0);
    ~QSpyWindow();

private slots:
    void on_rdt_edt_stack_clicked();
    void on_rdt_edt_stylesheet_clicked();
    void on_rdt_edt_text_clicked();
    void on_but_install_clicked(bool checked);

    void on_but_qss_clicked();

private:
    Ui::QSpyWindow *ui;
};

#endif // QSpyWindow_H
