#include "database.h"
#include <QFile>
#include <phonon/BackendCapabilities>
#include "mime/freedesktopmime.h"

DataBase *DataBase::_instance = 0;

DataBase::DataBase()
{
    QDir dir(DATADIR);
    if (!dir.exists())
    {
        dir.mkpath(DATADIR);
    }
}

DataBase::~DataBase()
{
    for (int i = 0; i < _albums.size(); ++i)
        delete _albums[i];
    for (int i = 0; i < _tracks.size(); ++i)
        delete _tracks[i];
}

DataBase *DataBase::instance()
{
    if (!_instance)
        _instance = new DataBase;
    return _instance;
}

void DataBase::destroy()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }
}

void DataBase::load()
{
    _artists.clear();
    for (int i = 0; i < _albums.size(); ++i)
        delete _albums[i];
    _albums.clear();
    for (int i = 0; i < _tracks.size(); ++i)
        delete _tracks[i];
    _tracks.clear();
    loadArtists();
    loadAlbums();
    loadTracks();
}

bool DataBase::hasTrack(const Track *track) const
{
    for (int i = 0; i < _tracks.size(); ++i)
    {
        if (!QString::compare(track->fileName(), _tracks[i]->fileName(), Qt::CaseInsensitive))
            return true;
    }
    return false;
}

void DataBase::addTrack(Track *track)
{
    if (hasTrack(track))
        return;
    if (!hasArtist(track->artist()))
    {
        _artists.append(track->artist());
        saveArtists();
        emit artistsUpdated();
    }
    if (!hasAlbum(track->album()))
    {
        AlbumData *data = new AlbumData;
        data->name = track->album();
        data->artist = track->artist();
        _albums.append(data);
        saveAlbums();
        emit albumsUpdated();
    }
    _tracks.append(track);
    saveTracks();
    emit tracksUpdated();
}

void DataBase::loadArtists()
{
    QFile file(ARTISTSFILE);
    if (!file.open(QFile::ReadOnly))
        return;

    int count;
    StringManager data;
    file.read((char*) &count, sizeof(count));
    data.load(file);
    file.close();
    for (int i = 0; i < count; ++i)
        _artists.append(data.string(i));
    emit artistsUpdated();
}

void DataBase::saveArtists()
{
    int count = _artists.size();
    StringManager data;
    QFile file(ARTISTSFILE);
    file.open(QFile::WriteOnly);
    file.write((char*) &count, sizeof(count));
    for (int i = 0; i < count; ++i)
        data.insert(i, _artists[i]);
    data.save(file);
    file.close();
}

void DataBase::loadAlbums()
{
    QFile file(ALBUMSFILE);
    if (!file.open(QFile::ReadOnly))
        return;

    int count;
    StringManager strings;
    AlbumData *data = 0;
    file.read((char*) &count, sizeof(count));
    strings.load(file);
    file.close();
    for (int i = 0; i < count; ++i)
    {
        data = new AlbumData;
        data->name = strings.string(i*2);
        data->artist = strings.string(i*2+1);
        _albums.append(data);
    }
    emit albumsUpdated();
}

void DataBase::saveAlbums()
{
    int count = _albums.size();
    StringManager data;
    QFile file(ALBUMSFILE);
    file.open(QFile::WriteOnly);
    file.write((char*) &count, sizeof(count));
    for (int i = 0; i < count; ++i)
    {
        data.insert(i*2, _albums[i]->name);
        data.insert(i*2+1, _albums[i]->artist);
    }
    data.save(file);
    file.close();
}

void DataBase::loadTracks()
{
    QFile file(TRACKSFILE);
    if (!file.open(QFile::ReadOnly))
        return;

    int count;
    TrackData data;
    StringManager strings;
    Track *track = 0;

    file.read((char*) &count, sizeof(count));
    for (int i = 0; i < count; ++i)
    {
        file.read((char*) &data, sizeof(data));
        strings.load(file);

        track = new Track;
        track->setArtist(strings.string(ArtistName));
        track->setAlbum(strings.string(AlbumName));
        track->setTitle(strings.string(TrackName));
        track->setFileName(strings.string(FileName));
        track->setTrack(data.trackNo);
        track->setDuration(data.duration);
        _tracks.append(track);
    }
    file.close();
    emit tracksUpdated();
}

void DataBase::saveTracks()
{
    int count = _tracks.size();
    TrackData data;
    StringManager strings;
    QFile file(TRACKSFILE);
    file.open(QFile::WriteOnly);
    file.write((char*) &count, sizeof(count));
    for (int i = 0; i < count; ++i)
    {
        data.duration = _tracks[i]->duration();
        data.trackNo = _tracks[i]->track();
        strings.clear();
        strings.insert(ArtistName, _tracks[i]->artist());
        strings.insert(AlbumName, _tracks[i]->album());
        strings.insert(TrackName, _tracks[i]->title());
        strings.insert(FileName, _tracks[i]->fileName());

        file.write((char*) &data, sizeof(data));
        strings.save(file);
    }
    file.close();
}

void DataBase::addDirectory(const QString &dirName)
{
    QStringList fileNames;
    QDir dir(dirName);
    bool newArtists = false;
    bool newAlbums = false;
    bool newTracks = false;
    Track *track = 0;

    fileNames.append(audioFilesFromSubDirs(dir));
    for (int i = 0; i < fileNames.size(); ++i)
    {
        track = new Track(fileNames[i]);
        if (!hasTrack(track))
        {
            newTracks = true;
            _tracks.append(track);
        }
        else
        {
            delete track;
            continue;
        }

        if (!hasAlbum(track->album()))
        {
            newAlbums = true;
            AlbumData *data = new AlbumData;
            data->name = track->album();
            data->artist = track->artist();
            _albums.append(data);
        }
        if (!hasArtist(track->artist()))
        {
            newArtists = true;
            _artists.append(track->artist());
        }

        emit dirAddProgress(i+1, fileNames.size());
    }

    if (newArtists)
    {
        saveArtists();
        emit artistsUpdated();
    }
    if (newAlbums)
    {
        saveAlbums();
        emit albumsUpdated();
    }
    if (newTracks)
    {
        saveTracks();
        emit tracksUpdated();
    }
}

QStringList DataBase::audioFiles(const QDir &dir)
{
    QFreeDesktopMime mime;
    QString mimeType;
    QStringList files = dir.entryList(QDir::Files);
    for (int i = 0; i < files.size(); ++i)
    {
        mimeType = mime.fromFileName(files[i]);
        if (mimeType.split('/')[0] == "audio" && Phonon::BackendCapabilities::isMimeTypeAvailable(mimeType))
            files[i].prepend(dir.absolutePath() + "/");
        else files.removeAt(i--);
    }
    return files;
}

QStringList DataBase::audioFilesFromSubDirs(const QDir &dir)
{
    QStringList files = audioFiles(dir);
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < dirs.size(); ++i)
        files.append(audioFilesFromSubDirs(QDir(QString("%1/%2").arg(dir.absolutePath(), dirs[i]))));
    return files;
}

bool DataBase::hasAlbum(const QString &album) const
{
    for (int i = 0; i < _albums.size(); ++i)
        if (!QString::compare(_albums[i]->name, album, Qt::CaseInsensitive))
            return true;
    return false;
}

QStringList DataBase::albums(const QString &artist) const
{
    QStringList r;
    if (artist == ALL)
        for (int i = 0; i < _albums.size(); ++i)
            r.append(_albums[i]->name);
    else
        for (int i = 0; i < _albums.size(); ++i)
            if (!QString::compare(_albums[i]->artist, artist, Qt::CaseInsensitive))
                r.append(_albums[i]->name);
    return r;
}

QList<Track*> DataBase::tracks(const QString &artist, const QString &album) const
{
    QList<Track*> r;

    if (artist == ALL && album == ALL)
        return _tracks;

    if (artist == ALL)
    {
        for (int i = 0; i < _tracks.size(); ++i)
            if (!QString::compare(_tracks[i]->album(), album, Qt::CaseInsensitive))
                r.append(_tracks[i]);
    }
    else if (album == ALL)
    {
        for (int i = 0; i < _tracks.size(); ++i)
            if (!QString::compare(_tracks[i]->artist(), artist, Qt::CaseInsensitive))
                r.append(_tracks[i]);
    }
    else
    {
        for (int i = 0; i < _tracks.size(); ++i)
            if (!QString::compare(_tracks[i]->album(), album, Qt::CaseInsensitive)
                && !QString::compare(_tracks[i]->artist(), artist, Qt::CaseInsensitive))
                r.append(_tracks[i]);
    }

    return r;
}
