#include "lastfmauth.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QCryptographicHash>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

LastFMAuth::LastFMAuth(const QString &apiKey, const QString &apiSecret, QObject *parent):
    QObject(parent),
    _apiKey(apiKey),
    _apiSecret(apiSecret),
    _network(new QNetworkAccessManager(this)),
    _reply(0),
    _state(Idle),
    _errorString(0)
{
    connect(_network, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)));
}

LastFMAuth::~LastFMAuth()
{}

bool LastFMAuth::getToken(QString *error)
{
    _state = RequestingToken;
    _errorString = error;
    QUrl url(APIBASE);
    url.addQueryItem("method", "auth.getToken");
    url.addQueryItem("api_key", _apiKey);
    QNetworkRequest request(url);
    _reply = _network->get(request);
    if (!_reply)
    {
        _state = Idle;
        if (error)
            *error = tr("Failed to make request");
        return false;
    }
    return true;
}

bool LastFMAuth::openAuthPage()
{
    _state = AwaitingAuth;
    QUrl url(AUTHURL);
    url.addQueryItem("api_key", _apiKey);
    url.addQueryItem("token", _token);
    bool r = QDesktopServices::openUrl(url);
    if (!r)
        _state = Idle;
    return r;
}

bool LastFMAuth::getSession(QString *error)
{
    _state = RequestingSession;
    _errorString = error;
    QUrl url(APIBASE);
    QStringList l;
    // Oh mom, what have I done...
    l.append(QString("%1%2").arg("api_key", _apiKey));
    l.append(QString("methodauth.getSession"));
    l.append(QString("%1%2").arg("token", _token));
    l.append(_apiSecret);
    url.addQueryItem("method", "auth.getSession");
    url.addQueryItem("api_key", _apiKey);
    url.addQueryItem("token", _token);
    url.addQueryItem("api_sig", makeSig(l));
    QNetworkRequest request(url);
    _reply = _network->get(request);
    if (!_reply)
    {
        _state = Idle;
        if (error)
            *error = tr("Failed to make request");
        return false;
    }
    return true;
}

QString LastFMAuth::makeSig(const QStringList &keysWithVals)
{
    QString r = keysWithVals.join("");
    return QCryptographicHash::hash(r.toUtf8(), QCryptographicHash::Md5).toHex().data();
}

void LastFMAuth::replyReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        if (_errorString)
            *_errorString = reply->errorString();
        State old = _state;
        _state = Idle;
        switch (old)
        {
        case RequestingToken:
            emit tokenReceived(true);
            return;
        case RequestingSession:
            emit sessionReceived(true);
            return;
        default: return;
        }
    }

    if (_state == RequestingToken)
    {
        QDomDocument document;
        QString data;
        document.setContent(reply);
        QDomElement elem = document.firstChildElement("lfm").firstChildElement("error");
        data = elem.text();
        if (!data.isEmpty())
        {
            if (_errorString)
                *_errorString = data;
            emit tokenReceived(true);
        }
        else
        {
            _token = document.firstChildElement("lfm").firstChildElement("token").text();
            emit tokenReceived(false);
        }
    }
    else if (_state == RequestingSession)
    {
        QDomDocument document;
        QString data;
        document.setContent(reply);
        QDomElement elem = document.firstChildElement("lfm").firstChildElement("error");
        data = elem.text();
        if (!data.isEmpty())
        {
            if (_errorString)
                *_errorString = data;
            emit sessionReceived(true);
        }
        else
        {
            _session = document.firstChildElement("lfm").firstChildElement("session").firstChildElement("key").text();
            emit sessionReceived(false);
        }
    }

    _state = Idle;
}
