#pragma once
#ifndef DOCKMODEL_H
#define DOCKMODEL_H

#include <QtCore>

#include "fileinfo.h"

class DockModel : public QObject
{
    Q_OBJECT
public:
    explicit DockModel(QObject *parent = 0);
    QList<FileInfo> list;
    QSettings *s;
    bool readingSettings;
    void readSettings();
    void writeSettings();
    bool addPath(const QString &path);
    void removePath(const QString &path);
    bool containsPath(const QString &path);
    
signals:
    void added(FileInfo &info);
    void removed(FileInfo &info);
    
public slots:
    
};

#endif // DOCKMODEL_H
