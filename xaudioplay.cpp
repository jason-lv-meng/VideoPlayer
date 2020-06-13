#include "xaudioplay.h"
#include <QAudioOutput>
#include <QIODevice>

class CXAudioPlay:public XAudioPlay
{
public:
    QAudioOutput * output = nullptr;
    QIODevice * io = nullptr;

    QMutex m_mutex;
    bool Start()
    {
        stop();

        m_mutex.lock();
        QAudioFormat fmt;

        fmt.setSampleRate(this->sampleRate); // 采样率 一秒钟 48000 个声音

        fmt.setSampleSize(this->sampleSize);// 一个声音有65535种

        fmt.setChannelCount(this->channel);// 多声道

        fmt.setCodec("audio/pcm");

        fmt.setByteOrder(QAudioFormat::LittleEndian);

        fmt.setSampleType(QAudioFormat::UnSignedInt);
        output = new QAudioOutput(fmt);
        io = output->start();
        m_mutex.unlock();
        return true;
    }

    bool stop()
    {
        m_mutex.lock();
        if (output)
        {
            output->stop();
            delete output;
            output = nullptr;
            io = nullptr;

        }
        m_mutex.unlock();
        return true;
    }

    void Play(bool isPlay)
    {
        m_mutex.lock();
        if (!output)
         {
             m_mutex.unlock();
            return ;
        }
        if (isPlay)
        {
            output->resume();
        }
        else {
            output->suspend();
        }
        m_mutex.unlock();
    }
    int GetFree()
    {
        m_mutex.lock();
        if (!output)
        {
            m_mutex.unlock();
            return 0;
        }
        int len = output->bytesFree();
        m_mutex.unlock();
        return len;
    }

    bool Write(const char *data, int datasize)
    {
        m_mutex.lock();
        if (io)
        {
            io->write(data,datasize);
        }

        m_mutex.unlock();
        return true;
    }

    ~CXAudioPlay(){};

};

XAudioPlay::XAudioPlay()
{

}

XAudioPlay::~XAudioPlay()
{

}

XAudioPlay * XAudioPlay::Get()
{
    static CXAudioPlay ap;
    return  &ap;
}
