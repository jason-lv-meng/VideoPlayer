#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
    VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    void paintEvent(QPaintEvent *e);

    void timerEvent(QTimerEvent *e);

};

#endif // VIDEOWIDGET_H
