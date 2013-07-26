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
    QDir *files_dir;
    QDir *info_dir;
    
signals:
    
public slots:
    FileOperationData* move_to_trash(const QString &path);

};

#endif // TRASHMODEL_H
