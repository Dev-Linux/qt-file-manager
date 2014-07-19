#pragma once
#ifndef FILEINFO_H
#define FILEINFO_H

#include <QIcon>
#include <QFileIconProvider>
#include <QDebug>

class FileInfo
{
public:
    FileInfo();
    FileInfo(const QString &path);
    FileInfo(const QFileInfo &file_info);

    QIcon icon() const;
    bool is_drive() const;
    void init() const;

    // from QFileInfo (some un-const-ed)
    const QString &file_name() const;
    const QString &file_path() const;
    const QString &abs_file_path() const;
    bool is_dir() const;
    QDir abs_dir() const;
    bool operator ==(const FileInfo &info) const;

    mutable QFileInfo file_info;

private:
    static QFileIconProvider m_file_icon_provider;
    static QHash<const QString, QIcon> m_icon_cache;

    mutable QString m_ctor_path;
    mutable QString m_abs_file_path;
    mutable QString m_file_name;
    mutable QString m_file_path;

    mutable bool m_is_loaded;
};

QDebug operator<<(QDebug dbg, const FileInfo &info);

#endif // FILEINFO_H
