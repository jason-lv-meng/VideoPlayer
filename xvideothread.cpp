#include "xvideothread.h"
#include "xffmpeg.h"
#include "xaudioplay.h"
#include <list>

using namespace std;

static list<AVPacket> videos;
static int apts = -1;
XVideoThread::XVideoThread(QObject *parent) : QThread(parent)
{

}

void XVideoThread::run()
{
    char out[10000] = {0};

    while (!m_stop)
    {

        if (!XFFmpeg::Get()->m_isPlay)
        {
            msleep(10);
            continue;
        }

        while (videos.size() > 0) {
            AVPacket pack = videos.front();
            int pts = XFFmpeg::Get()->GetPts(&pack);

            if (pts > apts)
            {
               break;
            }

             XFFmpeg::Get()->Decode(&pack);
             av_packet_unref(&pack);
             videos.pop_front();
        }

        int free = XAudioPlay::Get()->GetFree();
        if (free < 4096)
        {
            msleep(1);
            continue;
        }

        AVPacket pkt = XFFmpeg::Get()->Read();

        if(pkt.size <= 0)
        {
            msleep(10);
            continue;
        }

        if (pkt.stream_index == XFFmpeg::Get()->m_audioStream)
        {
            apts = XFFmpeg::Get()->Decode(&pkt);
            av_packet_unref(&pkt);
            int len =  XFFmpeg::Get()->ToPCM(out);
            XAudioPlay::Get()->Write(out,len);

            continue;
        }

        if (pkt.stream_index  != XFFmpeg::Get()->m_videoStream)
        {
            av_packet_unref(&pkt);
            continue;
        }

        videos.push_back(pkt);
//        XFFmpeg::Get()->Decode(&pkt);
//        av_packet_unref(&pkt);
//        if(XFFmpeg::Get()->m_fps > 0)
//            msleep(1000 / XFFmpeg::Get()->m_fps );
    }
}
