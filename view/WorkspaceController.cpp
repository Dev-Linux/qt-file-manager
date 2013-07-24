#include "WorkspaceController.h"

#include "WorkspaceView.h"
#include "DirModel.h"
#include "DockModel.h"
#include "TabBarItem.h"

WorkspaceController::WorkspaceController(DirModel *model,
                                         DockModel *dockModel) :
    QObject()
{
    view = new WorkspaceView(model, dockModel);

    connect(model, &DirModel::pathChanged,
            view->tabBarItem, &TabBarItem::modelPathChanged);
    connect(view, &WorkspaceView::rubberBandChanged,
            view->root_item_view, &RootItem::selRectChanged);
}

void WorkspaceController::set_layout(RootItem::Layout layout)
{
    view->root_item_view->setLayout(layout);
}

void WorkspaceController::zoom_in()
{
    view->scale(1.2, 1.2);
}

void WorkspaceController::zoom_out()
{
    view->scale(1 / 1.2, 1 / 1.2);
}
