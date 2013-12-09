#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QAxObject>
#include <QStringList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString,int> m_KeyIndexMap;
    QMap<QString,QString> m_KeyStringMap;
    QList<QPair<QString,QStringList> > m_FileNameKeyMap;
    QList<QPair<QString,QString> > m_NoTranslate;
    QMap<QString,int> m_CountryColMap;
    QMap<QString,QString> m_CommantFileMap;
    QList<QString> m_FileNameList;
    QList<QString> m_CountryList;
    QList<QString> m_MissInExcel;
    QList<QStringList> m_RootExcel;

    QAxObject *m_Excel;
    QAxObject *m_Workbooks;
    QAxObject *m_Workbook;
    QAxObject *m_Worksheet;

private:
    void Init();
    void Start();
    void ReadExcel();
    void GetKeyIndexMap();
    void BeginTranslate();
    bool Compile(QString en,QString tr);
    void Writei18n(QString, QString, QList<QPair<QString,QString> > &);
    void FullValue(QString,QString,QList<QPair<QString,QString> > &);
};

#endif // MAINWINDOW_H
