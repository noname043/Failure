#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define LASTFMUSER "LastFMUsername"
#define LASTFMSESSION "LastFMSession"
#define SCROBBLING "Scrobbling"

class Settings: public QObject
{
    Q_OBJECT

    Settings();
    ~Settings();

public:
    static Settings *instance();
    static void destroy();

    QString lastFMUser();
    void setLastFMUser(const QString &user);

    QString lastFMSession();
    void setLastFMSession(const QString &session);

    bool isScrobblingEnabled();
    void setScroblingEnabled(bool scrobbling);

signals:
    void lastFMUserChanged(QString user);
    void lastFMSessionChanged(QString session);
    void scrobblingChanged(bool enabled);

private:
    static Settings *_instance;
    QSettings _settings;
};

#endif // SETTINGS_H
