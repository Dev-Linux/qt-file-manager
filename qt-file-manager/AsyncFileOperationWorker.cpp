#include "AsyncFileOperationWorker.h"

#include "models/TrashModel.h"
#include "models/FileOperationData.h"

#include "misc.h"

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <shellapi.h>
#include <ShObjIdl.h>
#include <ShlObj.h>
#endif

AsyncFileOperationWorker::AsyncFileOperationWorker(QObject *parent) :
    QObject(parent)
{
}

void AsyncFileOperationWorker::operate(FileOperationData *data)
{
    /**
     * @note it would probably be better to work with FileInfo's pointers,
     * somehow, don't know ... Advantage: I wouldn't have to work with path
     * differences as much as I do now
     */
    QString type = data->type();
    if (type == "copy") {
        QPair<qint64, qint64> size = scan(data->source_list());
        data->work_total = size.first;
        data->bytes_total = size.second;

        copy(data->source_list(), data->destination(), data);
    } else if (type == "move") {
        QPair<qint64, qint64> size = scan(data->source_list());
        data->work_total = 2 * size.first;
        data->bytes_total = 2 * size.second;

        copy(data->source_list(), data->destination(), data);
        recursive_delete(data->source_list(), data);
    } else if (type == "recycle") {
#ifdef Q_OS_WIN32
        // delete files to Recycle Bin (FOF_ALLOWUNDO)
        foreach (const QString& path, data->source_list()) {
            const wchar_t *constPath = reinterpret_cast<const wchar_t *>(path.utf16());
            HRESULT hr;
            IFileOperation *fop;

            hr = CoCreateInstance(CLSID_FileOperation,
                                  NULL,
                                  CLSCTX_ALL,
                                  IID_PPV_ARGS(&fop));
            if (SUCCEEDED(hr)) {
                hr = fop->SetOperationFlags(FOF_ALLOWUNDO | FOF_SILENT);
                if (SUCCEEDED(hr)) {
                    IShellItem *shItem = 0;
                    hr = SHCreateItemFromParsingName(constPath,
                                                     NULL,
                                                     IID_PPV_ARGS(&shItem));
                    if (SUCCEEDED(hr)) {
                        hr = fop->DeleteItem(shItem, NULL);
                        shItem->Release();
                        if (SUCCEEDED(hr)) {
                            hr = fop->PerformOperations();

                            BOOL anyOperationsAborted;
                            fop->GetAnyOperationsAborted(&anyOperationsAborted);
                            if (anyOperationsAborted) {
                                data->any_ops_aborted = true;
                            }
                        } else {
                            qDebug() << COMError(hr);
                        }
                    } else {
                        qDebug() << COMError(hr);
                    }
                } else {
                    qDebug() << COMError(hr);
                }
                qDebug() << COMError(hr);
            } else {
                qDebug() << COMError(hr);
            }

            // BUG: abandonarea nu ofera informatii suficiente in meniul cu operatiuni
            // TODO: interfata pentru confirmari, erori etc. ar trebui sa fie facuta de mine
            // si aici
            // NOTE: ar trebui sa le sterg pe toate odata, poate este mai eficient
            // TODO: ar trebui sa pot vedea Recycle Bin-ul
        }
#else // linux
        foreach (const QString& path, data->source_list()) {
            auto data = trash_model->move_to_trash(path);
            operate(data); // strange way of doing this,
            // modifying arch will be needed to watch progress, I think
            // probably it would be the best to just read the `data`
            // here and do the operations in a recursive method, not by
            // calling operate (again..)

            /**
             * @todo notify progress here... it's all about moving files,
             * folders so I shouldn't ignore this thing.
             */
        }
#endif
    } else if (type == "delete") {
        // cross-platform, undoable
        recursive_delete(data->source_list(), data);
    }
    emit done(data);
}

void AsyncFileOperationWorker::copy(const QStringList &src, const QString &dest,
                                    FileOperationData* data)
{
    // assuming dest is a folder
    QDir destDir(dest);
    QFileInfo *sourceInfo;
    QFile sourceFile;
    QString sourceFileName, a;
    foreach (const QString& source, src) {
        sourceInfo = new QFileInfo(source);
        sourceFile.setFileName(source);
        if (sourceInfo->isFile() || sourceInfo->isSymLink()) {
            sourceFileName = sourceInfo->fileName();
            a = destDir.absoluteFilePath(sourceFileName);
            copy_file(source, a, data);
        } else {
            QDir sourceDir(source);
            destDir.mkdir(sourceDir.dirName());
            QStringList sourceDirEntries = sourceDir.entryList();
            QStringList sourceList;
            foreach (const QString& entry, sourceDirEntries) {
                if (entry != "." && entry != "..") {
                    QString sourceEntry = sourceDir.absoluteFilePath(entry);
                    sourceList << sourceEntry;
                }
            }
            copy(sourceList, destDir.absoluteFilePath(sourceDir.dirName()), data);
        }
        delete sourceInfo;
    }
}

void AsyncFileOperationWorker::recursive_delete(const QStringList &src,
                                               FileOperationData* data)
{
    QFileInfo *sourceInfo;
    QFile sourceFile;
    foreach (const QString& source, src) {
        sourceInfo = new QFileInfo(source);
        sourceFile.setFileName(source);
        if (sourceInfo->isFile() || sourceInfo->isSymLink()) {
            sourceFile.remove();
            data->work_so_far++;
            data->bytes_so_far += sourceInfo->size();
            emit progress(data);
        } else {
            QDir sourceDir(source);
            QStringList sourceDirEntries = sourceDir.entryList();
            QStringList sourceList;
            foreach (const QString& entry, sourceDirEntries) {
                if (entry != "." && entry != "..") {
                    QString sourceEntry = sourceDir.absoluteFilePath(entry);
                    sourceList << sourceEntry;
                }
            }
            recursive_delete(sourceList, data);
            sourceDir.rmdir(source);
        }
        delete sourceInfo;
    }
}

bool AsyncFileOperationWorker::copy_file(const QString &fileName, const QString &newName,
                                        FileOperationData *data)
{
    // took from QFile::copy in Qt 5.0.2 method and modified to fulfill my needs

    QFile f(fileName);
    if (fileName.isEmpty()) {
        qWarning("QFile::copy: Empty or null file name");
        return false;
    }
    if (QFile(newName).exists()) {
        // ### Race condition. If a file is moved in after this, it /will/ be
        // overwritten. On Unix, the proper solution is to use hardlinks:
        // return ::link(old, new) && ::remove(old); See also rename().
        qWarning("Destination file exists");
        return false;
    }

    bool error = false;
    if(!f.open(QFile::ReadOnly)) {
        error = true;
        qWarning("Cannot open %s for input", qPrintable(fileName));
    } else {
        QString fileTemplate = QLatin1String("%1/qt_temp.XXXXXX");
#ifdef QT_NO_TEMPORARYFILE
        QFile out(fileTemplate.arg(QFileInfo(newName).path()));
        if (!out.open(QIODevice::ReadWrite))
            error = true;
#else
        QTemporaryFile out(fileTemplate.arg(QFileInfo(newName).path()));
        if (!out.open()) {
            out.setFileTemplate(fileTemplate.arg(QDir::tempPath()));
            if (!out.open())
                error = true;
        }
#endif
        if (error) {
            out.close();
            f.close();
            qWarning("Cannot open for output");
        } else {
            char block[4096];
            qint64 totalRead = 0;
            for (int i = 1; !f.atEnd(); i++) {
                qint64 in = f.read(block, sizeof(block));
                if (in <= 0)
                    break;
                totalRead += in;
                data->bytes_so_far += in;
                if (i % 3 == 0 && totalRead != f.size()) {
                    emit progress(data);
                }
                if(in != out.write(block, in)) {
                    f.close();
                    qWarning("Failure to write block");
                    error = true;
                    break;
                }
            }

            if (totalRead != f.size()) {
                // Unable to read from the source. The error string is
                // already set from read().
                error = true;
            }
            if (!error && !out.rename(newName)) {
                error = true;
                f.close();
                qWarning("Cannot create %s for output", qPrintable(newName));
            }
#ifdef QT_NO_TEMPORARYFILE
            if (error)
                out.remove();
#else
            if (!error)
                out.setAutoRemove(false);
#endif
        }
    }
    if(!error) {
        QFile::setPermissions(newName, f.permissions());
        f.close();
        data->work_so_far++;
        emit progress(data);
        return true;
    }
    return false;
}


QPair<qint64, qint64> AsyncFileOperationWorker::scan(const QStringList &src)
{
    qint64 ci = 0, cb = 0;
    QFileInfo *sourceInfo;
    QFile sourceFile;
    foreach (const QString& source, src) {
        sourceInfo = new QFileInfo(source);
        sourceFile.setFileName(source);
        if (sourceInfo->isFile() || sourceInfo->isSymLink()) {
            ci++;
            cb += sourceInfo->size();
        } else {
            QDir sourceDir(source);
            QStringList sourceDirEntries = sourceDir.entryList();
            QStringList sourceList;
            foreach (const QString& entry, sourceDirEntries) {
                if (Q_LIKELY(entry != "." && entry != "..")) {
                    QString sourceEntry = sourceDir.absoluteFilePath(entry);
                    sourceList << sourceEntry;
                }
            }
            QPair<qint64, qint64> pair = scan(sourceList);
            ci += pair.first;
            cb += pair.second;
        }
        delete sourceInfo;
    }
    return qMakePair(ci, cb);
}
