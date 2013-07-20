#include "asyncfileoperation.h"

#include "misc.h"
#include "mainwindow.h"
#include "asyncfileoperationworker.h"
#include "fileoperationitem.h"
#include "fileoperationdata.h"

#ifdef Q_OS_WIN32
#include <objbase.h>
#endif

AsyncFileOperation::AsyncFileOperation(QObject *parent) :
    QThread(parent), threadIsReady(false)
{
    start();
    while (!threadIsReady) {
        msleep(50);
    }
}

AsyncFileOperation::~AsyncFileOperation()
{
#ifdef Q_OS_WIN32
    CoUninitialize();
#endif
}

void AsyncFileOperation::doAsync(FileOperationData *data,
                                 FileOperationItem *item)
{
    MainWindow::getInstance()->fileOperations->insert(data, item);
    emit operationRequested(data);
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

    connect(this, &AsyncFileOperation::operationRequested,
            &worker, &AsyncFileOperationWorker::operate);
    connect(&worker, &AsyncFileOperationWorker::progress,
            this, &AsyncFileOperation::progress);
    connect(&worker, &AsyncFileOperationWorker::done,
            this, &AsyncFileOperation::done);

    threadIsReady = true;
    exec();
}
