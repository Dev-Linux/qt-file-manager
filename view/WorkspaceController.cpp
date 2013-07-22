#include "WorkspaceController.h"

#include "WorkspaceView.h"
#include "DirModel.h"
#include "DockModel.h"

WorkspaceController::WorkspaceController(DirModel *model,
                                         DockModel *dockModel) :
    QObject()
{
    view = new WorkspaceView(model, dockModel);
}

void WorkspaceController::set_layout(RootItem::Layout layout)
{
    view->rootItem->setLayout(layout);
}

void WorkspaceController::zoom_in()
{
    view->zoomIn();
}

void WorkspaceController::zoom_out()
{
    view->zoomOut();
}
