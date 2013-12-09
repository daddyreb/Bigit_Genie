#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAxObject>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QPair>
#include <QDir>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Init();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReadExcel()
{
    QAxObject *excel = NULL;
    QAxObject *workbooks = NULL;
    QAxObject *workbook = NULL;

    excel = new QAxObject("Excel.Application");
    if (!excel)
    {
        QMessageBox::critical(this, "错误信息", "EXCEL对象丢失");
        return;
    }
    excel->dynamicCall("SetVisible(bool)", false);
    workbooks = excel->querySubObject("WorkBooks");
    workbook = workbooks->querySubObject("Open(QString, QVariant)", QString(ui->lineEdit->text()));
    QAxObject * worksheet = workbook->querySubObject("WorkSheets(int)", 1);//打开第一个sheet

    //QAxObject * worksheet = workbook->querySubObject("WorkSheets");//获取sheets的集合指针
    //int intCount = worksheet->property("Count").toInt();//获取sheets的数量


    QAxObject * usedrange = worksheet->querySubObject("UsedRange");//获取该sheet的使用范围对象
    QAxObject * rows = usedrange->querySubObject("Rows");
    QAxObject * columns = usedrange->querySubObject("Columns");
    //获取行数和列数
    int intRowStart = 1;//usedrange->property("Row").toInt();
    int intColStart = 1;//usedrange->property("Column").toInt();
    int intCols = columns->property("Count").toInt();
    int intRows = ui->line_RowSetp->text().toInt();//rows->property("Count").toInt();
    qDebug() << "intRowStart:" << intRowStart;
    qDebug() << "intColStart:" << intColStart;
    qDebug() << "intCols:" << intCols;
    qDebug() << "intRows:" << intRows;

//    QFile fileOut("E:\\qt_unicode.txt");
//    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        qDebug() << "open file field!";
//        return;
//    }

//    QTextStream streamFileOut(&fileOut);
//    streamFileOut.setCodec("UTF-8");

    /*获取excel内容*/
    qDebug() << "----------start read---------------------";
    for (int i = intRowStart; i < intRowStart + intRows; i++)  //行
    {
        qDebug() << "----------------read loop" << i;
        QStringList list;
        for (int j = intColStart; j < intColStart + intCols; j++)  //列
        {
            QAxObject * cell = worksheet->querySubObject("Cells(int,int)", i, j );  //获取单元格
            list << QString::fromUtf8(cell->dynamicCall("Value2()").toByteArray()).trimmed();
            //qDebug() << i << j << QString::fromUtf8(cell->dynamicCall("Value2()").toByteArray()); //正确
//            streamFileOut << QString::fromUtf8(cell->dynamicCall("Value2()").toByteArray()) << "\r\n";
//            streamFileOut.flush();
        }
        m_RootExcel << list;
    }
    qDebug() << "----------------read end";
    //fileOut.close();
    //qDebug() << "----jjjj:" << m_RootExcel[18][5];
    workbook->dynamicCall("Close (Boolean)", false);

    delete excel;
}

void MainWindow::Init()
{
    this->m_FileNameList << "{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}.txt"
                         << "{5ddd2596-5211-4d83-980b-71f3516d0b5c}.txt"
                         << "{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}.txt"
                         << "{6c7b332f-a107-40ab-84f3-2163ac13ad0a}.txt"
                         << "{7b399884-b36d-4de8-ac04-19e7452023d8}.txt"
                         << "{142d68b8-aadf-4500-9d32-7c27947bd6bb}.txt"
                         << "{5325cdd0-d419-4e98-88d8-66d865932824}.txt"
                         << "{6653bbdd-844a-465b-bcdb-94111206b1b2}.txt"
                         << "{b3665786-212a-4ae8-bf26-03b70b22cee7}.txt"
                         << "{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}.txt"
                         << "{ced77229-c59a-4669-9c26-0bb4860d96a7}.txt"
                         << "{e722b972-b05e-4d71-8cd9-676ebd1535b9}.txt"
                         << "{f8018c9f-5431-497e-ac5d-511dd0ae7402}.txt";
    m_CountryColMap["enu"] = 1;
    m_CountryColMap["deu"] = 2;
    m_CountryColMap["sve"] = 3;
    m_CountryColMap["fra"] = 4;
    m_CountryColMap["nld"] = 5;
    m_CountryColMap["ita"] = 6;
    m_CountryColMap["esp"] = 7;
    m_CountryColMap["dan"] = 8;
    m_CountryColMap["fin"] = 9;
    m_CountryColMap["nor"] = 10;
    m_CountryColMap["ell"] = 11;
    m_CountryColMap["ptb"] = 12;
    m_CountryColMap["rus"] = 13;
    m_CountryColMap["plk"] = 14;
    m_CountryColMap["hun"] = 15;
    m_CountryColMap["csy"] = 16;
    m_CountryColMap["slv"] = 17;
    m_CountryColMap["sky"] = 18;
    m_CountryColMap["rom"] = 19;
    m_CountryColMap["chs"] = 22;
    m_CountryColMap["kor"] = 23;
    m_CountryColMap["jpn"] = 24;
    m_CountryColMap["ara"] = 25;
    m_CountryColMap["tur"] = 26;

    m_CommantFileMap["{4bb1fd3c-7a84-4740-8fb3-5960bb1b28b8}.txt"] = QString::fromUtf8("##1900##");
    m_CommantFileMap["{5ddd2596-5211-4d83-980b-71f3516d0b5c}.txt"] = QString::fromUtf8("##1800##");
    m_CommantFileMap["{6c1c9b2f-5f5e-45f8-8dad-1633ff446377}.txt"] = QString::fromUtf8("##1500##");
    m_CommantFileMap["{6c7b332f-a107-40ab-84f3-2163ac13ad0a}.txt"] = QString::fromUtf8("####1000###");
    m_CommantFileMap["{7b399884-b36d-4de8-ac04-19e7452023d8}.txt"] = QString::fromUtf8("##300##");
    m_CommantFileMap["{142d68b8-aadf-4500-9d32-7c27947bd6bb}.txt"] = QString::fromUtf8("##1750##");
    m_CommantFileMap["{5325cdd0-d419-4e98-88d8-66d865932824}.txt"] = QString::fromUtf8("##1200##");
    m_CommantFileMap["{6653bbdd-844a-465b-bcdb-94111206b1b2}.txt"] = QString::fromUtf8("####300###");
    m_CommantFileMap["{b3665786-212a-4ae8-bf26-03b70b22cee7}.txt"] = QString::fromUtf8("##1900##");
    m_CommantFileMap["{c02fb5db-6c62-42e7-bf4a-f704247ed7b8}.txt"] = QString::fromUtf8("##1950##");
    m_CommantFileMap["{ced77229-c59a-4669-9c26-0bb4860d96a7}.txt"] = QString::fromUtf8("##1300##");
    m_CommantFileMap["{e722b972-b05e-4d71-8cd9-676ebd1535b9}.txt"] = QString::fromUtf8("####1100###");
    m_CommantFileMap["{f8018c9f-5431-497e-ac5d-511dd0ae7402}.txt"] = QString::fromUtf8("####1600###");
}

void MainWindow::on_pushButton_clicked()
{
    Start();
}

void MainWindow::Start()
{
    ReadExcel();
    GetKeyIndexMap();
    BeginTranslate();
    Writei18n("","no_translate.txt",this->m_NoTranslate);
}

void MainWindow::GetKeyIndexMap()
{
    //get key in enu file
    foreach(QString name, this->m_FileNameList)
    {
        QString filename =  "E:\\GitHub\\GenieWindowRelease\\GenieWindow\\plugins\\GeniePlugin_Resource\\i18n\\enu\\" + name;
        qDebug() << "-----out put file name:" << filename;
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "open file field!";
            return;
        }
        QStringList namekeylist;
        while (!file.atEnd())
        {
           QByteArray line = file.readLine();
           QString sline = QString::fromUtf8(line);
           QString sp = QString::fromUtf8("=");
           if(sline.contains(sp))
           {

               QString k = sline.left(sline.indexOf(sp));
               QString v = sline.right(sline.length() - sline.indexOf(sp) - 1);
               qDebug() << "name :" << name << "K & V:" << k << ":" << v;
               if(!this->m_KeyIndexMap.contains(k))
               {
                   this->m_KeyIndexMap[k] = -1;
                   this->m_KeyStringMap[k] = v.trimmed();
                   namekeylist << k;
               }
               else
               {
                   qDebug() << "-------------------------exist:" << k;
               }
           }
        }
        this->m_FileNameKeyMap.append(QPair<QString,QStringList>(name,namekeylist));
        file.close();
    }
    int a = 0;
    foreach(const QString s, m_KeyIndexMap.keys())
    {
        a++;
        qDebug() << a << ":" << s << m_KeyStringMap[s];
    }
    //QAxObject * cell = m_Worksheet->querySubObject("Cells(int,int)", 4202, 2);
    //qDebug() << "------4202:" << QString::fromUtf8(cell->dynamicCall("Value2()").toByteArray());


    //get value in excel file
    int RowStart = 0;//ui->line_Row->text().toInt();
    int RowEnd = m_RootExcel.count();//ui->line_RowSetp->text().toInt();
    
    foreach(const QString k, this->m_KeyStringMap.keys())
    {
		for (int j = RowStart; j < RowEnd; j++)
		{

            if(Compile(this->m_KeyStringMap[k],this->m_RootExcel[j][1]))
            {
                this->m_KeyIndexMap[k] = j;
                qDebug() << k << j;
                break;
            }
        }
    }
}

bool MainWindow::Compile(QString en,QString tr)
{
    if(en == tr)
    {
        return true;
    }
    else if(en.replace(QString::fromUtf8(":"),"") == tr)
    {
        return true;
    }
    else if(tr.right(1) == QString::fromUtf8(";"))
    {
        if(en == tr.left(tr.length() - 1))
        {
            return true;
        }
    }
    return false;
}

void MainWindow::BeginTranslate()
{
	foreach(QString country, this->m_CountryColMap.keys())
    {
        foreach(QString file, this->m_FileNameList)
        {
            QList<QPair<QString,QString> > map;
            FullValue(country,file,map);//compile m_KeyIndexMap
            Writei18n(country,file,map);
        }
    }

}
void MainWindow::Writei18n(QString country, QString filename,QList<QPair<QString,QString> > &filecontent)
{
    QDir dir;
    QString path = "temp\\" + country;
    if(!dir.exists(path))
    {
        dir.mkpath(path);
    }
    QString fname = path + "\\" + filename;
    QFile file(fname);
    file.open(QIODevice::WriteOnly);
    file.close();

    QFile fileOut(fname);
    if (!fileOut.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "open file field!";
        return;
    }

    QTextStream streamFileOut(&fileOut);
    streamFileOut.setCodec("UTF-8");
    streamFileOut.setGenerateByteOrderMark(true);
    streamFileOut << m_CommantFileMap[filename] + QString::fromUtf8("\n");
    for(int a = 0; a < filecontent.count(); a++)
    {
        streamFileOut << filecontent[a].first + QString::fromUtf8("=") + filecontent[a].second + QString::fromUtf8("\n");
    }
    streamFileOut.flush();
}

void MainWindow::FullValue(QString country, QString filename, QList<QPair<QString,QString> > &filemap)
{
    for(int a = 0; a < this->m_FileNameKeyMap.count(); a++)
    {
        if(this->m_FileNameKeyMap[a].first == filename)
        {
            foreach(const QString s,this->m_FileNameKeyMap[a].second)
            {
                if(this->m_KeyStringMap.contains(s))
                {
					QString rootENstr = this->m_KeyStringMap[s];
                    if(m_KeyIndexMap[s] == -1)
                    {
                        filemap.append(QPair<QString,QString>(s,rootENstr));
                        m_NoTranslate.append(QPair<QString,QString>(s,rootENstr));
                    }
                    else
                    {
                        QString mus = QString::fromUtf8(":\"");
                        QString v = this->m_RootExcel[m_KeyIndexMap[s]][m_CountryColMap[country]];
                        if(v == QString::fromUtf8(""))
                            v = rootENstr;
                        else if (v.right(1) == QString::fromUtf8(";"))
                        {
                            v = v.left(v.length() - 1);
                            if(rootENstr.right(2) == mus && v.right(2) != mus)
                            {
                                v = v.left(v.lastIndexOf(QString::fromUtf8("\""))) + mus;
                            }
                        }
                        else if(rootENstr.right(2) == mus &&
                                v.right(1) != QString::fromUtf8(";") &&
                                v.right(2) != mus)
                        {
                            v = v.left(v.lastIndexOf(QString::fromUtf8("\""))) + mus;
                        }
                        filemap.append(QPair<QString,QString>(s,v));
                    }
                }
            }
        }
    }


}
