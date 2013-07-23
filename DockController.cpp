#include "DockController.h"

#include "Dock.h"
#include "DockModel.h"

#include "view/View.h"
#include "view/ListViewItem.h"

DockController::DockController(QObject *parent) :
    QObject(parent)
{
    this->model = new DockModel();
    this->view = new Dock();

    connect(model, &DockModel::added,
            this, &DockController::add_item);
    connect(model, &DockModel::removed,
            view, &Dock::removeItem);

    connect(view, &Dock::remove_action_triggered,
            this, &DockController::remove_action_triggered);

    model->readSettings();
}

void DockController::add_item(const FileInfo &info)
{
    ListViewItem *item = new ListViewItem(info);

    connect(item, &ListViewItem::doubleClicked,
            view, &Dock::itemDoubleClicked);
    connect(item, &ListViewItem::rightClicked,
            view, &Dock::itemRightClicked);

    view->view->addItem(item);
}

void DockController::remove_action_triggered()
{
    model->removePath(view->selectedItem->fileInfo.absoluteFilePath());
}
