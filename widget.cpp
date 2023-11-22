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
    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    //Player init:
    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontalSliderVolume->setValue(m_player->volume());
    m_player->setNotifyInterval(100);

    //Connections:
    connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player,SIGNAL(positionChanged(qint64)),this,SLOT(on_positionChanged(qint64)));
    connect(m_player,SIGNAL(durationChanged(qint64)),this,SLOT(on_durationChanged(qint64)));

    //taskbarprogress:
    taskbarButton = new QWinTaskbarButton(this);
    taskbarButton->setWindow(windowHandle());
    taskbar = taskbarButton->progress();
    taskbar->show();

    //playlist:

    m_playlist_model = new QStandardItemModel(this);
    ui->tableviewPlaylist->setModel(m_playlist_model);
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path");
    m_playlist = new QMediaPlaylist(m_player);
    ui->tableviewPlaylist->hideColumn(1);
    ui->tableviewPlaylist->horizontalHeader()->setStretchLastSection(true);
    ui->tableviewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);



}
Widget::~Widget()
{
    delete m_playlist;
    delete m_playlist_model;
    delete taskbarButton;
    delete m_player;
    delete ui;
}


void Widget::on_pushButtonOpen_clicked()
{
//    QString file = QFileDialog::getOpenFileName(
//                this,               //Parent window
//                tr("Open file"),    //Title of dialog window
//                "D:\\Музыка\\Моя музыка",            //Рабочий каталог
//                tr("Audio files (*.mp3 *.flac)")
//                );
//    m_player->setMedia(QUrl::fromLocalFile(file));
//    ui->labelComposition->setText(file.split('/').last());
//    this->setWindowTitle(QString("Potato Music - ").append((file.split('/').last())));
    QStringList files = QFileDialog::getOpenFileNames(
                this,
                tr("Open file"),
                "D:\\Музыка\\Моя музыка",
                tr("Audio files (*.mp3 *.flac)")
                );
    for (QString filesPath: files)
    {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(filesPath).dirName()));
        items.append(new QStandardItem(filesPath));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl(filesPath));
    }
    m_player->setPlaylist(m_playlist);
}


void Widget::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
}


void Widget::on_pushButtonPlay_clicked()
{
    m_player->play();
    //ui->labelDuration->setText(secondsToMinutes(m_player->duration()/1000));
    taskbar->resume();
}


void Widget::mediaStatusChanged(QMediaPlayer::MediaStatus state)
{

}

void Widget::on_pushButtonPause_clicked()
{
    m_player->pause();
    taskbar->pause();
}


void Widget::on_positionChanged(qint64 progress)
{
    ui->horizontalSliderProgress->setValue(progress);
    QTime qt_position = QTime::fromMSecsSinceStartOfDay(progress);
    ui->labelProgress->setText(qt_position.toString("mm:ss"));
    taskbar->setValue(progress);
}

void Widget::on_durationChanged(qint64 duration)
{
   ui->horizontalSliderProgress->setMaximum(duration);
   QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);
   ui->labelDuration->setText(QString("Duration:").append(qt_duration.toString("mm:ss")));
   taskbar->setMaximum(duration);
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
    m_player->setPosition(position);
    //ui->labelProgress->setText(secondsToMinutes(position * m_player->duration()/100000));
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
    this->setWindowTitle(QString("Potato Music"));
}

void Widget::showEvent(QShowEvent *event)
{
    taskbarButton->setWindow(windowHandle());
    event->accept();
}


void Widget::on_pushButtonMute_clicked()
{
    muted = !muted;
    m_player->setMuted(muted);
    ui->pushButtonMute->setIcon(style()->standardIcon(muted?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
}


void Widget::on_pushButtonPrev_clicked()
{
    m_playlist->previous();
}


void Widget::on_pushButtonNext_clicked()
{
    m_playlist->next();
}


void Widget::on_tableviewPlaylist_doubleClicked(const QModelIndex &index)
{
    //m_playlist->setCurrentIndex(index);
}

