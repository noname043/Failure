#include "lastfmauthdialog.h"
#include <QMessageBox>
#include "settings.h"
#include "global.h"

LastFMAuthDialog::LastFMAuthDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::LastFMAuthDialog),
    _auth(new LastFMAuth(APIKEY, APISECRET, this))
{
    _ui->setupUi(this);

    connect(_ui->loginButton, SIGNAL(clicked()), this, SLOT(login()));
    connect(_ui->finishLoginButton, SIGNAL(clicked()), this, SLOT(finishLogin()));
    connect(_ui->logoutButton, SIGNAL(clicked()), this, SLOT(logout()));
    connect(_ui->closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(_auth, SIGNAL(tokenReceived(bool)), this, SLOT(tokenReceived(bool)));
    connect(_auth, SIGNAL(sessionReceived(bool)), this, SLOT(sessionReceived(bool)));

    _ui->username->setText(Settings::instance()->lastFMUser());
    if (Settings::instance()->isScrobblingEnabled()
        && !Settings::instance()->lastFMSession().isEmpty())
        setLoggedIn();
}

LastFMAuthDialog::~LastFMAuthDialog()
{}

void LastFMAuthDialog::setLoggedIn()
{
    _ui->loginButton->setEnabled(false);
    _ui->finishLoginButton->setEnabled(false);
    _ui->logoutButton->setEnabled(true);
    _ui->username->setEnabled(false);
}

void LastFMAuthDialog::setLoggedOut()
{
    _ui->loginButton->setEnabled(true);
    _ui->finishLoginButton->setEnabled(false);
    _ui->logoutButton->setEnabled(false);
    _ui->username->setEnabled(true);
}

void LastFMAuthDialog::login()
{
    if (_ui->username->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Hola!"), tr("Your username, please!"));
        return;
    }
    if (!_auth->getToken(&_errorString))
        QMessageBox::warning(this, tr("Error!"), _errorString);
    else
    {
        _ui->loginButton->setEnabled(false);
        _ui->username->setEnabled(false);
    }
}

void LastFMAuthDialog::tokenReceived(bool error)
{
    if (error)
    {
        setLoggedOut();
        QMessageBox::warning(this, tr("Error!"), _errorString);
    }
    else
    {
        if (!_auth->openAuthPage())
        {
            setLoggedOut();
            QMessageBox::warning(this, tr("Error!"), tr("Could not open auth page!"));
        }
        else
            _ui->finishLoginButton->setEnabled(true);
    }
}

void LastFMAuthDialog::finishLogin()
{
    if (!_auth->getSession(&_errorString))
        QMessageBox::warning(this, tr("Error!"), _errorString);
}

void LastFMAuthDialog::sessionReceived(bool error)
{
    if (error)
    {
        QMessageBox::warning(this, tr("Error!"), _errorString + tr("<br>Authorize application and try again."));
    }
    else
    {
        Settings::instance()->setLastFMSession(_auth->session());
        Settings::instance()->setLastFMUser(_ui->username->text());
        setLoggedIn();
        this->close();
    }
}

void LastFMAuthDialog::logout()
{
    setLoggedOut();
    Settings::instance()->setScroblingEnabled(false);
}

void LastFMAuthDialog::onClose()
{
    // When user is not properly logged in - turn off scrobbling.
    if (!_ui->logoutButton->isEnabled())
        logout();
}
