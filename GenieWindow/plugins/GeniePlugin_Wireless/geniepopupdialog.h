#ifndef GENIEPOPUPDIALOG_H
#define GENIEPOPUPDIALOG_H

#include "scalabledialogframe.h"

class GeniePopupDialog : public ScalableDialogContentWidget
{
    Q_OBJECT
public:
    explicit GeniePopupDialog(QWidget *parent = 0);

public:
    void setWindowTitle(const QString &title);
    void show();

    bool isSizeGripEnabled() const;
    int	result() const;
    void setModal(bool modal);
    void setResult (int i);
    void setSizeGripEnabled (bool enable);

signals:
    void accepted();
    void finished(int result);
    void rejected();

public slots:
    virtual void accept ();
    virtual void reject();
    virtual void done(int r);
    int	exec();
    void open();


};


#endif // GENIEPOPUPDIALOG_H
