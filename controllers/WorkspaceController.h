#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>

#include "views/RootItem.h"

class WorkspaceView;
class DirModel;
class DockModel;
class RootItemController;

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

private slots:
    void view_resized();
};

#endif // WORKSPACECONTROLLER_H
