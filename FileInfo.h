#pragma once
#ifndef FILEINFO_H
#define FILEINFO_H

#include <QIcon>
#include <QFileIconProvider>
#include <QDebug>

class FileInfo
{
public:
    mutable QFileInfo fileInfo;

    explicit FileInfo();
    FileInfo(const QString &path);
    FileInfo(const QFileInfo &fileInfo);

    QIcon icon() const;
    bool isDrive() const;
    void init() const;

    // from QFileInfo (some un-const-ed)
    const QString &fileName() const;
    const QString &filePath() const;
    const QString &absoluteFilePath() const;
    bool isDir() const;
    QDir absoluteDir() const;
    bool operator ==(const FileInfo &info) const;

private:
    static QFileIconProvider fileIconProvider;
    static QHash<const QString, QIcon> iconCache;

    mutable QString m_ctor_path;
    mutable QString m_absoluteFilePath;
    mutable QString m_fileName;
    mutable QString m_filePath;

    mutable bool isLoaded;
};

QDebug operator<<(QDebug dbg, const FileInfo &info);

#endif // FILEINFO_H
