#pragma once
#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <QtWidgets>

#include "FileInfo.h"

class ViewSelectionModel;

class DirModel : public QObject
{
    Q_OBJECT
public:
    explicit DirModel(QObject *parent = 0);
    ~DirModel();
    void set_path(const QString &path);
    void set_name_filters(const QStringList &nf);
    void clear();
    int count();
    bool is_empty();

    void add_tag(int index, QString tag);
    void remove_tag(int index, QString tag);

    bool selected(int index);
    QStringList selected_abs_paths(bool nativeSeparators);

    QDir dir;
    bool drives;
    QString path;
    QList<FileInfo> drives_list;
    QList<FileInfo> list;
    QHash<int, QSet<QString>> tags;
    ViewSelectionModel *sel;
    QFileSystemWatcher *watcher;

signals:
    void added(const FileInfo &info);
    void tag_added(int index, const QString &tag);
    void tag_removed(int index, const QString &tag);
    void cleared();
    void path_changed(const QString &path);
    void name_filters_changed(const QStringList &nf);

    /**
     * @brief Emitted before n continuous emissions of the added()
     * signal.
     * @param n The number of emissions of added() that will follow.
     */
    void before_adding_n(int n);

    /**
     * @brief Emitted after the cleared() and added() signals.
     */
    void file_system_change();

public slots:

private:
    void load_entries();
};

#endif // DIRMODEL_H
