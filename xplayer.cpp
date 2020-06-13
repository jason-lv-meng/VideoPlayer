#include "xplayer.h"
#include "ui_xplayer.h"
#include <QFileDialog>
#include <QMessageBox>

#include "xffmpeg.h"
#include "xaudioplay.h"

static bool isPressSlider = false;
static bool isPlay = true;

#define PAUSE "QPushButton:!hover{border-image: url(:/Resources/pause_normal.png);}\
QPushButton:hover{border-image: url(:/Resources/pause_hot.png);}"


#define PLAY "QPushButton:!hover{border-image: url(:/Resources/play_normal.png);}\
QPushButton:hover{border-image: url(:/Resources/play_hot.png);}"

XPlayer::XPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XPlayer)
{
    ui->setupUi(this);
    startTimer(40);

    openFile("1.mp4");
}

XPlayer::~XPlayer()
{
    delete ui;
}

void XPlayer::openFile(const QString & name)
{
    if (name.isEmpty())
    {
        return ;
    }

    this->setWindowTitle(name);
    int totalMs = XFFmpeg::Get()->Open(name.toUtf8());
    if (totalMs <= 0)
    {
        QMessageBox::information(this,"error","file open  failed");
        return ;
    }

    XAudioPlay::Get()->sampleRate = XFFmpeg::Get()->sampleRate;
    XAudioPlay::Get()->channel = XFFmpeg::Get()->channel;
    XAudioPlay::Get()->sampleSize = 16;

    XAudioPlay::Get()->Start();

    char buf[1024] = {0};
    int min = (totalMs/1000)/60;
    int sec = (totalMs/1000)%60;

    sprintf(buf, "%03d:%02d",min,sec);

    ui->totalTime->setText(buf);
    isPlay = false;
    play();
}

void XPlayer::resizeEvent(QResizeEvent * e)
{
    ui->openGLWidget->resize(size());
    ui->playBtn->move(this->width()/2 + 50, this->height() -80);
    ui->openBtn->move(this->width()/2 - 50, this->height() -80);
    ui->playSlider->move(25,this->height() -120);
    ui->playSlider->resize(this->width() -50,ui->playSlider->height());
    ui->playTime->move(25,ui->playBtn->y());
    ui->sp->move(ui->playTime->x()+ui->playTime->width()+ 3,ui->playTime->y());
    ui->totalTime->move(ui->sp->x()+ui->sp->width()+ 3,ui->playTime->y());


}

void XPlayer::play()
{
    isPlay = !isPlay;
    XFFmpeg::Get()->m_isPlay = isPlay;
    if(isPlay)
    {
        // pause
        ui->playBtn->setStyleSheet(PAUSE);
    }
    else
    {
        ui->playBtn->setStyleSheet(PLAY);
    }
}

void XPlayer::open()
{
    QString name =  QFileDialog::getOpenFileName(
                this,QString::fromUtf8("选择视频文件"));

    openFile(name);

}


void XPlayer::timerEvent(QTimerEvent * e)
{
    char buf[1024] = {0};
    int min = (XFFmpeg::Get()->m_pts / 1000)/60;
    int sec = (XFFmpeg::Get()->m_pts/1000)%60;

    sprintf(buf, "%03d:%02d",min,sec);

    ui->playTime->setText(buf);

    if (XFFmpeg::Get()->m_totalMs > 0)
    {
        float rate = (float) XFFmpeg::Get()->m_pts /((float) (XFFmpeg::Get()->m_totalMs));
        if(!isPressSlider)
        {
            ui->playSlider->setValue(rate * 1000);
        }

    }
}

void XPlayer::sliderPress()
{
    isPressSlider = true;
}

void XPlayer::sliderRelease()
{
    isPressSlider = false;

    float pos = 0;
    pos = (float)ui->playSlider->value() / (float)(ui->playSlider->maximum() +1);

    XFFmpeg::Get()->Seek(pos);
}
