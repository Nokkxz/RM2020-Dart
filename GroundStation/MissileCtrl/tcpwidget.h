#ifndef TCPWIDGET_H
#define TCPWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include "videowidget.h"
#include "infowidget.h"

namespace Ui {
class TCPWidget;
}

class TCPWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TCPWidget(QWidget *parent = nullptr);
    ~TCPWidget();

    VideoWidget videoWidget;
    InfoWidget infoWidget;
    int dataSize = 0;

private slots:
    void on_pushButtonConnect_clicked();

    void on_pushButtonCLRPkg_clicked();

    void on_pushButtonCLRText_clicked();

private:
    Ui::TCPWidget *ui;

    QTcpSocket* tcpSocket = nullptr;
    QTimer* secTimer;

    QString ip;
    quint16 port;

    int pkgNum = 0;
    int pkgNumSec = 0;

    void slotReadyRead();
    void slotStateChange();
    void slotTimeOut();

};
#endif // TCPWIDGET_H
