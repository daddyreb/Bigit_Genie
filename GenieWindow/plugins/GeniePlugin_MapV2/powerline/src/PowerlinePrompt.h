#ifndef POWERLINEPROMPT_H
#define POWERLINEPROMPT_H

#include <QDialog>
#include <QEvent>

namespace Ui {
    class PowerlinePrompt;
}

class PowerlinePrompt : public QDialog
{
    Q_OBJECT

public:
    PowerlinePrompt(QWidget *parent = 0);
    ~PowerlinePrompt();

private:
    Ui::PowerlinePrompt *ui;
private:
    void init();
    void clearData();
	void initLanguage();

public:
    void setTitle(QString strTitle);
    void setPic(QString strPath);
    void setContext(QString strTip);
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    bool    m_bMove;
    QPoint  m_position;
private slots:
    void btn_ok();
    void btn_cancel();
signals:
    void btnOK();

};

#endif // POWERLINEPROMPT_H
