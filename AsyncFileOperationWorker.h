#pragma once
#ifndef ASYNCFILEOPERATIONWORKER_H
#define ASYNCFILEOPERATIONWORKER_H

#include <QtCore>

class TrashModel;
class FileOperationData;

//! @todo pinned files/folders in every path (even in root)

class AsyncFileOperationWorker : public QObject
{
    Q_OBJECT

public:
    explicit AsyncFileOperationWorker(QObject *parent = 0);
    TrashModel *trash_model;
    
public slots:
    void operate(FileOperationData *data);

signals:
    void done(FileOperationData* data);
    void progress(FileOperationData* data);

private:
    QPair<qint64, qint64> scan(const QStringList& src);
    void copy(const QStringList& src,
              const QString& dest,
              FileOperationData* data);
    void recursive_delete(const QStringList& src,
                         FileOperationData* data);
    bool copy_file(const QString& fileName,
                  const QString& newName,
                  FileOperationData* data);
};

#endif // ASYNCFILEOPERATIONWORKER_H
