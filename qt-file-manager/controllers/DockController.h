#pragma once
#ifndef DOCKCONTROLLER_H
#define DOCKCONTROLLER_H

#include <QObject>

class Dock;
class DockModel;
class FileInfo;

class DockController : public QObject
{
    Q_OBJECT
public:
    explicit DockController();
    Dock *view;
    DockModel *model;
signals:
    
public slots:

private slots:
    void add_item(const FileInfo &info);
    void remove_action_triggered();
};

#endif // DOCKCONTROLLER_H
