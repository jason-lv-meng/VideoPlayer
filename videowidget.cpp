#include "videowidget.h"
#include "xffmpeg.h"
#include "xvideothread.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent ) :QOpenGLWidget (parent)
{

    startTimer(20);
    XVideoThread::Get()->start();
}

VideoWidget::~VideoWidget()
{
    XVideoThread::Get()->m_stop =  true;
    XVideoThread::Get()->wait();
}

void VideoWidget::paintEvent(QPaintEvent *e)
{
    static QImage * image = nullptr;
    static int w = 0;
    static int h = 0;

    if (w != width() || h != height())
    {
        if (image)
        {
            delete  image->bits();
            delete  image;
            image = nullptr;
        }
    }

    w = width();
    h = height();

    if (image == nullptr)
    {
        uchar *buf = new uchar[width()*height()*4];
        image = new QImage(buf,width(),height(),QImage::Format_ARGB32);
    }

    //    AVPacket pkt = XFFmpeg::Get()->Read();

    //    if (pkt.stream_index != XFFmpeg::Get()->m_videoStream)
    //    {
    //        av_packet_unref(&pkt);
    //        return;
    //    }
    //    if (pkt.size == 0) return;

    //    AVFrame * yuv = XFFmpeg::Get()->Decode(&pkt);
    //    av_packet_unref(&pkt);
    //    if (yuv == nullptr) return;
    XFFmpeg::Get()->ToRGB((char*) image->bits(),width(),height());

    QPainter painter;
    painter.begin(this);
    painter.drawImage(QPoint(0,0),*image);
    painter.end();
}

void VideoWidget::timerEvent(QTimerEvent *e)
{

    this->update();

}
