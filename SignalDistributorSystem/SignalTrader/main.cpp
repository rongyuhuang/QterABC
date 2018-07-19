#include "traderwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TraderWindow w;
    w.show();

    return a.exec();
}
