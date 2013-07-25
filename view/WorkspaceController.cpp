#include "WorkspaceController.h"

#include "WorkspaceView.h"
#include "DirModel.h"
#include "DockModel.h"
#include "TabBarItem.h"
#include "RootItemController.h"

WorkspaceController::WorkspaceController(DirModel *model,
                                         DockModel *dockModel) :
    QObject()
{
    m_root_item_ctrl = new RootItemController(model, this, dockModel);

    view = new WorkspaceView(model, m_root_item_ctrl->view);

    m_root_item_ctrl->workspace_view_resized();

    connect(model, &DirModel::pathChanged,
            view->tabBarItem, &TabBarItem::modelPathChanged);
    connect(view, &WorkspaceView::resized,
            this, &WorkspaceController::view_resized);
    connect(view, &WorkspaceView::sel_rect_changed,
            m_root_item_ctrl, &RootItemController::sel_rect_changed);
}

void WorkspaceController::set_layout(RootItem::Layout layout)
{
    view->root_item_view->setLayout(layout);
}

void WorkspaceController::set_scene_rect_to_items_bounding_rect()
{
    // without this condition, it crashes
    if (view != nullptr) {
        view->setSceneRect(view->scene->itemsBoundingRect());
    }
}

QList<QGraphicsItem *>
WorkspaceController::items_intersecting_rect(QRectF rect) const
{
    return view->scene->items(rect, Qt::IntersectsItemShape);
}

int WorkspaceController::viewport_width() const
{
    return view->viewport()->width();
}

int WorkspaceController::viewport_height() const
{
    return view->viewport()->height();
}

void WorkspaceController::ensure_visible(const QGraphicsItem *item)
{
    view->ensureVisible(item, 0, 0);
}

void WorkspaceController::zoom_in()
{
    view->scale(1.2, 1.2);
}

void WorkspaceController::zoom_out()
{
    view->scale(1 / 1.2, 1 / 1.2);
}

void WorkspaceController::view_resized()
{
    m_root_item_ctrl->workspace_view_resized();
}
