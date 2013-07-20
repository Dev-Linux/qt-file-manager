#include "TrashModel.h"

#include "fileinfo.h"
#include "fileoperationdata.h"

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
    this->filesDir = new QDir(trashPath + "/files");
    this->infoDir = new QDir(trashPath + "/info");

    // a lot of unimplemented things
}

FileOperationData* TrashModel::moveToTrash(const QString &path)
{
    FileInfo info(path);
    info.init();

    if (info.isDrive()) {
        qDebug() << "Can't trash/recycle drive:" << path;
        return nullptr;
    }

    if (!info.fileInfo.isWritable()) { // if (can't write) then (can't delete)
        // should I also check for execute (explore) permission for folders?
        // should I check for permissions recursively?
        // TODO: read more about these things
        qDebug() << "Can't trash/recycle this file/folder:" << path
                 << ". No write permission on this file/folder.";
        return nullptr;
    }

    auto fn = info.fileName();
    QString f;
    if (!filesDir->exists(fn)) {
        f = fn;
    } else {
        bool ok;
        int i = 1;
        do {
            i++;
            f = fn + "." + QString::number(i);
            ok = !filesDir->exists(f);
        } while (!ok);
    }
    Q_ASSERT(!filesDir->exists(f));

    auto fi = f + ".trashinfo";
    auto fic = QString("[Trash Info]\n"
                        "Path=%1\n"
                        "DeletionDate=%2")
            // RFC 3339: https://www.ietf.org/rfc/rfc3339.txt
            // YYYY-MM-DDThh:mm:ss
            // encode url: http://www.faqs.org/rfcs/rfc2396.html
        .arg(QUrl(info.absoluteFilePath()).toString(QUrl::FullyEncoded),
             QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss"));

    // not atomic :
    QFile inf(infoDir->absoluteFilePath(fi));
    if (inf.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&inf);
        out << fic;
        inf.close();
    } else {
        qDebug() << "Error writing '" << infoDir->absoluteFilePath(fi)
                 << "':" << inf.errorString();
        return nullptr;
    }

    return new FileOperationData("move",
                                 QStringList() << info.absoluteFilePath(),
                                 filesDir->absolutePath(),
                                 QStringList() << f);
}
