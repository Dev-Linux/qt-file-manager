#pragma once
#ifndef DIRMODEL_H
#define DIRMODEL_H

#include <QtWidgets>
#include "fileinfo.h"

class ViewSelectionModel;

class DirModel : public QObject
{
    Q_OBJECT
public:
    explicit DirModel(QObject *parent = 0);
    QDir dir;
    void setPath(const QString &path);
    QString path;
    void setNameFilters(const QStringList &nf);
    void clear();
    int count();
    bool isEmpty();
    bool drives;
    QList<FileInfo> drivesList;
    QList<FileInfo> list;
    QHash<int, QSet<QString>> tags;

    ViewSelectionModel *sel;
    QFileSystemWatcher *watcher;

    void addTag(int index, QString tag);
    void removeTag(int index, QString tag);

    bool selected(int index);
    QStringList selectedAbsolutePaths(
            bool nativeSeparators);

signals:
    void added(const FileInfo &info);
    void tagAdded(int index, const QString &tag);
    void tagRemoved(int index, const QString &tag);
    void cleared();
    void pathChanged(const QString &path);
    void nameFiltersChanged(const QStringList &nf);

    /**
     * @brief Emitted after the cleared() and added() signals.
     */
    void file_system_change();
public slots:

private:
    void loadEntries();
    
};

#endif // DIRMODEL_H
