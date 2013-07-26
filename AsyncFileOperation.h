#pragma once
#ifndef ASYNCFILEOPERATION_H
#define ASYNCFILEOPERATION_H

#include <QtCore>

class FileOperationData;
class FileOperationItem;

class AsyncFileOperation : public QThread
{
    Q_OBJECT

public:
    explicit AsyncFileOperation(QObject *parent = 0);
    ~AsyncFileOperation();

    void do_async(FileOperationData* data,
                  FileOperationItem* item);

public slots:

signals:
    void operation_requested(FileOperationData* data);
    void done(FileOperationData *data);
    void progress(FileOperationData *data);

protected:
    void run();

private:
    bool m_thread_is_ready;
};

#endif // ASYNCFILEOPERATION_H
