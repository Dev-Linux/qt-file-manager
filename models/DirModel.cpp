#include "DirModel.h"

#include "models/ViewSelectionModel.h"

DirModel::DirModel(QObject *parent) :
    QObject(parent)
{
    foreach (const QFileInfo &info, QDir::drives()) {
        drives_list << FileInfo(info);
    }

    drives = false;
    watcher = new QFileSystemWatcher();
    dir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    sel = new ViewSelectionModel();
    connect(watcher, &QFileSystemWatcher::directoryChanged,
            [this] (const QString &path) {
        Q_UNUSED(path)
        //! @todo if { dir | parent dir } { deleted | renamed } then ...
        dir.refresh();
        load_entries();
        emit file_system_change();
    });
}

DirModel::~DirModel()
{
    delete sel;
}

void DirModel::set_path(const QString &path)
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

        load_entries();
    } else {
        drives = true;

        //dir.setPath(path);
        dir.setNameFilters(QStringList());

        watcher->removePaths(watcher->directories());

        load_entries();
    }

    emit path_changed(p);
}

//! @todo "accelerated" drag, just like the bug I had in the past.

void DirModel::set_name_filters(const QStringList &nf)
{
    if (!drives && dir.nameFilters() != nf) {
        dir.setNameFilters(nf);
        load_entries();
        emit name_filters_changed(nf);
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

bool DirModel::is_empty()
{
    return list.isEmpty();
}

void DirModel::add_tag(int index, QString tag)
{
    if (!tags[index].contains(tag)) {
        tags[index] << tag;
        emit tag_added(index, tag);
    }
}

void DirModel::remove_tag(int index, QString tag)
{
    if (tags[index].contains(tag)) {
        tags[index].remove(tag);
        emit tag_removed(index, tag);
    }
}

bool DirModel::selected(int index)
{
    return sel->contains(index);
}

QStringList DirModel::selected_abs_paths(bool nativeSeparators)
{
    auto set = sel->saved_set;
    QStringList paths;
    if (set.isEmpty()) return paths;
    QString path;
    if (nativeSeparators) {
        foreach (const int& index, set) {
            path = list[index].abs_file_path();
            paths << QDir::toNativeSeparators(path);
        }
    } else {
        foreach (const int& index, set) {
            path = list[index].abs_file_path();
            paths << path;
        }
    }
    return paths;
}

void DirModel::load_entries()
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
        list = drives_list;
        emit before_adding_n(list.size());
        foreach (const FileInfo &info, list) {
            emit added(info);
        }
    }
}
