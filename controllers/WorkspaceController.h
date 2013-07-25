#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>

#include "views/RootItem.h"

class WorkspaceView;
class DirModel;
class DockModel;
class RootItemController;
class FileNode;

class WorkspaceController : public QObject
{
    Q_OBJECT
public:
    explicit WorkspaceController(DirModel *model,
                                 DockModel *dockModel);
    //! The WorkspaceView which contains the RootItem.
    WorkspaceView *view = nullptr;
    void set_layout(RootItem::Layout layout);
    void set_scene_rect_to_items_bounding_rect();
    QList<QGraphicsItem *> items_intersecting_rect(QRectF rect) const;
    int viewport_width() const;
    int viewport_height() const;
    void ensure_visible(const QGraphicsItem *item);

signals:
    
public slots:
    void zoom_in();
    void zoom_out();

private:
    RootItemController *m_root_item_ctrl;
    DirModel *m_dir_model;

private slots:
    void view_resized();
    void clicked_on_empty_space();
    void before_drag(FileNode *drag_start_node);
    void escape_key_pressed();
};

#endif // WORKSPACECONTROLLER_H
