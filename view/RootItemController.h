#pragma once
#ifndef ROOTITEMCONTROLLER_H
#define ROOTITEMCONTROLLER_H

#include <QObject>

class RootItem;
class DirModel;
class WorkspaceView;
class DockModel;

class RootItemController : public QObject
{
    Q_OBJECT
public:
    explicit RootItemController(DirModel* dir_model,
                                WorkspaceView *workspace_view,
                                DockModel *dock_model);
    RootItem *view;
signals:
    
public slots:
    
private:
    DockModel *m_dock_model;
    DirModel *m_dir_model;
    void refresh_tags();

private slots:
    void path_changed(const QString &path);
    void name_filters_changed();
    void file_system_change();
};

#endif // ROOTITEMCONTROLLER_H
