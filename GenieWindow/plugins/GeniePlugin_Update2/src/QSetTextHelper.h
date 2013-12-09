#ifndef Q_SETTEXT_HEPER__
#define Q_SETTEXT_HEPER__
#include <QtGui>
#include <QtWidgets>
class QSetTextHelper
{
public:
    QSetTextHelper(QWidget *w);
    bool isOk();
    void setText(const QString &text);
    QString text();

protected:
    QLabel *mLabel;
    QPushButton *mButton;
    QRadioButton *mRdtButton;
    QCheckBox *mChkBox;
    QTabBar *mTabBar;
};
#endif
