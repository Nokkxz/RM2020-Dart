#include "tcpwidget.h"
#include "infowidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TCPWidget w;
    w.show();
    w.videoWidget.show();
    w.infoWidget.show();
    return a.exec();
}
