#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QDir>
#include <QStringList>
#include <QApplication>
#include "stringmanager.h"
#include "track.h"
#include "mime/freedesktopmime.h"

#define DATADIR     QString("%1/.%2").arg(QDir::homePath(), qApp->applicationName())
#define ARTISTSFILE DATADIR + "/artists.dat"
#define ALBUMSFILE  DATADIR + "/albums.dat"
#define TRACKSFILE  DATADIR + "/tracks.dat"
#define ALL         tr(" -All- ")

// DataBase consists of three files:
// 1) artists.dat:
//    - count (int)
//    - strings (StringManager data block)
// 2) albums.dat:
//    - count (int)
//    - strings (StringManager data block)
// 3) tracks.dat:
//    - count (int)
//    - count times:
//      - TrackData
//      - strings (StringManager data block)
// All these files are placed in DATADIR

class DataBase: public QObject
{
    Q_OBJECT

    DataBase(); // Singleton
    ~DataBase();

    // String IDs
    enum Strings
    {
        TrackName,
        ArtistName,
        AlbumName,
        FileName
    };

    // Album data
    struct AlbumData
    {
        QString name;
        QString artist;
    };

    // Track binary data
    struct TrackData
    {
        int trackNo;
        int duration;
    };

public:
    static DataBase *instance();
    static void destroy();

    bool hasArtist(const QString &artist) const { return _artists.contains(artist, Qt::CaseInsensitive); }
    bool hasAlbum(const QString &album) const;
    bool hasTrack(const Track *track) const;
    bool hasTrack(const QString &fileName);

    QStringList artists() const { return _artists; }
    QStringList albums(const QString &artist) const;
    QList<Track*> tracks(const QString &artist, const QString &album) const;
    Track *track(const QString &fileName);

    static bool isSupported(const QString &fileName);

public slots:
    void load();
    void addTrack(Track *track);
    void addDirectory(const QString &dirName);

private slots:
    void loadArtists();
    void loadAlbums();
    void loadTracks();
    void saveArtists();
    void saveAlbums();
    void saveTracks();

signals:
    void artistsUpdated();
    void albumsUpdated();
    void tracksUpdated();
    void dirAddProgress(int done, int total);

private:
    QStringList audioFiles(const QDir &dir);
    QStringList audioFilesFromSubDirs(const QDir &dir);

private:
    static DataBase *_instance;
    static QFreeDesktopMime *_mime;
    QStringList _artists;
    QList<AlbumData*> _albums;
    QList<Track*> _tracks;
    int _doneDirs;
    int _totalDirs;
};

#endif // DATABASE_H
