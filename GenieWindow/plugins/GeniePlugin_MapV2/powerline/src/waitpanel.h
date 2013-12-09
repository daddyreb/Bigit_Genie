#ifndef WAITTINGPANEL_H
#define WAITTINGPANEL_H

#include <QWidget>
#include <QString>

class QEvent;
class QLabel;
class QProgressIndicator;

class WaitPanel : public QWidget
{
    Q_OBJECT
public:
    explicit WaitPanel(QWidget *parent = 0);
    ~WaitPanel();

private:
    QProgressIndicator  *m_indicator;
    QLabel              *m_promptLabel;

protected:
    virtual void changeEvent(QEvent *);

private:
    void retranslateUi();
    QString translateText(int idx, const QString &defaultText = QString());

signals:
    void translateText(int idx ,QString *text);

public slots:
    void start();
    void stop();
};

#endif // WAITTINGPANEL_H
