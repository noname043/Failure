#include "player.h"
#include <QApplication>
#include <QTextCodec>
#include "database.h"
#include "settings.h"
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.addLibraryPath(a.applicationDirPath());
    a.setApplicationName("Failure");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Noname043");
    a.setQuitOnLastWindowClosed(false);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    lastfm::ws::ApiKey = APIKEY;
    lastfm::ws::SharedSecret = APISECRET;
    lastfm::ws::Username = Settings::instance()->lastFMUser();
    lastfm::ws::SessionKey = Settings::instance()->lastFMSession();

    Player w;
    w.adjustSize();
    w.show();

    DataBase::instance()->load();
    
    return a.exec();
}
