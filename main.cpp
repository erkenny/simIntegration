

#include <QtGui/QApplication>
#include "garbageapp.h"

// #include <QProcess>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    garbageApp w;
    w.show();

    return a.exec();
}

#
