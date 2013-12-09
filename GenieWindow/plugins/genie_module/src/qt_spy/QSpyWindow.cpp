#include "QSpyWindow.h"
#include "ui_QSpyWindow.h"
#include <QtCore>

QSpyWindow::QSpyWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QSpyWindow)
{
    ui->setupUi(this);
    ui->spy_w->setEditType(0);
}

QSpyWindow::~QSpyWindow()
{
    delete ui;
}

void QSpyWindow::on_but_install_clicked(bool checked)
{
    QWidgetList wlist=qApp->allWidgets();
    if(checked)
    {
        //install event filter
        foreach(QWidget *w,wlist)
        {
            if(w!=this)
            {
                QWidget *_testW=w;
                bool bIgnore=false;
                while(_testW)
                {
                    if(_testW==this)
                    {
                        bIgnore=true;
                        break;
                    }
                    _testW=_testW->parentWidget();
                }
                if(!bIgnore)
                {
                    w->installEventFilter(ui->spy_w);
                }
            }
        }

        qDebug()<<wlist.count()-1<<"widgets installed spy";
    }
    else
    {
        //uninstall event filter
        foreach(QWidget *w,wlist)
        {
            if(w!=this)
            {
                w->removeEventFilter(ui->spy_w);
            }
        }
    }
    ui->spy_w->whenInstall(checked);

    foreach(QWidget *w,wlist)
    {
        if(w!=this)
        {
            w->update();
        }
    }
}

void QSpyWindow::on_but_qss_clicked()
{
    QString s=ui->edt_path->text();
    if(QFile::exists(s))
    {
        QFile file(s);
        file.open(QFile::ReadOnly);
        this->setStyleSheet(QLatin1String(file.readAll()));
    }
    else
    {
        this->setStyleSheet("");
    }


}

void QSpyWindow::on_rdt_edt_text_clicked()
{
    ui->spy_w->setEditType(0);
}

void QSpyWindow::on_rdt_edt_stylesheet_clicked()
{
    ui->spy_w->setEditType(1);
}

void QSpyWindow::on_rdt_edt_stack_clicked()
{
    ui->spy_w->setEditType(2);
}
