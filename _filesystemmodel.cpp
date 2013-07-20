#include "filesystemmodel.h"
#include "mainwindow.h"

FileSystemModel::FileSystemModel(QObject *parent) :
    QFileSystemModel(parent)
{
    op = MainWindow::getInstance()->asyncFileOperation;
}

FileSystemModel::~FileSystemModel()
{
}

/*QModelIndex FileSystemModel::qmlIndex(int row, int column, const QModelIndex &parent)
{
    return index(row, column, parent);
}

QModelIndex FileSystemModel::qmlIndex(const QString &path, int column)
{
    return index(path, column);
}

int FileSystemModel::qmlRowCount(const QModelIndex &parent) const
{
    return rowCount(parent);
}

QString FileSystemModel::qmlFileName(const QModelIndex &index) const
{
    return fileName(index);
}

QModelIndex FileSystemModel::setRootPath(const QString &path)
{
    qDebug() << path;
    QModelIndex mi = QFileSystemModel::setRootPath(path);
    if (path.isEmpty()) {
        emit directoryLoaded(path);
    }
    return mi;
}*/

Qt::DropActions FileSystemModel::supportedDropActions() const
{
    Qt::DropActions da = QFileSystemModel::supportedDropActions();
    // implicit: copy, move, link
    return da;
}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QFileSystemModel::flags(index);
    return defaultFlags | Qt::ItemIsDropEnabled;
}

bool FileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                   int /*row*/, int /*column*/, const QModelIndex &parent)
{
    QString path;
    if (!parent.isValid()) {
        path = rootPath();
        if (path.isEmpty()) return false;
    } else {
        if (hasChildren(parent)) {
            path = this->filePath(parent);
        } else {
            path = rootPath();
            if (path.isEmpty()) return false;
        }
    }
    path = QDir::toNativeSeparators(path);
    QList<QUrl> urls = data->urls();

    QStringList paths;
    foreach(const QUrl& url, urls) {
        paths << QDir::toNativeSeparators(url.toLocalFile());
    }

    FileOperationData *opData;
    FileOperationItem *item;
    if (action == Qt::CopyAction) {
        opData = new FileOperationData("copy", paths, path);
        item = MainWindow::getInstance()->fileOperationsMenu->addItem(new FileOperationItem(opData,
            "<strong>" + paths.join(", ") + "</strong> to <strong>" + path + "</strong>."));
        op->doAsync(opData, item);
        return true;
    }
    if (action == Qt::MoveAction) {
        opData = new FileOperationData("move", paths, path);
        item = MainWindow::getInstance()->fileOperationsMenu->addItem(new FileOperationItem(opData,
            "<strong>" + paths.join(", ") + "</strong> to <strong>" + path + "</strong>."));
        op->doAsync(opData, item);
        return true;
    }
    if (action == Qt::LinkAction) {
        // TODO: implement Qt::LinkAction
        qDebug() << "Error: unhandled action \"link\" in FileSystemModel::dropMimeData.";
        return false;
    }

    qDebug() << "Unknown action.";
    return false;
}
