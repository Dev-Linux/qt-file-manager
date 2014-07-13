#pragma once
#ifndef DOCKMODEL_H
#define DOCKMODEL_H

#include <QtCore>

#include "FileInfo.h"

class DockModel : public QObject
{
    Q_OBJECT
public:
    explicit DockModel(QObject *parent = 0);

    bool reading_settings;
    void read_settings();
    void write_settings();
    bool add_path(const QString &path);
    void remove_path(const QString &path);
    bool contains_path(const QString &path);

    QList<FileInfo> list;
    QSettings *s;
    
signals:
    void added(FileInfo &info);
    void removed(FileInfo &info);
    
public slots:
    
};

#endif // DOCKMODEL_H
