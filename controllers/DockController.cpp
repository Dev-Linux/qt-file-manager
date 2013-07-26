#include "DockController.h"

#include "models/DockModel.h"

#include "views/Dock.h"
#include "views/View.h"
#include "views/ListViewItem.h"

DockController::DockController() :
    QObject()
{
    this->model = new DockModel();
    this->view = new Dock();

    connect(model, &DockModel::added,
            this, &DockController::add_item);
    connect(model, &DockModel::removed,
            view, &Dock::remove_item);

    connect(view, &Dock::remove_action_triggered,
            this, &DockController::remove_action_triggered);

    model->read_settings();
}

void DockController::add_item(const FileInfo &info)
{
    ListViewItem *item = new ListViewItem(info);

    connect(item, &ListViewItem::double_clicked,
            view, &Dock::item_double_clicked);
    connect(item, &ListViewItem::right_clicked,
            view, &Dock::item_right_clicked);

    view->view->add_item(item);
}

void DockController::remove_action_triggered()
{
    model->remove_path(view->selected_item->file_info.abs_file_path());
}
