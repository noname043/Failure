#ifndef LASTFMAUTHDIALOG_H
#define LASTFMAUTHDIALOG_H

#include "ui_lastfmauthdialog.h"
#include "lastfmauth.h"

class LastFMAuthDialog: public QDialog
{
    Q_OBJECT

public:
    LastFMAuthDialog(QWidget *parent = 0);
    ~LastFMAuthDialog();

public slots:
    void login();
    void finishLogin();
    void logout();

private slots:
    void setLoggedIn();
    void setLoggedOut();

    void tokenReceived(bool error);
    void sessionReceived(bool error);

private:
    Ui::LastFMAuthDialog *_ui;
    LastFMAuth *_auth;
    QString _errorString;
};

#endif // LASTFMAUTHDIALOG_H
