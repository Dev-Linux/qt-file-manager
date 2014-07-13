#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QtWidgets>
#include <QModelIndex>

#include "asyncfileoperation.h"

class MainWindow;

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FileSystemModel(QObject *parent = 0);
    ~FileSystemModel();

    /*Q_INVOKABLE QModelIndex qmlIndex(int row, int column, const QModelIndex & parent = QModelIndex());
    Q_INVOKABLE QModelIndex qmlIndex(const QString & path, int column = 0);
    Q_INVOKABLE int qmlRowCount(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE QString qmlFileName(const QModelIndex &index) const;

    Q_INVOKABLE QModelIndex setRootPath(const QString &path);*/
    
signals:

public slots:

private:
    AsyncFileOperation *op;

protected:
    virtual Qt::DropActions supportedDropActions() const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent);
};

#endif // FILESYSTEMMODEL_H
