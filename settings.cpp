#include "settings.h"

Settings *Settings::_instance = 0;

Settings::Settings()
{}

Settings::~Settings()
{}

Settings *Settings::instance()
{
    if (!_instance)
        _instance = new Settings;
    return _instance;
}

void Settings::destroy()
{
    if (_instance)
        delete _instance;
    _instance = 0;
}

QString Settings::lastFMUser()
{
    return _settings.value(LASTFMUSER).toString();
}

void Settings::setLastFMUser(const QString &user)
{
    if (user == lastFMUser())
        return;
    _settings.setValue(LASTFMUSER, user);
    emit lastFMUserChanged(user);
}

QString Settings::lastFMSession()
{
    return _settings.value(LASTFMSESSION).toString();
}

void Settings::setLastFMSession(const QString &session)
{
    if (session == lastFMSession())
        return;
    _settings.setValue(LASTFMSESSION, session);
    emit lastFMSessionChanged(session);
}

bool Settings::isScrobblingEnabled()
{
    return _settings.value(SCROBBLING).toBool();
}

void Settings::setScroblingEnabled(bool scrobbling)
{
    if (scrobbling == isScrobblingEnabled())
        return;
    _settings.setValue(SCROBBLING, scrobbling);
    emit scrobblingChanged(scrobbling);
}
