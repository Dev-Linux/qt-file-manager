#include "AsyncFileOperation.h"

#include "controllers/MainWindowController.h"

#include "views/MainWindow.h"
#include "views/FileOperationItem.h"

#include "models/FileOperationData.h"

#include "misc.h"
#include "AsyncFileOperationWorker.h"

#ifdef Q_OS_WIN32
#include <objbase.h>
#endif

AsyncFileOperation::AsyncFileOperation(QObject *parent) :
    QThread(parent), m_thread_is_ready(false)
{
    start();
    while (!m_thread_is_ready) {
        msleep(50);
    }
}

AsyncFileOperation::~AsyncFileOperation()
{
#ifdef Q_OS_WIN32
    CoUninitialize();
#endif
}

void AsyncFileOperation::do_async(FileOperationData *data,
                                 FileOperationItem *item)
{
    MainWindowController::instance()->file_ops->insert(data, item);
    emit operation_requested(data);
}

void AsyncFileOperation::run()
{
#ifdef Q_OS_WIN32
    HRESULT hr = CoInitializeEx(NULL,
                                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr)) {
        qDebug() << COMError(hr);
        QMessageBox::warning(0, "Error", "Cannot initialize COM.");
        return;
    }
#endif

    AsyncFileOperationWorker worker;

    connect(this, &AsyncFileOperation::operation_requested,
            &worker, &AsyncFileOperationWorker::operate);
    connect(&worker, &AsyncFileOperationWorker::progress,
            this, &AsyncFileOperation::progress);
    connect(&worker, &AsyncFileOperationWorker::done,
            this, &AsyncFileOperation::done);

    m_thread_is_ready = true;
    exec();
}
