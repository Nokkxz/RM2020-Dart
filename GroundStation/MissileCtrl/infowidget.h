#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QQueue>

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = nullptr);
    ~InfoWidget();

    Ui::InfoWidget *ui;

    void slotTimeOut();
    void update(QByteArray *byteBuf);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    int infoCount = 0;
    int infoCountPS = 0;

    int TargetX, TargetY, TargetGrey, TargetR, TargetG, TargetB;

    QQueue<QString> infoQueue;
};

#endif // INFOWIDGET_H
