#ifndef XFFMPEG_H
#define XFFMPEG_H
#include <iostream>
#include <string>
#include <QMutex>


// 要引用c语言函数
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class XFFmpeg
{
public:
    static XFFmpeg * Get()
    {
        static XFFmpeg ff;
        return &ff;
    }

    /*
     * 打开视频文件，如果上次已经打开会先关闭
    */
    int Open(const char * path);
    bool OpenDecode();
    bool Close();
    std::string GetError();
    // 返回值要用户清理
    AVPacket Read();

    // return pts
    int Decode(const AVPacket * pkt);
    bool ToRGB(char * out,int outWidth,int outHeight);
    int ToPCM(char *out);
    bool Seek(float pos);
    int GetPts(const AVPacket * pkt);
    virtual ~XFFmpeg();
    int m_totalMs = 0;
    int m_videoStream = 0;
    int m_fps = 0;
    int m_pts = 0;
    bool m_isPlay = false;
    int m_audioStream = 1;
    int sampleRate = 48000;
    int sampleSize = 16;
    int channel = 2;


protected:
    char m_errorbuf[1024];
    AVFormatContext *m_ic = NULL;
    AVFrame * m_yuv = NULL;
    SwsContext *m_cCtx = NULL;
    SwrContext *m_aCtx = NULL;
    AVFrame * m_pcm = NULL;

    QMutex m_mutex;
    XFFmpeg();

};

#endif // XFFMPEG_H
