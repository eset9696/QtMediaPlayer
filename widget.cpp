#include "widget.h"
#include "ui_widget.h"
#include "QStyle"
#include "QFileDialog"
#include "QDir"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //Buttons style:
    ui->pushButtonOpen->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    //Player init:
    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontalSliderVolume->setValue(m_player->volume());
    m_player->setNotifyInterval(100);

    //Connections:
    connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player,SIGNAL(positionChanged(qint64)),this,SLOT(progressChanged(qint64)));

    //taskbarprogress
    taskbarButton = new QWinTaskbarButton(this);
    taskbarButton->setWindow(windowHandle());
    taskbar = taskbarButton->progress();
    taskbar->show();

}
Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButtonOpen_clicked()
{
    QString file = QFileDialog::getOpenFileName(
                this,               //Parent window
                tr("Open file"),    //Title of dialog window
                "D:\\Музыка\\Моя музыка",            //Рабочий каталог
                tr("Audio files (*.mp3 *.flac)")
                );
    ui->labelComposition->setText(file);
    m_player->setMedia(QUrl::fromLocalFile(file));
}


void Widget::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
}


void Widget::on_pushButtonPlay_clicked()
{
    m_player->play();
    ui->labelDuration->setText(secondsToMinutes(m_player->duration()/1000));
    taskbar->resume();
}


void Widget::mediaStatusChanged(QMediaPlayer::MediaStatus state)
{
    //ui->labelProgress->setText("pause");
}

void Widget::on_pushButtonPause_clicked()
{
    m_player->pause();
    taskbar->pause();
}


void Widget::progressChanged(qint64 pos)
{
    ui->labelProgress->setText(secondsToMinutes(m_player->position()/1000));
    ui->horizontalSliderProgress->setValue((int)((double)m_player->position()/(m_player->duration()/100)));
    taskbar->setValue((int)((double)m_player->position()/(m_player->duration()/100)));
}


QString Widget::secondsToMinutes(const qint64 seconds)
{
    QString minutes = QString::number(seconds / 60);
    QString sec = QString::number(seconds % 60);
    QString result = minutes + ":" + (sec.length() == 1 ? "0" + sec: sec);
    return result;
}


void Widget::on_horizontalSliderProgress_sliderMoved(int position)
{
    m_player->setPosition(position * m_player->duration()/100);
    ui->labelProgress->setText(secondsToMinutes(position * m_player->duration()/100000));
}


void Widget::on_horizontalSliderProgress_sliderPressed()
{
    m_player->pause();
}


void Widget::on_horizontalSliderProgress_sliderReleased()
{
    m_player->play();
}


void Widget::on_pushButtonStop_clicked()
{
    m_player->stop();
    taskbar->stop();
}

void Widget::showEvent(QShowEvent *event)
{
    taskbarButton->setWindow(windowHandle());
    event->accept();
}





