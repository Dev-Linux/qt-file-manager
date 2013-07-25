#include "DirModel.h"
#include "ViewSelectionModel.h"

DirModel::DirModel(QObject *parent) :
    QObject(parent)
{
    foreach (const QFileInfo &info, QDir::drives()) {
        drivesList << FileInfo(info);
    }

    drives = false;
    watcher = new QFileSystemWatcher();
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    sel = new ViewSelectionModel(this);
    connect(watcher, &QFileSystemWatcher::directoryChanged,
            [this] (const QString &path) {
        Q_UNUSED(path)
        //! @todo if { dir | parent dir } { deleted | renamed } then ...
        dir.refresh();
        loadEntries();
        emit file_system_change();
    });
}

void DirModel::setPath(const QString &path)
{
    this->path = path;
    QString &p = this->path;

    tags.clear();
    if (!p.isEmpty()) {
        drives = false;

        dir.setPath(p);
        dir.setNameFilters(QStringList());

        watcher->removePaths(watcher->directories());
        watcher->addPath(p);

        loadEntries();
    } else {
        drives = true;

        //dir.setPath(path);
        dir.setNameFilters(QStringList());

        watcher->removePaths(watcher->directories());

        loadEntries();
    }

    emit pathChanged(p);
}

//! @todo "accelerated" drag, just like the bug I had in the past.

void DirModel::setNameFilters(const QStringList &nf)
{
    if (!drives && dir.nameFilters() != nf) {
        dir.setNameFilters(nf);
        loadEntries();
        emit nameFiltersChanged(nf);
    }
}

void DirModel::clear()
{
    sel->clear();
    sel->save();

    list.clear();

    emit cleared();
}

int DirModel::count()
{
    return list.count();
}

bool DirModel::isEmpty()
{
    return list.isEmpty();
}

void DirModel::addTag(int index, QString tag)
{
    if (!tags[index].contains(tag)) {
        tags[index] << tag;
        emit tagAdded(index, tag);
    }
}

void DirModel::removeTag(int index, QString tag)
{
    if (tags[index].contains(tag)) {
        tags[index].remove(tag);
        emit tagRemoved(index, tag);
    }
}

bool DirModel::selected(int index)
{
    return sel->contains(index);
}

QStringList DirModel::selectedAbsolutePaths(bool nativeSeparators)
{
    auto set = sel->savedSet;
    QStringList paths;
    if (set.isEmpty()) return paths;
    QString path;
    if (nativeSeparators) {
        foreach (const int& index, set) {
            path = list[index].absoluteFilePath();
            paths << QDir::toNativeSeparators(path);
        }
    } else {
        foreach (const int& index, set) {
            path = list[index].absoluteFilePath();
            paths << path;
        }
    }
    return paths;
}

void DirModel::loadEntries()
{
    clear();

    QFileInfoList l;
    if (!drives) {
        l = dir.entryInfoList();
        foreach (const QFileInfo &info, l) {
            list << FileInfo(info);
        }
        emit before_adding_n(list.size());
        foreach (const FileInfo &info, list) {
            emit added(info);
        }
    } else {
        list = drivesList;
        emit before_adding_n(list.size());
        foreach (const FileInfo &info, list) {
            emit added(info);
        }
    }
}
