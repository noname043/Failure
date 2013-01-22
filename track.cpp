#include "track.h"
#include <QObject>
#include <QFileInfo>

Track::Track(const QString &fileName):
    _track(0),
    _duration(0)
{
    setFileName(fileName, true);
}

void Track::setFileName(const QString &fileName, bool loadData)
{
    _fileName = fileName;
    if (fileName.isEmpty() || !loadData)
        return;

    TagLib::FileRef fileRef(fileName.toUtf8().data());
    TagLib::Tag *tags = fileRef.tag();
    if (!tags)
    {
        _title = QFileInfo(fileName).fileName();
        _artist = QObject::tr("Unknown");
        _album = QObject::tr("Unknown");
    }
    else
    {
        _title = QString::fromUtf8(tags->title().toCString(true));
        if (_title.isEmpty())
            _title = QObject::tr("Unknown");
        _artist = QString::fromUtf8(tags->artist().toCString(true));
        if (_artist.isEmpty())
            _artist = QObject::tr("Unknown");
        _album = QString::fromUtf8(tags->album().toCString(true));
        if (_album.isEmpty())
            _album = QObject::tr("Unknown");
        _track = tags->track();
    }
}
