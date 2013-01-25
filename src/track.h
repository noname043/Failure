#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <taglib/tag.h>
#include <taglib/fileref.h>

class Track
{
public:
    Track(const QString &fileName = QString());

    int track() const { return _track; }
    void setTrack(int track) { _track = track; }

    QString title() const { return _title; }
    void setTitle(const QString &newTitle) { _title = newTitle; }

    QString artist() const { return _artist; }
    void setArtist(const QString &newArtist) { _artist = newArtist; }

    QString album() const { return _album; }
    void setAlbum(const QString &newAlbum) { _album = newAlbum; }

    int duration() const { return _duration; }
    void setDuration(int duration) { _duration = duration; }

    QString fileName() const { return _fileName; }
    void setFileName(const QString &fileName, bool loadData = false);

private:
    int _track;
    QString _title;
    QString _artist;
    QString _album;
    int _duration;
    QString _fileName;
};

#endif // TRACK_H
