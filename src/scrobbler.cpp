#include "scrobbler.h"
#include "settings.h"
#include "global.h"

Scrobbler *Scrobbler::_instance = 0;

Scrobbler::Scrobbler():
    _scrobbler(new lastfm::Audioscrobbler("tst")),
    _track(new lastfm::MutableTrack)
{
    connect(Settings::instance(), SIGNAL(lastFMUserChanged(QString)), this, SLOT(setUser(QString)));
    connect(Settings::instance(), SIGNAL(lastFMSessionChanged(QString)), this, SLOT(setSession(QString)));
    connect(_scrobbler, SIGNAL(nowPlayingError(int,QString)), this, SLOT(onNowPlayingError(int,QString)));
}

Scrobbler::~Scrobbler()
{
    delete _track;
    delete _scrobbler;
}

Scrobbler *Scrobbler::instance()
{
    if (!_instance)
    {
        lastfm::ws::ApiKey = APIKEY;
        lastfm::ws::SharedSecret = APISECRET;
        lastfm::ws::SessionKey = Settings::instance()->lastFMSession();
        lastfm::ws::Username = Settings::instance()->lastFMUser();
        _instance = new Scrobbler;
    }
    return _instance;
}

void Scrobbler::destroy()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }
}

void Scrobbler::nowPlaying(const Track *track)
{
    _track->setTitle(track->title());
    _track->setArtist(track->artist());
    _track->setAlbum(track->album());
    _track->setTrackNumber(track->track());
    _track->setDuration(track->duration());
    _track->stamp();
    _scrobbler->nowPlaying(*_track);
}

void Scrobbler::scrobble()
{
    _scrobbler->cache(*_track);
    _scrobbler->submit();
}

void Scrobbler::setUser(QString user)
{
    lastfm::ws::Username = user;
}

void Scrobbler::setSession(QString session)
{
    lastfm::ws::SessionKey = session;
}

void Scrobbler::onNowPlayingError(int code, QString message)
{
    emit scrobblerError(code, message);
}
