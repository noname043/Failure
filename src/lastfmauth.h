#ifndef LASTFMAUTH_H
#define LASTFMAUTH_H

#include <QNetworkAccessManager>
#include <QDesktopServices>

#define APIBASE "http://ws.audioscrobbler.com/2.0/"
#define AUTHURL "http://www.last.fm/api/auth/"

class LastFMAuth: public QObject
{
    Q_OBJECT
public:
    enum State
    {
        Idle,
        RequestingToken,
        AwaitingAuth,
        RequestingSession
    };

    LastFMAuth(const QString &apiKey, const QString &apiSecret, QObject *parent = 0);
    ~LastFMAuth();
    
    QString token() const { return _token; }
    QString session() const { return _session; }
    State state() const { return _state; }

public slots:
    //bool checkSession(const QString &session = "");
    bool getToken(QString *error = 0);
    bool openAuthPage();
    bool getSession(QString *error = 0);

signals:
    void tokenReceived(bool error);
    void sessionReceived(bool error);

private slots:
    // This code is so simple, that makeSig() does not have to sort params.
    QString makeSig(const QStringList &keysWithVals);
    void replyReceived(QNetworkReply *reply);

private:
    QString _apiKey;
    QString _apiSecret;
    QString _token;
    QString _session;
    QNetworkAccessManager *_network;
    QNetworkReply *_reply;
    State _state;
    QString *_errorString;
};

#endif // LASTFMAUTH_H
