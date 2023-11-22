#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QtWinExtras>

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

    void on_pushButtonPlay_clicked();

    void mediaStatusChanged(QMediaPlayer::MediaStatus state);

    void on_pushButtonPause_clicked();

    void progressChanged(qint64 progress);

    QString secondsToMinutes(const qint64 seconds);

    void on_horizontalSliderProgress_sliderMoved(int position);

    void on_horizontalSliderProgress_sliderPressed();

    void on_horizontalSliderProgress_sliderReleased();

    void on_pushButtonStop_clicked();

    void showEvent(QShowEvent *event);
private:
    Ui::Widget *ui;

    QMediaPlayer* m_player;

    QWinTaskbarButton* taskbarButton;

    QWinTaskbarProgress* taskbar;
};
#endif // WIDGET_H
