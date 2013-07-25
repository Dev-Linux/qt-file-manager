#pragma once
#ifndef TRASHMODEL_H
#define TRASHMODEL_H

#include <QtCore>

class FileOperationData;

class TrashModel : public QObject
{
    Q_OBJECT
public:
    explicit TrashModel();
    QProcessEnvironment env;
    QDir *filesDir;
    QDir *infoDir;
    
signals:
    
public slots:
    FileOperationData* moveToTrash(const QString &path);
};

#endif // TRASHMODEL_H
