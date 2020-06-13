#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

#include <QThread>

class XVideoThread : public QThread
{
    Q_OBJECT
public:
    static XVideoThread * Get()
    {
        static XVideoThread vt;

        return &vt;
    }

    volatile bool m_stop = false;
    explicit XVideoThread(QObject *parent = nullptr);

protected:
    void run();
signals:

public slots:

private:


};

#endif // XVIDEOTHREAD_H
