#ifndef QGENIEMODALDIALOG_H
#define QGENIEMODALDIALOG_H

#include <QDialog>

namespace Ui {
    class QGenieModalDialog;
}

class QGenieModalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QGenieModalDialog(QWidget *parent = 0);
    ~QGenieModalDialog();

    void setTitle(const QString &title);
    void setExplain(const QString &explain);
private:
    Ui::QGenieModalDialog *ui;
};

#endif // QGENIEMODALDIALOG_H
