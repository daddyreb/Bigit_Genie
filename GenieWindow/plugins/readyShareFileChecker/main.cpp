#include <QtCore/QCoreApplication>
#include <QtCore>
//0 file exist
//1 file not exist
int main(int argc, char *argv[])
{
    if(argc <2)
    {
        return -1;
    }
    QDir dir(argv[1]);
    return dir.exists()?0:1;
//    QCoreApplication a(argc, argv);

//    return a.exec();
}
