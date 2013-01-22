#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <QListWidgetItem>
#include <QSystemTrayIcon>
#include "ui_player.h"
#include "track.h"

class Player: public QMainWindow
{
    Q_OBJECT

public:
    Player(QWidget *parent = 0);
    ~Player();

private slots:
    void savePlaylist() {}
    void loadPlaylist() {}
    void addFilesToLibrary();
    void addDirToLibrary();

    void playPrevious();
    void play();
    void pause() {}
    void stop();
    void playNext();

    void updateArtists();
    void updateAlbums();
    void updateTracks();

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void totalTimeChanged(qint64 newTotalTime);
    void tick(qint64 time);
    void trackFinished();

    void showArtistsContextMenu(const QPoint &point);
    void showAlbumsContextMenu(const QPoint &point);
    void showTracksContextMenu(const QPoint &point);

    void addToPlaylist(const Track *track);
    void addToPlaylist(const QList<Track*> &tracks);
    void clearPlaylist();

private:
    void fixHeader(QHeaderView *header);

private:
    Ui::Player *_ui;
    Phonon::MediaObject *_player;
    Phonon::AudioOutput *_audioOutput;
    QListWidgetItem *_allArtists;
    QListWidgetItem *_allAlbums;
    QSystemTrayIcon *_trayIcon;
    QList<Track*> _libTracks;
    QList<Track*> _plistTracks;
    bool _isStopped;

    QAction *_artistToPlaylistAction;
    QMenu *_artistsMenu;
    QAction *_albumToPlaylistAction;
    QMenu *_albumsMenu;
    QAction *_trackToPlaylistAction;
    QMenu *_tracksMenu;
};

#endif // MAINWINDOW_H
