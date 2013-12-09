#ifndef QLAUNCHUPDATEDIALOG_H
#define QLAUNCHUPDATEDIALOG_H

#include <QDialog>
#include <QHttp>
#include <QFile>
#include <QStringList>
#include <QList>
#include <QTimer>
#include "QLaunchManager.h"

class QUpdateProcess;
namespace Ui {
    class QLaunchUpdateDialog;
}

enum DIALOG_CACHED_TEXT
{
    DCT_LEFT_BUTTON=0,
    DCT_RIGHT_BUTTON,
    DCT_FINISHPAGE,
    DCT_NUM
};

enum DIALOG_STATE
{
    DS_INIT,
    DS_GETDISP_DB,
    DS_SHOW_RELEASENOTE,
    DS_GETFILE,
    DS_FINISH
};


class QLaunchUpdateDialog : public QDialog {
    Q_OBJECT
public:
    QLaunchUpdateDialog(QWidget *parent = 0);
    ~QLaunchUpdateDialog();
    void startup(UPDATE_RESON reson);
    bool mMustRestart;
    void changeText();
    void changeUiState(DIALOG_STATE state);
protected:

    QUpdateProcess *mUpdateProcess;

    void changeEvent(QEvent *e);

    int mButtonText[DCT_NUM];




    virtual void	closeEvent ( QCloseEvent * event );
    QString getText(int idx);

public:
    Ui::QLaunchUpdateDialog *ui;

//    bool allMd5Match();


//    int mCmdGetDiscription;

protected:
#ifndef SHEET_STYLE_FORMAC
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool mDrag;
    QPoint mDragPosition;
#endif
 //   QTimer mReloadTimer;
protected slots:
    void on_but_left_clicked();
    void on_but_right_clicked();
    friend class QUpdateProcess;

};

#endif // QLAUNCHUPDATEDIALOG_H
