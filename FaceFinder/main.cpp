#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QCoreApplication::setOrganizationName("DinoSoft");
    QCoreApplication::setApplicationName("FaceFinder");
    
    return a.exec();
}
