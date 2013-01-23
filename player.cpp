#include "player.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include "database.h"
#include "track.h"
#include "progressdialog.h"

Player::Player(QWidget *parent):
    QMainWindow(parent),
    _ui(new Ui::Player),
    _player(new Phonon::MediaObject(this)),
    _audioOutput(new Phonon::AudioOutput(Phonon::MusicCategory, this)),
    _trayIcon(new QSystemTrayIcon(this)),
    _artistToPlaylistAction(new QAction(tr("Add to playlist"), this)),
    _artistsMenu(new QMenu(this)),
    _albumToPlaylistAction(new QAction(tr("Add to playlist"), this)),
    _albumsMenu(new QMenu(this)),
    _trackToPlaylistAction(new QAction(tr("Add to playlist"), this)),
    _tracksMenu(new QMenu(this)),
    _removeTrackAction(new QAction(tr("Remove track"), this)),
    _playlistMenu(new QMenu(this)),
    _playAction(new QAction(tr("Play"), this)),
    _pauseAction(new QAction(tr("Pause"), this)),
    _playNextAction(new QAction(tr("Play next"), this)),
    _playPrevAction(new QAction(tr("Play previous"), this)),
    _stopAction(new QAction(tr("Stop"), this)),
    _trayMenu(new QMenu(this))
{
    _ui->setupUi(this);
    this->setWindowIcon(QIcon(":/icon.png"));
    _ui->pauseButton->setHidden(true);
    fixHeader(_ui->tracks->horizontalHeader());
    fixHeader(_ui->playlist->horizontalHeader());
    _ui->tracks->verticalHeader()->setDefaultSectionSize(_ui->artists->sizeHintForRow(0) + 4);
    _ui->playlist->verticalHeader()->setDefaultSectionSize(_ui->artists->sizeHintForRow(0) + 4);

    _ui->previousButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    _ui->playButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
    _ui->pauseButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
    _ui->stopButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaStop));
    _ui->nextButton->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaSkipForward));

    _playPrevAction->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    _playAction->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPlay));
    _pauseAction->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaPause));
    _pauseAction->setVisible(false);
    _stopAction->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaStop));
    _playNextAction->setIcon(qApp->style()->standardIcon(QStyle::SP_MediaSkipForward));
    _ui->actionQuit->setIcon(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton));
    _trayMenu->addAction(_playAction);
    _trayMenu->addAction(_pauseAction);
    _trayMenu->addAction(_playPrevAction);
    _trayMenu->addAction(_playNextAction);
    _trayMenu->addAction(_stopAction);
    _trayMenu->addAction(_ui->actionQuit);

    _trayIcon->setContextMenu(_trayMenu);
    _trayIcon->setIcon(QIcon(":/icon.png"));
    _trayIcon->show();

    Phonon::createPath(_player, _audioOutput);
    _ui->seekSlider->setMediaObject(_player);
    _ui->volumeSlider->setAudioOutput(_audioOutput);

    _artistsMenu->addAction(_artistToPlaylistAction);
    _albumsMenu->addAction(_albumToPlaylistAction);
    _tracksMenu->addAction(_trackToPlaylistAction);
    _playlistMenu->addAction(_removeTrackAction);

    connect(_ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(_ui->actionAdd_directory, SIGNAL(triggered()), this, SLOT(addDirToLibrary()));
    connect(_ui->actionAdd_files, SIGNAL(triggered()), this, SLOT(addFilesToLibrary()));
    connect(_ui->actionClear_playlist, SIGNAL(triggered()), this, SLOT(clearPlaylist()));
    connect(_ui->actionLoad_playlist, SIGNAL(triggered()), this, SLOT(loadPlaylist()));
    connect(_ui->actionSave_playlist, SIGNAL(triggered()), this, SLOT(savePlaylist()));
    connect(_ui->nextButton, SIGNAL(clicked()), this, SLOT(playNext()));
    connect(_ui->playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(_ui->tracks, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(play()));
    connect(_ui->playlist, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(play()));
    connect(_ui->stopButton, SIGNAL(clicked()), this, SLOT(stop()));
    connect(_ui->pauseButton, SIGNAL(clicked()), _player, SLOT(pause()));
    connect(_ui->stopButton, SIGNAL(clicked()), _player, SLOT(stop()));
    connect(_ui->previousButton, SIGNAL(clicked()), this, SLOT(playPrevious()));
    connect(_playAction, SIGNAL(triggered()), this, SLOT(play()));
    connect(_pauseAction, SIGNAL(triggered()), _player, SLOT(pause()));
    connect(_stopAction, SIGNAL(triggered()), _player, SLOT(stop()));
    connect(_playNextAction, SIGNAL(triggered()), this, SLOT(playNext()));
    connect(_playPrevAction, SIGNAL(triggered()), this, SLOT(playPrevious()));
    connect(_ui->artists, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showArtistsContextMenu(const QPoint &)));
    connect(_ui->albums, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showAlbumsContextMenu(const QPoint &)));
    connect(_ui->tracks, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showTracksContextMenu(const QPoint &)));
    connect(_ui->playlist, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showPlaylistContextMenu(const QPoint &)));
    connect(DataBase::instance(), SIGNAL(albumsUpdated()), this, SLOT(updateAlbums()));
    connect(DataBase::instance(), SIGNAL(artistsUpdated()), this, SLOT(updateArtists()));
    connect(DataBase::instance(), SIGNAL(tracksUpdated()), this, SLOT(updateTracks()));
    connect(_ui->artists, SIGNAL(currentRowChanged(int)), this, SLOT(updateAlbums()));
    connect(_ui->albums, SIGNAL(currentRowChanged(int)), this, SLOT(updateTracks()));
    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(_player, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(_player, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
    connect(_player, SIGNAL(totalTimeChanged(qint64)), this, SLOT(totalTimeChanged(qint64)));
    connect(_player, SIGNAL(finished()), this, SLOT(trackFinished()));

    _allArtists = _ui->artists->item(0);
    _allArtists->setText(ALL);
    _allAlbums = _ui->albums->item(0);
    _allAlbums->setText(ALL);

    loadPlaylist(RECENTPLAYLIST);
}

Player::~Player()
{
    savePlaylist(RECENTPLAYLIST);
    delete _ui;
    delete _player;
    DataBase::destroy();
}

void Player::fixHeader(QHeaderView *header)
{
    // Track no.
    header->setResizeMode(0, QHeaderView::ResizeToContents);
    // Track name
    header->setResizeMode(1, QHeaderView::Stretch);
    // Artist name
    header->setResizeMode(2, QHeaderView::Stretch);
    // Album name
    header->setResizeMode(3, QHeaderView::Stretch);
    // Time
    header->setResizeMode(4, QHeaderView::ResizeToContents);

    header->hideSection(4);
}

void Player::addFilesToLibrary()
{
    Track *track;
    ProgressDialog progress(this);
    progress.show();
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files"));
    for (int i = 0; i < files.size(); ++i)
    {
        if (DataBase::isSupported(files[i]))
        {
            track = new Track(files[i]);
            DataBase::instance()->addTrack(track);
        }
        progress.setValue(i+1, files.size());
    }
}

void Player::addDirToLibrary()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select music directory"));
    if (dir.isEmpty())
        return;

    ProgressDialog progressDialog(this);
    connect(DataBase::instance(), SIGNAL(dirAddProgress(int,int)), &progressDialog, SLOT(setValue(int,int)));
    progressDialog.show();
    DataBase::instance()->addDirectory(dir);
    progressDialog.close();
}

void Player::updateArtists()
{
    _ui->artists->takeItem(_ui->artists->row(_allArtists));
    _ui->artists->clear();
    _ui->artists->addItem(_allArtists);
    _ui->artists->addItems(DataBase::instance()->artists());
    _ui->artists->setCurrentItem(_allArtists);
}

void Player::updateAlbums()
{
    QString artist;
    if (_ui->artists->currentItem())
        artist = _ui->artists->currentItem()->text();
    else artist = ALL;

    _ui->albums->takeItem(_ui->albums->row(_allAlbums));
    _ui->albums->clear();
    _ui->albums->addItem(_allAlbums);
    _ui->albums->addItems(DataBase::instance()->albums(artist));
    _ui->albums->setCurrentItem(_allAlbums);
}

void Player::updateTracks()
{
    QString artist;
    QString album;

    if (_ui->artists->currentItem())
        artist = _ui->artists->currentItem()->text();
    else artist = ALL;

    if (_ui->albums->currentItem())
        album = _ui->albums->currentItem()->text();
    else album = ALL;

    _ui->tracks->clearContents();
    QList<Track*> tracks = DataBase::instance()->tracks(artist, album);
    QTableWidgetItem *item = 0;
    _ui->tracks->setRowCount(tracks.size());
    for (int i = 0; i < tracks.size(); ++i)
    {
        item = new QTableWidgetItem(QString::number(tracks[i]->track()));
        _ui->tracks->setItem(i, 0, item);
        item = new QTableWidgetItem(tracks[i]->title());
        _ui->tracks->setItem(i, 1, item);
        item = new QTableWidgetItem(tracks[i]->artist());
        _ui->tracks->setItem(i, 2, item);
        item = new QTableWidgetItem(tracks[i]->album());
        _ui->tracks->setItem(i, 3, item);
    }
    _libTracks = tracks;
    _ui->tracks->setCurrentCell(0, 0);
}

void Player::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Context)
        return;

    if (this->isHidden())
        this->show();
    else this->hide();
}

void Player::play()
{
    if (_player->state() != Phonon::PausedState)
    {
        //_playingNextOrPrev = false;
        if (_ui->tabWidget->currentWidget() == _ui->libraryTab)
        {
            Phonon::MediaSource source(_libTracks[_ui->tracks->currentRow()]->fileName());
            _player->setCurrentSource(source);
        }
        else
        {
            if (_plistTracks.isEmpty() || _ui->playlist->currentRow() == -1)
                return;
            Phonon::MediaSource source(_plistTracks[_ui->playlist->currentRow()]->fileName());
            _player->setCurrentSource(source);
        }
    }
    _player->play();
    _isStopped = false;
}

void Player::stateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    if (newState == Phonon::StoppedState || newState == Phonon::PausedState)
    {
        _ui->playButton->show();
        _ui->pauseButton->hide();
        _playAction->setVisible(true);
        _pauseAction->setVisible(false);
    }
    else if (newState == Phonon::PlayingState)
    {
        Track *track = 0;
        if (_ui->tabWidget->currentWidget() == _ui->libraryTab)
            track = _libTracks[_ui->tracks->currentRow()];
        else track = _plistTracks[_ui->playlist->currentRow()];
        _ui->trackLabel->setText(track->title());
        _ui->artistLabel->setText(track->artist());
        _ui->playButton->hide();
        _ui->pauseButton->show();
        _playAction->setVisible(false);
        _pauseAction->setVisible(true);
        _trayIcon->setToolTip(QString("%1 - %2").arg(track->artist(), track->title()));
        //this->adjustSize();
    }
    else if (newState == Phonon::ErrorState)
    {
        _ui->playButton->show();
        _ui->pauseButton->hide();
        _playAction->setVisible(true);
        _pauseAction->setVisible(false);
        QMessageBox::warning(this, tr("Error!"), _player->errorString());
    }
}

void Player::totalTimeChanged(qint64 newTotalTime)
{
    QDateTime time = QDateTime::fromMSecsSinceEpoch(newTotalTime);
    _ui->totalTimeLabel->setText(time.toString("m:ss"));
}

void Player::tick(qint64 time)
{
    QDateTime t = QDateTime::fromMSecsSinceEpoch(time);
    _ui->currTimeLabel->setText(t.toString("m:ss"));
}

void Player::playPrevious()
{
    stop();
    int r;
    QTableWidget *widget = 0;
//    QList<Track*> *playlist = 0;
//    Track *track = 0;

    if (_ui->tabWidget->currentWidget() == _ui->libraryTab)
    {
        widget = _ui->tracks;
//        playlist = &_libTracks;
    }
    else
    {
        widget = _ui->playlist;
//        playlist = &_plistTracks;
    }

    r = widget->currentRow();
    if (r <= 0)
        return;

    widget->setCurrentCell(--r, 0);
    play();
}

void Player::playNext()
{
    stop();

    int r;
    QTableWidget *widget = 0;
    QList<Track*> *playlist = 0;
//    Track *track = 0;

    if (_ui->tabWidget->currentWidget() == _ui->libraryTab)
    {
        widget = _ui->tracks;
        playlist = &_libTracks;
    }
    else
    {
        widget = _ui->playlist;
        playlist = &_plistTracks;
    }

    r = widget->currentRow() + 1;
    if (r >= playlist->size())
        return;

    widget->setCurrentCell(r, 0);
    play();
}

void Player::stop()
{
    _isStopped = true;
    _player->stop();
}

void Player::showArtistsContextMenu(const QPoint &point)
{
    if (_ui->artists->itemAt(point)
        && _artistsMenu->exec(_ui->artists->mapToGlobal(point)) == _artistToPlaylistAction)
    {
        addToPlaylist(DataBase::instance()->tracks(_ui->artists->itemAt(point)->text(), ALL));
    }
}

void Player::showAlbumsContextMenu(const QPoint &point)
{
    if (_ui->albums->itemAt(point)
        && _albumsMenu->exec(_ui->albums->mapToGlobal(point)) == _albumToPlaylistAction)
    {
        addToPlaylist(DataBase::instance()->tracks(_ui->artists->currentItem()->text(),
                                                   _ui->albums->itemAt(point)->text()));
    }
}

void Player::showTracksContextMenu(const QPoint &point)
{
    if (_ui->tracks->currentRow() >= 0
        && _tracksMenu->exec(_ui->tracks->mapToGlobal(point)) == _trackToPlaylistAction)
    {
        addToPlaylist(_libTracks[_ui->tracks->currentRow()]);
    }
}

void Player::addToPlaylist(const Track *track)
{
    QTableWidgetItem *item = 0;
    int row = _ui->playlist->rowCount();
    _plistTracks.append(const_cast<Track*>(track));
    _ui->playlist->insertRow(row);
    item = new QTableWidgetItem(QString::number(track->track()));
    _ui->playlist->setItem(row, 0, item);
    item = new QTableWidgetItem(track->title());
    _ui->playlist->setItem(row, 1, item);
    item = new QTableWidgetItem(track->artist());
    _ui->playlist->setItem(row, 2, item);
    item = new QTableWidgetItem(track->album());
    _ui->playlist->setItem(row, 3, item);
}

void Player::addToPlaylist(const QList<Track *> &tracks)
{
    for (int i = 0; i < tracks.size(); ++i)
        addToPlaylist(tracks[i]);
}

void Player::clearPlaylist()
{
    for (int i = 0; i < _plistTracks.size(); ++i)
    {
        if (!DataBase::instance()->hasTrack(_plistTracks[i]))
            delete _plistTracks[i];
    }
    _plistTracks.clear();
    _ui->playlist->clearContents();
    _ui->playlist->setRowCount(0);
}

void Player::trackFinished()
{
    if (!_isStopped)
        playNext();
}

void Player::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Select file"), QDir::homePath(), "*.plst");
    if (fileName.isEmpty())
        return;
    if (!fileName.endsWith(".plst"))
        fileName.append(".plst");
    if (!savePlaylist(fileName))
        QMessageBox::critical(this, tr("Error!"), tr("Failed to save playlist!"));
}

bool Player::savePlaylist(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;

    for (int i = 0; i < _plistTracks.size(); ++i)
        file.write(_plistTracks[i]->fileName().toUtf8() + '\n');
    file.close();
    return true;
}

void Player::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), QDir::homePath(), "*.plst");
    if (fileName.isEmpty())
        return;
    clearPlaylist();
    if (!loadPlaylist(fileName))
        QMessageBox::critical(this, tr("Error!"), tr("Failed to load playlist!"));
}

bool Player::loadPlaylist(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return false;

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    Track *track = 0;
    while (!stream.atEnd())
    {
        QString trackFileName = stream.readLine();
        if (!trackFileName.isEmpty())
        {
            track = DataBase::instance()->track(trackFileName);
            if (!track)
                track = new Track(trackFileName);
            addToPlaylist(track);
        }
    }
    file.close();
    return true;
}

void Player::showPlaylistContextMenu(const QPoint &point)
{
    if (_ui->playlist->currentRow() >= 0
        && _playlistMenu->exec(_ui->playlist->mapToGlobal(point)) == _removeTrackAction)
    {
        int row = _ui->playlist->currentRow();
        _ui->playlist->removeRow(row);
        _plistTracks.removeAt(row);
    }
}
