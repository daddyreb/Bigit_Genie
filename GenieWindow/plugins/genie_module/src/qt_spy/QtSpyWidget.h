#ifndef QTSPYWIDGET_H
#define QTSPYWIDGET_H

#include <QWidget>
#include <QTextEdit>

class QtSpyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QtSpyWidget(QWidget *parent = 0);
    ~QtSpyWidget();

    void setEditType(int f);
    void whenInstall(bool install);
protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *, QEvent *);
//    QWidget *mCurW;
    QList<QWidget *> mWStack;
    QMap<QWidget *,QPixmap *> mPixmapCache;
    void updatePixmapCache(QWidget *w);
    QImage mCanvasFocus;

    int mCount;
    enum _Flags
    {
        F_INPAINT=1<<0,
        F_EDIT_TEXT=(1<<1),
        F_EDIT_STYLESHEET=(1<<2),
        F_EDIT_STACK=(1<<3)

    };

    quint32 mFlags;
    QTextEdit *mEditor;

signals:

public slots:
    void slot_FocusChanged(QWidget *,QWidget *);

};

#endif // QTSPYWIDGET_H
