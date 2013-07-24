#pragma once
#ifndef ROOTITEMCONTROLLER_H
#define ROOTITEMCONTROLLER_H

#include <QObject>

class RootItem;
class DirModel;
class WorkspaceView;
class DockModel;
class FileInfo;

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
    //! The DockModel which stores the items marked as important.
    DockModel *m_dock_model;
    DirModel *m_dir_model;
    WorkspaceView *m_workspace_view;
    void refresh_tags();

private slots:
    void path_changed(const QString &path);
    void name_filters_changed();
    void file_system_change();
    void important_added(FileInfo &info);
    void important_removed(FileInfo &info);
    void dir_model_tag_added(int index, const QString &tag);
    void dir_model_tag_removed(int index, const QString &tag);
};

#endif // ROOTITEMCONTROLLER_H
