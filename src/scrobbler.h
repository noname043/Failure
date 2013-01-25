#ifndef SCROBBLER_H
#define SCROBBLER_H

#include <QObject>
#include <lastfm/ws.h>
#include <lastfm/Audioscrobbler.h>
#include <lastfm/Track.h>
#include "track.h"

class Scrobbler : public QObject
{
    Q_OBJECT

    Scrobbler();
    ~Scrobbler();

public:
    static Scrobbler *instance();
    static void destroy();

public slots:
    void nowPlaying(const Track *track);
    void scrobble();

private slots:
    void setUser(QString user);
    void setSession(QString session);
    void onNowPlayingError(int code, QString message);

signals:
    void scrobblerError(int code, QString message);
    
private:
    static Scrobbler *_instance;
    lastfm::Audioscrobbler *_scrobbler;
    lastfm::MutableTrack *_track;
};

#endif // SCROBBLER_H
