#pragma once
#ifndef ROOTITEMCONTROLLER_H
#define ROOTITEMCONTROLLER_H

#include <QObject>
#include "RootItem.h"

class RootItem;
class DirModel;
class WorkspaceView;
class WorkspaceController;
class DockModel;
class FileInfo;
class FileNode;

class RootItemController : public QObject
{
    Q_OBJECT
public:
    explicit RootItemController(DirModel* dir_model,
                                WorkspaceController *workspace_ctrl,
                                DockModel *dock_model);
    RootItem *view;
signals:
    
public slots:
    void sel_rect_changed(QRect sel_rect, QPointF from_scene_pt,
                          QPointF to_scene_pt);
    void workspace_view_resized();

private:
    //! The DockModel which stores the items marked as important.
    DockModel *m_dock_model;
    DirModel *m_dir_model;
    WorkspaceController *m_workspace_ctrl;

    void refresh_tags();
    //! The context menu.
    QMenu *m_context_menu = nullptr;
    qreal m_viewport_height, m_y;

private slots:
    void path_changed(const QString &path);
    void name_filters_changed();
    void file_system_change();
    void important_added(FileInfo &info);
    void important_removed(FileInfo &info);
    void dir_model_tag_added(int index, const QString &tag);
    void dir_model_tag_removed(int index, const QString &tag);
    void node_dbl_clicked(FileNode *node);
    void node_left_clicked(FileNode *node,
                           const Qt::KeyboardModifiers &modifiers);
    void node_right_clicked(FileNode *node,
                            const Qt::KeyboardModifiers &modifiers);
    void context_menu_triggered(QAction *action);
    void view_position_changed(QPointF new_pos);
    void before_adding_n(int n);
    void layout_updated();
    void before_layout_update(RootItem::Layout layout);
    void sel_model_changed(QSet<int> added, QSet<int> removed);
};

#endif // ROOTITEMCONTROLLER_H
