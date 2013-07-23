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
    explicit DockController(QObject *parent = 0);
    Dock *view;
    DockModel *model;
signals:
    
public slots:

private slots:
    void add_item(const FileInfo &info);
    
};

#endif // DOCKCONTROLLER_H
