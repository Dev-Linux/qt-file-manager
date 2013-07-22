#include "fileinfo.h"

#include "mainwindow.h"
#include "MainWindowController.h"
#include "view/DirController.h"
#include "view/DirModel.h"

QFileIconProvider FileInfo::fileIconProvider;
QHash<const QString, QIcon> FileInfo::iconCache;

FileInfo::FileInfo()
{
    isLoaded = false;
}
FileInfo::FileInfo(const QString &path) : FileInfo()
{
    m_ctor_path = path;
}
FileInfo::FileInfo(const QFileInfo &info) : FileInfo()
{
    fileInfo = info;
}

QIcon FileInfo::icon() const
{
    auto dr = MainWindowController::instance()->view->
            dirCtrl->model->drives;
    if (dr) {
        if (!iconCache.contains(absoluteFilePath())) {
            // this thing makes a long and powerful noise,
            // for the floppy disk and the icons are low-quality
            iconCache[absoluteFilePath()] = fileIconProvider.icon(fileInfo);
        }
        return iconCache[absoluteFilePath()];
    }
    return fileIconProvider.icon(fileInfo);
}

bool FileInfo::isDrive() const
{
    init();
    return fileInfo.fileName().isEmpty();
}

const QString &FileInfo::fileName() const
{
    //! @todo other options in the context menu
    //! @todo re-do d&d

    init();
    return m_fileName;
}

const QString &FileInfo::filePath() const
{
    init();
    return m_filePath;
}

const QString &FileInfo::absoluteFilePath() const
{
    init();
    return m_absoluteFilePath;
}

bool FileInfo::isDir() const
{
    init();
    return fileInfo.isDir();
}

QDir FileInfo::absoluteDir() const
{
    init();
    return fileInfo.absoluteDir();
}

bool FileInfo::operator ==(const FileInfo &info) const
{
    init();
    info.init();
    //qDebug() << "==(): " << (this->fileInfo == info.fileInfo);
    return this->fileInfo == info.fileInfo;
}

void FileInfo::init() const
{
    if (!isLoaded) {
#ifdef Q_OS_LINUX
        // only dirs can have a trailing / in path
        const int size = m_ctor_path.size();
        if (m_ctor_path.endsWith("/") && size > 1) {
            m_ctor_path.remove(size - 1, 1);
        }
#else // Q_OS_WIN32
        //! @todo ...
        Q_ASSERT_X(false, "FileInfo::init", "not impl. for Windows");
#endif
        if (!m_ctor_path.isEmpty()) {
            fileInfo.setFile(m_ctor_path);
        }

        m_absoluteFilePath = fileInfo.absoluteFilePath();
        m_filePath = fileInfo.filePath();

        //! @todo this should include the drive "label" as in File Explorer
        m_fileName = fileInfo.fileName();
        if (m_fileName.isEmpty()) { // is drive..
            m_fileName = m_absoluteFilePath;
        }

        isLoaded = true;
    }
}

