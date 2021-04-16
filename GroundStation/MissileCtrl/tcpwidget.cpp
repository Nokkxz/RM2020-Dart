#include "tcpwidget.h"
#include "ui_tcpwidget.h"
#include <QHostAddress>
#include <QDebug>

TCPWidget::TCPWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TCPWidget)
{
    ui->setupUi(this);

    videoWidget.show();

    secTimer = new QTimer(this);
    secTimer->start(1000);

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, &QTcpSocket::stateChanged, this, &TCPWidget::slotStateChange);
    connect(secTimer, &QTimer::timeout, this, &TCPWidget::slotTimeOut);
    connect(secTimer, &QTimer::timeout, &(this->infoWidget), &InfoWidget::slotTimeOut);
    connect(secTimer, &QTimer::timeout, &(this->videoWidget), &VideoWidget::slotTimeOut);
}

void TCPWidget::slotStateChange()
{
    switch (tcpSocket->state())
    {
    case QAbstractSocket::UnconnectedState:
        ui->labelState->setText("Unconnected");
        ui->pushButtonConnect->setText("Connect");
        break;
    case QAbstractSocket::HostLookupState:
        ui->labelState->setText("HostLookup");
        break;
    case QAbstractSocket::ConnectingState:
        ui->labelState->setText("Connecting");
        ui->pushButtonConnect->setText("Disconnect");
        break;
    case QAbstractSocket::ConnectedState:
        ui->labelState->setText("Connected");
        ui->pushButtonConnect->setText("Disconnect");
        connect(tcpSocket, &QTcpSocket::readyRead, this, &TCPWidget::slotReadyRead);
        break;
    case QAbstractSocket::ClosingState:
        ui->labelState->setText("Closing");
        break;
    default:
        break;
    }
}

void TCPWidget::slotReadyRead()
{
    QByteArray byteBuf = tcpSocket->readAll();

    if(!byteBuf.isNull())
    {
        if(ui->comboBoxShow->currentText() == "ShowPkgInfo")
        {
            ui->textEdit->append(QString("%1").arg(byteBuf.length()));
        }
        else if (ui->comboBoxShow->currentText() == "ShowPkgContent")
        {
            ui->textEdit->append(byteBuf);
        }

        this->pkgNumSec++;
        this->pkgNum++;
        this->dataSize += byteBuf.length();

        if(byteBuf.size() == 10){
            infoWidget.update(&byteBuf);
        }
        else if(byteBuf.size() == 2){
            videoWidget.fresh();
        }
        else if(byteBuf.size() == 1000){
            videoWidget.paint(&byteBuf);
        }
    }
}

void TCPWidget::slotTimeOut()
{
    ui->labelPkgSpeed->setText(QString("PkgSpeed: %1 pkg/s").arg(pkgNumSec));
    ui->labelDataSpeed->setText(QString("DataSpeed: %1 b/s").arg(dataSize));
    ui->labelPkgNum->setText(QString("PkgNum: %1 ").arg(pkgNum));
    pkgNumSec = 0;
    dataSize = 0;
}

TCPWidget::~TCPWidget()
{
    tcpSocket->close();
    delete ui;
}

void TCPWidget::on_pushButtonConnect_clicked()
{
    switch (tcpSocket->state())
    {
    case QAbstractSocket::UnconnectedState:
        this->ip = ui->lineEditIP->text();
        this->port = quint16(ui->lineEditPort->text().toInt());
        tcpSocket->connectToHost(QHostAddress(ip),port);
        break;
    case QAbstractSocket::ConnectingState:
        tcpSocket->abort();
        break;
    case QAbstractSocket::ConnectedState:
        tcpSocket->disconnectFromHost();
        break;
    default:
        break;
    }
}

void TCPWidget::on_pushButtonCLRPkg_clicked()
{
    pkgNum = 0;
}

void TCPWidget::on_pushButtonCLRText_clicked()
{
    ui->textEdit->clear();
}
