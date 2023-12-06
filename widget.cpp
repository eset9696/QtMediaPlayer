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
    QWidget::setFixedSize(QWidget::width(), QWidget::height());
    //Buttons style:
    ui->pushButtonOpen->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    QPixmap shuffle("D:\\Qt\\Source\\Repos\\Qt\\MediaPlayer\\Icons\\shuffle.png");
    QPixmap round("D:\\Qt\\Source\\Repos\\Qt\\MediaPlayer\\Icons\\round.png");
    ui->pushButtonShuffle->setIcon(QIcon(shuffle));
    ui->pushButtonRound->setIcon(QIcon(round));


    //Player init:
    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontalSliderVolume->setValue(m_player->volume());
    m_player->setNotifyInterval(100);
    ui->tableviewPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);

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


    //Connections:
    connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player,SIGNAL(positionChanged(qint64)),this,SLOT(on_positionChanged(qint64)));
    connect(m_player,SIGNAL(durationChanged(qint64)),this,SLOT(on_durationChanged(qint64)));
    connect(m_playlist,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChanged(int)));
    connect(ui->pushButtonNext, &QToolButton::clicked, m_playlist, &QMediaPlaylist::next);
    connect(ui->pushButtonPrev, &QToolButton::clicked, m_playlist, &QMediaPlaylist::previous);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, ui->tableviewPlaylist,
            [this](int index)
    {
        ui->labelComposition->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
        this->setWindowTitle("Potato player - " + ui->labelComposition->text());
        ui->tableviewPlaylist->selectRow(index);

    });
    connect(ui->pushButtonClearPlaylist, &QToolButton::clicked, [this]()
    {
        m_playlist->clear();
        m_playlist_model->clear();
    });
    connect(ui->pushButtonDelete, &QToolButton::clicked, [this]()
    {
        QItemSelectionModel* selection =  ui->tableviewPlaylist->selectionModel();
        QModelIndexList rows = selection->selectedRows();
        std::sort(rows.begin(), rows.end());
        int num_deleted = 0;
        for (QModelIndex element:rows) {
            m_playlist_model->removeRow(element.row() - num_deleted);
            m_playlist->removeMedia(element.row() - num_deleted);
            num_deleted++;
        }
    });

    ui->tableviewPlaylist->hideColumn(1);
    ui->tableviewPlaylist->horizontalHeader()->setStretchLastSection(true);
    ui->tableviewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadPlaylist(DEFAULT_PLAYLIST);
    m_player->setPlaylist(m_playlist);
}
Widget::~Widget()
{
    savePlaylist(DEFAULT_PLAYLIST);
    delete m_playlist;
    delete m_playlist_model;
    delete taskbarButton;
    delete m_player;
    delete ui;
}


void Widget::on_pushButtonOpen_clicked()
{

    QStringList files = QFileDialog::getOpenFileNames(
                this,
                tr("Open file"),
                "D:\\music",
                tr("Audio files (*.mp3 *.flac);; MP-3 (*.mp3);; Flac (*.flac);; Playlists (*.m3u *.cue);; M3U (*.m3u);; CUE (*.cue)")
                );
    if(!(files.last().split('.').last() == "cue") && !(files.last().split('.').last() == "m3u"))
    {
        for (QString filesPath: files)
        {
            QList<QStandardItem*> items;
            items.append(new QStandardItem(QDir(filesPath).dirName()));
            items.append(new QStandardItem(filesPath));
            m_playlist_model->appendRow(items);
            m_playlist->addMedia(QUrl(filesPath));
        }
        ui->labelDebug->setText("hello");
    }
    else
    {
        if(files.last().split('.').last() == "m3u")loadPlaylist(files.last());
        if(files.last().split('.').last() == "cue")
        {
            load_cue_playlist(files.last());
            isCuePlaylist = true;
        }
    }
    m_player->setPlaylist(m_playlist);
}


void Widget::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(value)));
}


void Widget::on_pushButtonPlayPause_clicked()
{
    if(played)
    {
        played = false;
        m_player->pause();
        ui->pushButtonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    else
    {
        played = true;
        m_player->play();
        ui->pushButtonPlayPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }

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
    ui->labelProgress->setText(qt_position.toString("hh:mm:ss"));
    taskbar->setValue(progress);
}

void Widget::on_durationChanged(qint64 duration)
{
    ui->labelComposition->text();
    ui->horizontalSliderProgress->setMaximum(duration);
    QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);
    ui->labelDuration->setText(QString("Duration:").append(qt_duration.toString("hh:mm:ss")));
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

void Widget::on_tableviewPlaylist_doubleClicked(const QModelIndex &index)
{
    if(!isCuePlaylist)m_playlist->setCurrentIndex(index.row());
    else
    {
        qint64 position = to_mseconds(tracklist.take(index.data().toString()));
        ui->horizontalSliderProgress->setValue(position);
        m_player->setPosition(position);
        ui->labelComposition->setText(index.data().toString());
        ui->labelDebug->setText(QString::number(index.row()));
    }
}


void Widget::on_pushButtonRound_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::PlaybackMode::Loop);
}


void Widget::currentIndexChanged(int index)
{
    ui->tableviewPlaylist->selectRow(index);
}


void Widget::savePlaylist(QString filename)
{
    QString format = filename.split('.').last();
    m_playlist->save(QUrl::fromLocalFile(DEFAULT_PLAYLIST), format.toStdString().c_str());
    ui->labelComposition->setText("save done");
}

void Widget::loadPlaylist(QString filename)
{
    QString format = filename.split('.').last();
    m_playlist->load(QUrl::fromLocalFile(DEFAULT_PLAYLIST), format.toStdString().c_str());
    for (int i = 0; i < m_playlist->mediaCount(); i++)
    {
        QMediaContent m_content = m_playlist->media(i);
        QString url = m_content.canonicalUrl().url();
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(url).dirName()));
        items.append(new QStandardItem(url));
        m_playlist_model->appendRow(items);
    }
}

void Widget::load_cue_playlist(QString filename)
{
    QString performer;
    QString flac_file;
    QFile file(filename);
    QDir dir = QFileInfo(file).absoluteDir();
    QString full_name;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    while(!file.atEnd())
    {
        QString buffer = file.readLine();
        if(buffer.split(' ')[0] == "PERFORMER") performer = buffer.remove(0, strlen("PERFORMER") + 1);
        if(buffer.split(' ')[0] == "FILE")
        {
            QString path = dir.absolutePath();
            flac_file = buffer.remove("FILE \"").remove("\" WAVE\n");
            full_name = path + "/" + flac_file;
        }
        if(buffer.contains("TRACK"))
        {
            QString composition;
            do
            {
                buffer = file.readLine();
                QString title;

                if(buffer.contains("TITLE"))
                {
                    title = buffer.remove(0, strlen("    TITLE") + 1).remove("\"").remove("\n").remove("*");
                    composition = performer.remove("\"").remove("\n") + " - " + title;
                    QList<QStandardItem*> items;
                    items.append(new QStandardItem(composition));
                    items.append(new QStandardItem(full_name));
                    m_playlist_model->appendRow(items);
                }

                if(buffer.contains("INDEX 01"))
                {
                    QString index = buffer.remove(0, strlen("    INDEX 01") + 1).remove("\n");
                    tracklist.insert(composition, index);
                    break;
                }
            }
            while(!buffer.contains("INDEX 01"));
        }
    }
    m_playlist->addMedia(QUrl(full_name));
    file.close();
}

int Widget::to_mseconds(const QString str)
{
    int minutes = str.split(':')[0].toInt();
    int seconds = str.split(':')[1].toInt();
    int mseconds = str.split(':')[2].toInt();
    return mseconds + seconds * 1000 + minutes * 60000;
}




