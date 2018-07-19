#include "distributorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DistributorWindow w;
    w.show();

    return a.exec();
}
