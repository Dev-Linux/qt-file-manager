#include "DirController.h"

#include "views/MainWindow.h"
#include "views/View.h"
#include "views/ListViewItem.h"
#include "views/ViewWidget.h"
#include "views/ViewScrollArea.h"
#include "views/FileOperationsMenu.h"
#include "views/FileOperationItem.h"

#include "models/ViewSelectionModel.h"
#include "models/DirModel.h"
#include "models/DockModel.h"
#include "models/FileOperationData.h"

#include "DockController.h"
#include "MainWindowController.h"

#include "AsyncFileOperation.h"
#include "misc.h"

/**
 * @class DirController
 * @brief DirController class
 * @deprecated Use WorkspaceController instead.
 */

DirController::DirController(QObject *parent) :
    QObject(parent)
{
    model = 0;

    view = new View();
    view->ctrl = this;
    connect(view, &View::search,
            this, &DirController::search);

    set_model(new DirModel());
}

void DirController::set_model(DirModel *m)
{
    bool re = has_model();
    if (re) {
        disconnect(model->sel, &ViewSelectionModel::changed,
                this, &DirController::sel_model_changed);
        disconnect(model, &DirModel::added,
                this, &DirController::add_item);
        disconnect(model, &DirModel::cleared,
                this, &DirController::clear_view);
        disconnect(model, &DirModel::tag_added,
                this, &DirController::add_tag);

        disconnect(model, &DirModel::path_changed,
                this, &DirController::path_changed);
        disconnect(model, &DirModel::name_filters_changed,
                this, &DirController::model_name_filters_changed);
    }
    model = m;
    if (m != 0) {
        connect(model->sel, &ViewSelectionModel::changed,
                this, &DirController::sel_model_changed);
        connect(model, &DirModel::added,
                this, &DirController::add_item);
        connect(model, &DirModel::cleared,
                this, &DirController::clear_view);
        connect(model, &DirModel::tag_added,
                this, &DirController::add_tag);

        connect(model, &DirModel::path_changed,
                this, &DirController::path_changed);
        connect(model, &DirModel::name_filters_changed,
                this, &DirController::model_name_filters_changed);
    }
    if (re) {
        refresh_view();
    }
}

bool DirController::has_model()
{
    return model != 0;
}

void DirController::set_path(const QString &path)
{
    model->set_path(path);
}

QSet<int> &DirController::saved_selected_indices()
{
    return model->sel->saved_set;
}

QList<FileInfo> &DirController::file_info_list()
{
    return model->list;
}

void DirController::item_clicked(const Qt::KeyboardModifiers &modifiers)
{
    auto clickedItem = qobject_cast<ListViewItem*>(sender());
    int index = view->index_of(clickedItem);

    if (!modifiers.testFlag(Qt::ControlModifier)) {
        bool clickedItemWasSelected = model->selected(index);
        int selc = model->sel->count();
        model->sel->clear();
        if (!clickedItemWasSelected || selc > 1) {
            model->sel->add(index);
        }
    } else {
        bool clickedItemSelected = model->selected(index);
        if (!clickedItemSelected) {
            model->sel->add(index);
        } else {
            model->sel->remove(index);
        }
    }
    model->sel->save();
}

void DirController::item_right_clicked()
{
    ListViewItem* clickedItem = qobject_cast<ListViewItem*>(sender());
    int index = view->index_of(clickedItem);
    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    // a try of making multiple selection
    // actually this mechanism should be implemented in the model first
    // the secondary selection should be dimmed and the user should be able
    // to swap the primary selection with the secondary selection
    // eventually, a history of selections would be complex but a nice feature

    /*bool init = item->isHighlighted();
    if (!init) {
        item->setHighlighted(true);
    }*/

    QMenu menu;
    menu.addAction("It's important");
#ifdef Q_OS_WIN32
    menu.addAction("Recycle this");
#else
    menu.addAction("Delete this");
#endif

    connect(&menu, &QMenu::triggered,
            this, &DirController::context_menu_triggered);
    menu.exec(QCursor::pos());

    /*if (!init) {
        item->setHighlighted(init);
    }*/
}

void DirController::item_double_clicked()
{
    ListViewItem* item = qobject_cast<ListViewItem*>(sender());
    int index = view->index_of(item);
    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    open_index(index);
}

void DirController::add_item(const FileInfo &info)
{
    auto item = new ListViewItem(info);

    connect(item, &ListViewItem::clicked,
            this, &DirController::item_clicked);

    connect(item, &ListViewItem::double_clicked,
            this, &DirController::item_double_clicked);

    connect(item, &ListViewItem::right_clicked,
            this, &DirController::item_right_clicked);

    view->add_item(item);
}

void DirController::clear_view()
{
    misc::clear_QLayout(view->w->layout());
}

void DirController::sel_model_changed(QSet<int> added,
                                          QSet<int> removed)
{
    int lastAdded = -1;
    foreach (const int &x, added) {
        view->item_at(x)->set_selected(true);
        lastAdded = x;
    }
    foreach (const int &x, removed) {
        view->item_at(x)->set_selected(false);
    }
    if (lastAdded != -1) {
        view->s->scroll_to(lastAdded);
    }
}

void DirController::refresh_view()
{
    clear_view();
    if (has_model()) {
        foreach (const FileInfo &info, model->list) {
            add_item(info);
        }
        foreach (const int &x, saved_selected_indices()) {
            view->item_at(x)->set_selected(true);
        }
    }
}

void DirController::add_tag(int index, const QString &tag)
{
    if (tag == "recent") {
        auto item = view->item_at(index);
        item->set_highlighted(true);
    }
}

void DirController::context_menu_triggered(QAction *action)
{
    MainWindowController *main_win_ctrl = MainWindowController::instance();
    if (action->text() == "It's important") {
        foreach (const int &x, model->sel->saved_set) {
            auto path = view->item_at(x)->file_info.abs_file_path();
            main_win_ctrl->mark_path_as_important(path);
        }
    } else if (action->text() == "Delete this" ||
               action->text() == "Recycle this") {
        auto list = model->sel->saved_set;
        QStringList pathList;
        foreach (const int& index, list) {
            QString path = model->list[index].abs_file_path();
            pathList << QDir::toNativeSeparators(path);
        }

        auto data = new FileOperationData("delete", pathList);
        auto item = new FileOperationItem(data);
        QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        //item->setLabel("<strong>" + s + "</strong>.");
        auto s = pathList.join(",<br>");
        main_win_ctrl->view->file_ops_menu->add_item(item);
        view->op->do_async(data, item);
    }
}

void DirController::search(const QString &str)
{
    emit search_started(str);
}

void DirController::model_name_filters_changed(const QStringList &nf)
{
    Q_UNUSED(nf);
}

void DirController::open_index(int index)
{
    auto item = view->item_at(index);
    QString path = item->file_info.file_path();

    if (item->file_info.is_dir()) {
        set_path(path);
    } else {
        if (misc::open_local_file(path)) {
            model->add_tag(index, "recent");
        }
    }
}

bool DirController::reduce_and_translate_sel_by(int x)
{
    auto sel = model->sel;

    int sel1 = sel->one();
    bool check;
    if (x > 0) {
        check = sel1 < model->count() - 1;
    } else {
        Q_ASSERT(x != 0);
        check = sel1 > 0;
    }

    if (check) {
        sel1 += x;

        sel->clear();
        sel->add(sel1);
        sel->save();

        return true;
    }
    return false;
}
