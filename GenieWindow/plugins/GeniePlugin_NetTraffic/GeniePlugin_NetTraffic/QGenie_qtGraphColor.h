#ifndef QTGRAPHCOLOR_H
#define QTGRAPHCOLOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtGui/QIcon>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtGui/QPixmap>

class QGenie_qtNetTraffic;

class QGenie_qtGraphColor : public QWidget
{
Q_OBJECT
public:
    explicit QGenie_qtGraphColor(QWidget *parent = 0);
    QFrame *frame;
    QLabel *lblColors;
    QGroupBox *groupBox;
    QLabel *lblReceivedData;
    QComboBox *cmbRec;
    QLabel *lblSentData;
    QComboBox *cmbSent;
    QLabel *lblTotalData;
    QComboBox *cmbTotal;
    QLabel *lblGraphBack;
    QComboBox *cmbGraphBack;
    QLabel *lblWinBack;
    QComboBox *cmbWinBack;
    QPushButton *btnReset;
    QGroupBox *gbxWinPro;
    QLabel *lblTran;
    QComboBox *cmbTran;
    QPushButton *btnColorOk;
    QPushButton *btnColorCancel;

    void initColorCombox(QComboBox* qcom);
    void retranslateUi(QWidget *Widget);
    void setupUi(QWidget *Widget);

    QColor qclrReceive;
    QColor qclrSent;
    QColor qclrTotal;
    QColor qclrGraphBg;
    QColor qclrWindowBg;
    double dbOpt;
    QGenie_qtNetTraffic* qtNet;
    QColor getColorFromComboBox(QComboBox* qcom);
    double getDblFromComboBox(QComboBox* qcom);
    void closeEvent ( QCloseEvent * event );
    void changeEvent(QEvent *e);

signals:

public slots:
    void okClicked(bool checked = false );
    void cancelClicked(bool checked = false);
    void setToDefaults(bool checked = false);

};

#endif // QTGRAPHCOLOR_H
