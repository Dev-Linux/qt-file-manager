#ifndef QUICKFILEGRAPHVIEW_H
#define QUICKFILEGRAPHVIEW_H

#include <QtQml>
#include <QtQuick>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QtWidgets>
#include "filesystemmodel.h"

class QuickFileGraphView : public QQuickView
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QFileInfo> driveList READ driveList)
public:
    explicit QuickFileGraphView(FileSystemModel *fsModel);
    QQmlListProperty<QFileInfo> driveList();
    QWidget *containerWidget();

signals:
    
public slots:
    
private:
    QFileInfoList *drives;
    QList<QFileInfo *> drivesPointers;
    FileSystemModel *fsModel;
};

#endif // QUICKFILEGRAPHVIEW_H
