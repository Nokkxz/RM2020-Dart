#include "infowidget.h"
#include "ui_infowidget.h"

InfoWidget::InfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoWidget)
{
    ui->setupUi(this);
}

InfoWidget::~InfoWidget()
{
    delete ui;
}

void InfoWidget::slotTimeOut()
{
    ui->Count->setText(QString("%1\t%2/s").arg(infoCount).arg(infoCountPS));
    infoCountPS = 0;
}

void InfoWidget::update(QByteArray *byteBuf)
{
    infoCount++;
    infoCountPS++;
    TargetX = byteBuf->at(0)&0xff;
    TargetY = byteBuf->at(1)&0xff;
    TargetGrey = ((byteBuf->at(2)&0xff)<<8) | (byteBuf->at(3)&0xff);
    TargetR = ((byteBuf->at(4)&0xff)<<8) | (byteBuf->at(5)&0xff);
    TargetG = ((byteBuf->at(6)&0xff)<<8) | (byteBuf->at(7)&0xff);
    TargetB = ((byteBuf->at(8)&0xff)<<8) | (byteBuf->at(9)&0xff);

    QString infoString = QString("%1\t%2\t%3\t%4\t%5\t%6\n")
            .arg(TargetX).arg(TargetY).arg(TargetGrey).arg(TargetR).arg(TargetG).arg(TargetB);

    infoQueue.enqueue(infoString);

    if(infoQueue.size()>10000000)
    infoQueue.dequeue();

    ui->TargetX->setText(QString("X: %1 ").arg(TargetX));
    ui->TargetY->setText(QString("Y: %1 ").arg(TargetY));
    ui->TargetGrey->setText(QString("Grey: %1 ").arg(TargetGrey));
    ui->TargetR->setText(QString("R: %1 ").arg(TargetR));
    ui->TargetG->setText(QString("G: %1 ").arg(TargetG));
    ui->TargetB->setText(QString("B: %1 ").arg(TargetB));
}

void InfoWidget::on_pushButton_clicked()
{
    QString savePath = QFileDialog::getOpenFileName(this, "open", "../");
    if(!savePath.isNull())
    {
        QFile saveFile(savePath);
        bool isOpen = saveFile.open(QIODevice::WriteOnly);
        bool canWrite = saveFile.isWritable();
        if(isOpen && canWrite)
        {
            saveFile.write("X\tY\tGrey\tR\tG\tB\n");
            foreach(QString info, infoQueue) {
                saveFile.write(info.toLocal8Bit());
            }
        }
    }
}

void InfoWidget::on_pushButton_2_clicked()
{
    infoQueue.clear();
    infoCount = 0;
}
