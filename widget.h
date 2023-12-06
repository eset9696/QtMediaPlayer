#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QtWinExtras>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QToolButton>
#include <algorithm>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButtonOpen_clicked();

    void on_horizontalSliderVolume_valueChanged(int value);

    void on_pushButtonPlayPause_clicked();

    void mediaStatusChanged(QMediaPlayer::MediaStatus state);

    void on_pushButtonPause_clicked();

    void on_positionChanged(qint64 progress);

    QString secondsToMinutes(const qint64 seconds);

    void on_horizontalSliderProgress_sliderMoved(int position);

    void on_horizontalSliderProgress_sliderPressed();

    void on_horizontalSliderProgress_sliderReleased();

    void on_durationChanged(qint64 duration);

    void on_pushButtonStop_clicked();

    void showEvent(QShowEvent *event);

    void on_pushButtonMute_clicked();

    void on_tableviewPlaylist_doubleClicked(const QModelIndex &index);

    void on_pushButtonRound_clicked();

    void currentIndexChanged(int index);

    void savePlaylist(QString filename);

    void loadPlaylist(QString filename);

    void load_cue_playlist(QString filename);

    int to_mseconds(const QString str);

private:
    Ui::Widget *ui;

    QMediaPlayer* m_player;

    QWinTaskbarButton* taskbarButton;

    QWinTaskbarProgress* taskbar;

    bool muted = false;

    bool played = false;

    bool isCuePlaylist = false;

    QModelIndexList rows;

    CONST CHAR* DEFAULT_PLAYLIST = "D:/Qt/Source/Repos/Qt/MediaPlayer/Save/playlist.m3u8";

    QMediaPlaylist* m_playlist;

    QList<QString> indexes;

    QStandardItemModel* m_playlist_model;

    QMap<QString, QString> tracklist;

    QList<QString> timings;
};
#endif // WIDGET_H
