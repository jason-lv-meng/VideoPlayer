#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H
#include <QMutex>

class XAudioPlay
{
public:
    static XAudioPlay * Get();
    virtual bool Start() = 0;
    virtual void Play(bool isPlay) = 0;
    virtual bool Write(const char *data, int datasize) = 0;
    virtual ~XAudioPlay();
    virtual int GetFree() = 0;
    int sampleRate = 48000;
    int sampleSize = 16;
    int channel = 2;
protected:
    XAudioPlay();

};

#endif // XAUDIOPLAY_H
