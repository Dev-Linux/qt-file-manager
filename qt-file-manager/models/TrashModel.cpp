#include "TrashModel.h"

#include "FileInfo.h"
#include "FileOperationData.h"

TrashModel::TrashModel() :
    QObject()
{ // http://www.ramendik.ru/docs/trashspec.html
    // http://standards.freedesktop.org/desktop-entry-spec/latest
    env = QProcessEnvironment::systemEnvironment();
    auto dataHome = env.value("XDG_DATA_HOME");
    if (dataHome.isEmpty()) {
        dataHome = env.value("HOME") + "/.local/share";
    }
    auto trashPath = dataHome + "/Trash";
    auto trashDir = new QDir(trashPath);
    if (!trashDir->exists()) {
        trashDir->mkpath(trashPath);
    }
    this->files_dir = new QDir(trashPath + "/files");
    this->info_dir = new QDir(trashPath + "/info");

    // a lot of unimplemented things
}

FileOperationData* TrashModel::move_to_trash(const QString &path)
{
    FileInfo info(path);
    info.init();

    if (info.is_drive()) {
        qDebug() << "Can't trash/recycle drive:" << path;
        return nullptr;
    }

    if (!info.file_info.isWritable()) { // if (can't write) then (can't delete)
        /**
         * @todo read more about these things:
         * - should I also check for execute (explore) permission for folders?
         * - should I check for permissions recursively?
         */
        qDebug() << "Can't trash/recycle this file/folder:" << path
                 << ". No write permission on this file/folder.";
        return nullptr;
    }

    auto fn = info.file_name();
    QString f;
    if (!files_dir->exists(fn)) {
        f = fn;
    } else {
        bool ok;
        int i = 1;
        do {
            i++;
            f = fn + "." + QString::number(i);
            ok = !files_dir->exists(f);
        } while (!ok);
    }
    Q_ASSERT(!files_dir->exists(f));

    auto fi = f + ".trashinfo";
    auto fic = QString("[Trash Info]\n"
                        "Path=%1\n"
                        "DeletionDate=%2")
            // RFC 3339: https://www.ietf.org/rfc/rfc3339.txt
            // YYYY-MM-DDThh:mm:ss
            // encode url: http://www.faqs.org/rfcs/rfc2396.html
        .arg(QUrl(info.abs_file_path()).toString(QUrl::FullyEncoded),
             QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));

    // not atomic :
    QFile inf(info_dir->absoluteFilePath(fi));
    if (inf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&inf);
        out << fic;
        inf.close();
    } else {
        qDebug() << "Error writing '" << info_dir->absoluteFilePath(fi)
                 << "':" << inf.errorString();
        return nullptr;
    }

    return new FileOperationData("move",
                                 QStringList() << info.abs_file_path(),
                                 files_dir->absolutePath(),
                                 QStringList() << f);
}
