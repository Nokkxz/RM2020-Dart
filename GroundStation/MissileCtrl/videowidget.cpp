#include "videowidget.h"
#include "ui_videowidget.h"

VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWidget)
{
    ui->setupUi(this);
}

VideoWidget::~VideoWidget()
{
    delete ui;
}

void VideoWidget::slotTimeOut()
{
    ui->lcdNumber->display(fps);
    fps = 15;
}

void VideoWidget::fresh()
{

}

void VideoWidget::paint(QByteArray *byteBuf)
{

}
