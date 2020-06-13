#ifndef XPLAYER_H
#define XPLAYER_H

#include <QWidget>

namespace Ui {
class XPlayer;
}

class XPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit XPlayer(QWidget *parent = nullptr);
    ~XPlayer();
    void timerEvent(QTimerEvent * e);
    void resizeEvent(QResizeEvent * e);

public slots:
    void open();
    void sliderPress();
    void sliderRelease();
    void play();
protected:
    void openFile(const QString & name);

private:
    Ui::XPlayer *ui;


};

#endif // XPLAYER_H
