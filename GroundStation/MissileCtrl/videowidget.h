#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>

namespace Ui {
class VideoWidget;
}

class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    Ui::VideoWidget *ui;

    void paint(QByteArray *byteBuf);

    void fresh();

    void slotTimeOut();

private:
    int fps = 0;



    bool newFrm = false;
};

#endif // VIDEOWIDGET_H
