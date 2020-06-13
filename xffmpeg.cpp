#include "xffmpeg.h"


#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")

using namespace  std;

static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0?0 : (double) r.num /(double) r.den;
}

XFFmpeg::XFFmpeg()
{
    m_errorbuf[0] = '\0';
    //  初始化封装库
    av_register_all();
}

XFFmpeg::~XFFmpeg()
{

}

int XFFmpeg::GetPts(const AVPacket * pkt)
{
    m_mutex.lock();

    if (!m_ic)
    {
        m_mutex.unlock();
        return  -1;
    }

    int pts = (pkt->pts *r2d(m_ic->streams[pkt->stream_index]->time_base))*1000;
    m_mutex.unlock();
    return pts;
}

int XFFmpeg::Open(const char * path)
{
    Close();
    m_mutex.lock();
    int ret =  avformat_open_input(
                &m_ic,
                path,
                0, // 0 表示自动选择解封装
                0 // 参数设置，比如 rtsp的延时时间
                );

    if (ret != 0)
    {
        m_mutex.unlock();
        av_strerror(ret,m_errorbuf,sizeof (m_errorbuf)-1);
        cout << "open " << path << " failed!: " << m_errorbuf << endl;

        return 0;

    }
    m_totalMs = ((m_ic->duration / AV_TIME_BASE)*1000);

    // 打开解码器
    for (int i = 0; i < m_ic->nb_streams; i ++)
    {
        AVCodecContext *enc = m_ic ->streams[i]->codec;

        if (enc->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_videoStream = i;

            m_fps = r2d(m_ic->streams[i]->avg_frame_rate);
            AVCodec * codec = avcodec_find_decoder(enc->codec_id);

            if (!codec)
            {
                printf("video code not find!\n");

                m_mutex.unlock();
                return  false;
            }

            int err = avcodec_open2(enc, codec, NULL);

            if (err != 0)
            {
                char buf[1024] =   {0};
                av_strerror(err,buf,sizeof(buf));

                printf(buf);
                m_mutex.unlock();
                return 0;
            }

            printf("open codec success! \n");
        }
        else if (enc->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audioStream = i;
            AVCodec *  codec = avcodec_find_decoder(enc->codec_id);
            if (avcodec_open2(enc,codec,NULL) < 0)
            {
                m_mutex.unlock();
                return false;
            }

            this->sampleRate = enc->sample_rate;
            this->channel = enc->channels;
            switch (enc->sample_fmt)
            {

            case AV_SAMPLE_FMT_S16:
                {
                    this->sampleSize = 16;
                    break;
                }
            case AV_SAMPLE_FMT_S32:
                {
                    this->sampleSize = 32;
                    break;

                }
            default:
                break;
            }

            printf("audio sample rate: %d sample size : %d",this->sampleRate,this->sampleSize);

        }
    }

    m_mutex.unlock();

    return m_totalMs;
}


bool XFFmpeg::Close()
{
    m_mutex.lock();
    if (m_ic)
    {
        avformat_close_input(&m_ic);
    }
    if (m_yuv)
    {
        av_frame_free(&m_yuv);
    }

    if (m_cCtx)
    {
        sws_freeContext(m_cCtx);
        m_cCtx = NULL;
    }

    if (m_aCtx)
    {
        swr_free(&m_aCtx);

    }
    m_mutex.unlock();

    return true;
}

bool XFFmpeg::ToRGB(char * out,int outWidth,int outHeight)
{
    m_mutex.lock();
    if (!m_ic || !m_yuv)
    {
        m_mutex.unlock();
        return false;
    }

    AVCodecContext * videoCtx  = m_ic->streams[this->m_videoStream]->codec;
    m_cCtx = sws_getCachedContext(m_cCtx,videoCtx->width,
                                  videoCtx->height,
                                  videoCtx->pix_fmt,
                                  outWidth,outHeight,
                                  AV_PIX_FMT_BGRA,
                                  SWS_BICUBIC,
                                  NULL,NULL,NULL);
    if (!m_cCtx)
    {
        m_mutex.unlock();
        printf("sws_getCachedContext failed! \n");
        return false;
    }
    uint8_t *data [AV_NUM_DATA_POINTERS];

    data[0] = (uint8_t *)out;
    int linesize[AV_NUM_DATA_POINTERS] = {0};
    linesize[0] = outWidth * 4;
    int h = sws_scale(m_cCtx,m_yuv->data,m_yuv->linesize,0,videoCtx->height,
                      data,
                      linesize
                      );
    if (h)
    {
        printf("(%d)", h);
    }
    m_mutex.unlock();
    return true;
}

AVPacket XFFmpeg::Read()
{
    AVPacket pkt;
    memset(&pkt, 0, sizeof (AVPacket));
    m_mutex.lock();
    if (!m_ic)
    {
        m_mutex.unlock();
        return pkt;
    }
    int error = av_read_frame(m_ic,&pkt);
    if (error != 0)
    {
        av_strerror(error,m_errorbuf,sizeof (m_errorbuf));
    }
    m_mutex.unlock();

    return pkt;

}

int XFFmpeg::ToPCM(char *out)
{
   m_mutex.lock();
   if (!m_ic || !m_pcm || !out)
   {
        m_mutex.unlock();
       return 0;
   }

   AVCodecContext * ctx = m_ic ->streams[m_audioStream]->codec;

   if (m_aCtx == NULL)
   {
       m_aCtx = swr_alloc();
       swr_alloc_set_opts(m_aCtx,ctx->channel_layout,
                          AV_SAMPLE_FMT_S16,ctx->sample_rate,ctx->channels,
                          ctx->sample_fmt,ctx->sample_rate,
                          0,0);
       swr_init(m_aCtx);

   }
   uint8_t * data[1];
   data[0] = (uint8_t *) out;
   int len = swr_convert(m_aCtx,data,10000,
                         (const uint8_t **)m_pcm->data,
                         m_pcm->nb_samples);
   if (len <= 0)
   {
       return 0;
   }

   int outsize = av_samples_get_buffer_size(nullptr,ctx->channels,
                                            m_pcm->nb_samples,AV_SAMPLE_FMT_S16,0);
   m_mutex.unlock();
   return  outsize;
}

bool XFFmpeg::Seek(float pos)
{
    m_mutex.lock();
    if (!m_ic)
    {
        m_mutex.unlock();
        return false;
    }
    int64_t stamp = 0;
    stamp = pos * m_ic->streams[m_videoStream]->duration;
    int re = av_seek_frame(m_ic,m_videoStream,stamp,
                           AVSEEK_FLAG_BACKWARD| AVSEEK_FLAG_FRAME );
    avcodec_flush_buffers(m_ic->streams[m_videoStream]->codec); // 释放解码的缓冲

    m_mutex.unlock();

    return re > 0 ?true:false;
}

int XFFmpeg::Decode(const AVPacket * pkt)
{
    m_mutex.lock();
    if (!m_ic)
    {
        m_mutex.unlock();
        return NULL;
    }

    if (!m_yuv)
    {
        m_yuv = av_frame_alloc();
    }

    if (!m_pcm)
    {
        m_pcm = av_frame_alloc();
    }

    AVFrame * frame = m_yuv;
     if (pkt->stream_index == m_audioStream)
    {
        frame = m_pcm;
    }
    int re = avcodec_send_packet(m_ic->streams[pkt->stream_index]->codec,pkt);
    if (re != 0)
    {
        m_mutex.unlock();
        return NULL;
    }
    re = avcodec_receive_frame(m_ic->streams[pkt->stream_index]->codec,frame);
    if (re != 0)
    {
        m_mutex.unlock();
        return NULL;
    }

    m_mutex.unlock();
    int p = (frame->pts *r2d(m_ic->streams[pkt->stream_index]->time_base))*1000;
    if (pkt->stream_index == m_audioStream)
    {
        m_pts = p;
    }
    return p;
}

std::string XFFmpeg::GetError()
{
    m_mutex.lock();
    std::string re = this->m_errorbuf;
    m_mutex.unlock();
    return re;

}
