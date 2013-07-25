#include "WorkspaceController.h"

#include "views/WorkspaceView.h"
#include "views/TabBarItem.h"
#include "views/FileNode.h"

#include "models/DirModel.h"
#include "models/DockModel.h"
#include "models/ViewSelectionModel.h"

#include "controllers/RootItemController.h"

WorkspaceController::WorkspaceController(DirModel *model,
                                         DockModel *dockModel) :
    QObject()
{
    m_dir_model = model;
    m_root_item_ctrl = new RootItemController(m_dir_model, this,
                                              dockModel);

    view = new WorkspaceView(m_root_item_ctrl->view);

    m_root_item_ctrl->workspace_view_resized();

    connect(m_dir_model, &DirModel::pathChanged,
            view->tabBarItem, &TabBarItem::modelPathChanged);
    connect(view, &WorkspaceView::resized,
            this, &WorkspaceController::view_resized);
    connect(view, &WorkspaceView::sel_rect_changed,
            m_root_item_ctrl, &RootItemController::sel_rect_changed);
    connect(view, &WorkspaceView::clicked_on_empty_space,
            this, &WorkspaceController::clicked_on_empty_space);
    connect(view, &WorkspaceView::before_drag,
            this, &WorkspaceController::before_drag);
    connect(view, &WorkspaceView::escape_key_pressed,
            this, &WorkspaceController::escape_key_pressed);
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

void WorkspaceController::clicked_on_empty_space()
{
    if (!m_dir_model->sel->isEmpty()) {
        m_dir_model->sel->clear();
        m_dir_model->sel->save();
    }
}

/**
 * @brief Changes the selection before drag.
 * @param drag_start_node The node through which the drag was started
 * (a.k.a. The node under the mouse cursor).
 */
void WorkspaceController::before_drag(FileNode *drag_start_node)
{
    // change drag selection
    int index = m_root_item_ctrl->view->
            fileNodes.indexOf(drag_start_node);
    bool draggedNodeWasSelected = m_dir_model->selected(index);

    if (!draggedNodeWasSelected) {
        m_dir_model->sel->clear();
        m_dir_model->sel->add(index);
        m_dir_model->sel->save();
    }

    view->nodes_to_drag = m_dir_model->sel->savedSet;
}

/**
 * @brief Called when #view gets an Esc press. Clears the selection.
 */
void WorkspaceController::escape_key_pressed()
{
    m_dir_model->sel->clear();
    m_dir_model->sel->save();
}
