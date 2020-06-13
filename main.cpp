#include "xplayer.h"
#include <QApplication>
#include <iostream>
#include <QIcon>
#include "xffmpeg.h"

using namespace std;

// 要引用c语言函数
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"swscale.lib")


#include <QAudioOutput>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QAudioOutput * out;
//    QAudioFormat fmt;

//    fmt.setSampleRate(48000); // 采样率 一秒钟 48000 个声音

//    fmt.setSampleSize(16);// 一个声音有65535种

//    fmt.setChannelCount(2);// 多声道

//    fmt.setCodec("audio/pcm");

//    fmt.setByteOrder(QAudioFormat::LittleEndian);

//    fmt.setSampleType(QAudioFormat::UnSignedInt);

//    out = new QAudioOutput(fmt);

//    QIODevice * ad = out->start();

//    ad->write();






//    a.setWindowIcon(QIcon(":/img/Resources/logo.ico"));
//    if (XFFmpeg::Get()->Open("1.mp4"))
//    {
//        printf("open video success!\n");
//    }
//    else
//    {
//        printf("open video failed!\n");
//        return -1;
//    }

//    for(;;)
//    {
//        AVPacket pkt = XFFmpeg::Get()->Read();

//        if (pkt.size == 0)
//        {
//            break;
//        }

//        printf("pts = %lld\n",pkt.pts);

//        if (pkt.stream_index != XFFmpeg::Get()->m_videoStream)
//        {
//            av_packet_unref(&pkt);
//            continue;
//        }
//        AVFrame * yuv = XFFmpeg::Get()->Decode(&pkt);
//        if (yuv)
//        {
//            printf("[D]");
//            int outWidth = 640;
//            int outHeight = 480;
//            char *rgb = new  char[outWidth*outHeight * 4];
//            XFFmpeg::Get()->ToRGB(yuv,rgb,outWidth,outHeight);
//        }

//        av_packet_unref(&pkt);
//    }
    //    const char *path = "1.mp4";
    //    //  初始化封装库
    //    av_register_all();

    //    // 初始化网络库，（可以打开 rtsp rtmp http 协议的流媒体视频）
    //    avformat_network_init();

    //    // 参数设置
    //    AVDictionary * opts = NULL;

    //    //    // 设置rtsp流已tcp协议打开
    //    //    av_dict_set(&opts,"rtsp_transport","tcp",0);

    //    //    // 网络延时时间
    //    //    av_dict_set(&opts,"max_delay","500",0);

    //    // 解封装上下文
    //    AVFormatContext *ic = NULL;

    //    int ret =  avformat_open_input(
    //                &ic,
    //                path,
    //                0, // 0 表示自动选择解封装
    //                &opts // 参数设置，比如 rtsp的延时时间
    //                );

    //    if (ret != 0)
    //    {
    //        char buf[1024] = {0};
    //        av_strerror(ret,buf,sizeof (buf)-1);
    //        cout << "open " << path << "failed! :" << buf << endl;
    //        avformat_close_input(&ic);
    //        return -1;

    //    }
    //    else {
    //        int totalSec = ic->duration / AV_TIME_BASE;
    //        printf("file totalSec is %dm-%ds\n",totalSec/60,totalSec%60);

    //        // 打开视频流
    //        int videoStream = 0;
    //        AVCodecContext * videoCtx  = NULL;
    //        for (int i = 0; i < ic->nb_streams; i ++)
    //        {
    //            AVCodecContext *enc = ic ->streams[i]->codec;

    //            if (enc->codec_type == AVMEDIA_TYPE_VIDEO)
    //            {
    //                videoStream = i;
    //                videoCtx = enc;
    //                AVCodec * codec = avcodec_find_decoder(enc->codec_id);

    //                if (!codec)
    //                {
    //                    printf("video code not find!\n");

    //                    return  -1;
    //                }

    //                int err = avcodec_open2(enc, codec, NULL);

    //                if (err != 0)
    //                {
    //                    char buf[1024] =   {0};
    //                    av_strerror(err,buf,sizeof(buf));

    //                    printf(buf);
    //                    return -2;
    //                }

    //                printf("open codec success! \n");
    //            }

    //        }

    //        AVFrame * yuv = av_frame_alloc();
    //        SwsContext *cCtx = NULL;
    //        int outWidth = 640;
    //        int outHeight = 480;
    //        char *rgb = new  char[outWidth*outHeight * 4];
    //        for (;;)
    //        {
    //            AVPacket pkt;
    //            int re = av_read_frame(ic,&pkt);
    //            if (re !=0)
    //            {
    //                continue;
    //            }

    //            if (pkt.stream_index != videoStream)
    //            {
    //                av_packet_unref(&pkt);
    //                continue;
    //            }
    //            int pts = pkt.pts * r2d(ic->streams[pkt.stream_index]->time_base) * 1000;

    //            re = avcodec_send_packet(videoCtx,&pkt);

    //            if (re != 0)
    //            {
    //                av_packet_unref(&pkt);
    //                continue;
    //            }

    //            re =avcodec_receive_frame(videoCtx,yuv);

    //            if (re != 0)
    //            {
    //                av_packet_unref(&pkt);
    //                continue;
    //            }

    //            printf("[D]");

    //            cCtx = sws_getCachedContext(cCtx,videoCtx->width,
    //                                        videoCtx->height,
    //                                        videoCtx->pix_fmt,
    //                                        outWidth,outHeight,
    //                                        AV_PIX_FMT_BGRA,
    //                                        SWS_BICUBIC,
    //                                        NULL,NULL,NULL);
    //            if (!cCtx)
    //            {
    //                 printf("sws_getCachedContext failed! \n");
    //                 break;
    //            }
    //            uint8_t *data [AV_NUM_DATA_POINTERS];

    //            data[0] = (uint8_t *)rgb;
    //            int linesize[AV_NUM_DATA_POINTERS] = {0};
    //            linesize[0] = outWidth * 4;
    //            int h = sws_scale(cCtx,yuv->data,yuv->linesize,0,videoCtx->height,
    //                      data,
    //                      linesize
    //                      );
    //            if (h)
    //            {
    //                printf("(%d)", h);
    //            }
    ////            int got_picture = 0;
    ////            re = avcodec_decode_video2(videoCtx,yuv, &got_picture, &pkt);
    ////            if (got_picture)
    ////            {
    ////                printf("re = [%d]",re);
    ////            }

    //            printf("pts:%lld\n",pts);
    //            av_packet_unref(&pkt);
    //        }

    //        if (cCtx)
    //        {
    //            sws_freeContext(cCtx);
    //            cCtx = NULL;
    //        }


    //        // 释放封装上下文，并把ic置为0
    //        avformat_close_input(&ic);
    //        ic = NULL;
    //    }





    XPlayer w;
    w.show();

    return a.exec();
}
