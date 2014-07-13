#include "FileInfo.h"

#include "views/MainWindow.h"

#include "controllers/MainWindowController.h"
#include "controllers/DirController.h"

#include "models/DirModel.h"

/**
 * @class FileInfo
 * @note Random info: [`#%ifdef Q_COMPILER_RVALUE_REFS`] both Clang and MSVC
 * support rvalue references.
 */

QFileIconProvider FileInfo::m_file_icon_provider;
QHash<const QString, QIcon> FileInfo::m_icon_cache;

FileInfo::FileInfo()
{
    m_is_loaded = false;
}
FileInfo::FileInfo(const QString &path) : FileInfo()
{
    m_ctor_path = path;
}
FileInfo::FileInfo(const QFileInfo &info) : FileInfo()
{
    file_info = info;
}

QIcon FileInfo::icon() const
{
    auto dr = MainWindowController::instance()->is_showing_drives();
    if (dr) {
        if (!m_icon_cache.contains(abs_file_path())) {
            // this thing makes a long and powerful noise,
            // for the floppy disk and the icons are low-quality
            m_icon_cache[abs_file_path()] = m_file_icon_provider.icon(file_info);
        }
        return m_icon_cache[abs_file_path()];
    }
    return m_file_icon_provider.icon(file_info);
}

bool FileInfo::is_drive() const
{
    init();
    return file_info.fileName().isEmpty();
}

const QString &FileInfo::file_name() const
{
    //! @todo other options in the context menu
    //! @todo re-do d&d

    init();
    return m_file_name;
}

const QString &FileInfo::file_path() const
{
    init();
    return m_file_path;
}

const QString &FileInfo::abs_file_path() const
{
    init();
    return m_abs_file_path;
}

bool FileInfo::is_dir() const
{
    init();
    return file_info.isDir();
}

QDir FileInfo::abs_dir() const
{
    init();
    return file_info.absoluteDir();
}

bool FileInfo::operator ==(const FileInfo &info) const
{
    init();
    info.init();
    //qDebug() << "==(): " << (this->fileInfo == info.fileInfo);
    return this->file_info == info.file_info;
}

void FileInfo::init() const
{
    if (!m_is_loaded) {
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
            file_info.setFile(m_ctor_path);
        }

        m_abs_file_path = file_info.absoluteFilePath();
        m_file_path = file_info.filePath();

        //! @todo this should include the drive "label" as in File Explorer
        m_file_name = file_info.fileName();
        if (m_file_name.isEmpty()) { // is drive..
            m_file_name = m_abs_file_path;
        }

        m_is_loaded = true;
    }
}

/**
 * @brief Allows qDebugging of FileInfo instances (prints their
 * absolute file path).
 *
 * @param dbg The QDebug instance (returned by calls to qDebug()).
 *
 * @param info The FileInfo to output.
 *
 * @return The same QDebug instance @a dbg, for chained calls.
 */
QDebug operator<<(QDebug dbg, const FileInfo &info)
{
    dbg.nospace() << "FileInfo("
                  << info.abs_file_path() << ")";
    return dbg.space();
}
