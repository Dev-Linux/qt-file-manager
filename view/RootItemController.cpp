#include "RootItemController.h"
#include "RootItem.h"

#include "DirModel.h"
#include "ViewSelectionModel.h"
#include "DockModel.h"

#include "WorkspaceController.h"
#include "WorkspaceView.h"

#include "MainWindowController.h"
#include "MainWindow.h"

#include "FileOperationData.h"
#include "FileOperationItem.h"
#include "FileOperationsMenu.h"
#include "AsyncFileOperation.h"

#include "FileNode.h"
#include "misc.h"

RootItemController::RootItemController(DirModel* dir_model,
                               WorkspaceController *workspace_ctrl,
                               DockModel *dock_model) :
    QObject()
{
    // I don't think it's good that this class depends on so many
    // components, also, the dir_model should be part of a RootItemModel
    // or most probably not because I should be able to use it without
    // a directory, but there certainly should be a RootItemModel class.

    m_dock_model = dock_model;
    m_dir_model = dir_model;
    m_workspace_ctrl = workspace_ctrl;
    view = new RootItem();

    connect(dock_model, &DockModel::added,
            this, &RootItemController::important_added);
    connect(dock_model, &DockModel::removed,
            this, &RootItemController::important_removed);

    connect(dir_model, &DirModel::pathChanged,
            this, &RootItemController::path_changed);
    connect(dir_model, &DirModel::nameFiltersChanged,
            this, &RootItemController::name_filters_changed);

    connect(dir_model->sel, &ViewSelectionModel::changed,
            this, &RootItemController::sel_model_changed);

    connect(dir_model, &DirModel::cleared,
            view, &RootItem::clearView);
    connect(dir_model, &DirModel::added,
            view, &RootItem::addNode);
    connect(dir_model, &DirModel::file_system_change,
            this, &RootItemController::file_system_change);
    connect(dir_model, &DirModel::before_adding_n,
            this, &RootItemController::before_adding_n);

    connect(dir_model, &DirModel::tagAdded,
            this, &RootItemController::dir_model_tag_added);
    connect(dir_model, &DirModel::tagRemoved,
            this, &RootItemController::dir_model_tag_removed);

    connect(view, &RootItem::node_dbl_clicked,
            this, &RootItemController::node_dbl_clicked);
    connect(view, &RootItem::node_left_clicked,
            this, &RootItemController::node_left_clicked);
    connect(view, &RootItem::node_right_clicked,
            this, &RootItemController::node_right_clicked);
    connect(view, &RootItem::position_changed,
            this, &RootItemController::view_position_changed);
    connect(view, &RootItem::before_layout_update,
            this, &RootItemController::before_layout_update);
    connect(view, &RootItem::layout_updated,
            this, &RootItemController::layout_updated);
}

/**
 * @brief Adds the important tag to files marked as important in the
 * current directory.
 */
void RootItemController::refresh_tags()
{
    // dock FileInfos in the current folder
    QList<FileInfo> list =
            misc::filterByDirAbsPath(m_dock_model->list,
                            m_dir_model->dir.absolutePath());

    QList<FileInfo>::iterator it, ei, bi, i;
    for (it = list.begin(); it != list.end(); ++it) {
        FileInfo &info = *it;
        bi = m_dir_model->list.begin();
        ei = m_dir_model->list.end();
        i = qFind(bi, ei, info);

        Q_ASSERT(i != ei);

        const int index = i - bi;

        m_dir_model->addTag(index, "important");
    }
}

/**
 * @brief Called when the model's path has changed. Updates FileNode positions,
 * the scene rect shown by the GraphView and refreshes tags using refreshTags().
 * @param path The new path of the DirModel.
 */
void RootItemController::path_changed(const QString &path)
{ Q_UNUSED(path)
    view->update_layout();
    m_workspace_ctrl->set_scene_rect_to_items_bounding_rect();
    refresh_tags();
}

/**
 * @brief Replaced by clearView(), addItem() and refreshPos().
 *
 * @bug Still called by search.
 *
 * @deprecated
 */
void RootItemController::name_filters_changed()
{
    view->clearView();
    //loadAllFilesInModel();
    Q_ASSERT_X(false, "RootItemController::name_filters_changed",
               "not sure I should call loadAllFilesInModel");
    m_workspace_ctrl->set_scene_rect_to_items_bounding_rect();
    refresh_tags();
}

/**
 * @brief Called when the model encounters a file system change.
 */
void RootItemController::file_system_change()
{
    view->update_layout();
}

/**
 * @brief Called when a folder was marked as important.
 * @param info The file/folder marked as important.
 * @note QDir operator == doesn't work even though they have the
 * same path and absolutePath.
 */
void RootItemController::important_added(FileInfo &info)
{
    // NOTE: also compares sort and filter settings
    if (info.absoluteDir().absolutePath() ==
            m_dir_model->dir.absolutePath()) { //! @todo drives on Win?
        for (int i = 0; i < m_dir_model->count(); i++) {
            if (m_dir_model->list[i] == info) {
                m_dir_model->addTag(i, "important");
                break;
            }
        }
    }
}

/**
 * @brief Called when an important folder was removed from the dock.
 * @param info The file/folder unmarked important.
 * @note QDir operator == doesn't work even though they have the
 * same path and absolutePath.
 */
void RootItemController::important_removed(FileInfo &info)
{
    // NOTE: also compares sort and filter settings
    if (info.absoluteDir().absolutePath() ==
            m_dir_model->dir.absolutePath()) { //! @todo drives on Win?
        for (int i = 0; i < m_dir_model->count(); i++) {
            if (m_dir_model->list[i] == info) {
                m_dir_model->removeTag(i, "important");
                break;
            }
        }
    }
}

void RootItemController::dir_model_tag_added(int index,
                                             const QString &tag)
{
    if (tag == "important") {
        auto node = view->fileNodes[index];
        node->setHighlighted();
    }
}

void RootItemController::dir_model_tag_removed(int index,
                                               const QString &tag)
{
    if (tag == "important") {
        auto node = view->fileNodes[index];
        node->setHighlighted(false);
    }
}

/**
 * @brief Called when an item was double-clicked.
 * @param node The clicked FileNode.
 */
void RootItemController::node_dbl_clicked(FileNode *node)
{
    int index = view->fileNodes.indexOf(node);

    bool clickedItemWasSelected = m_dir_model->selected(index);

    if (!clickedItemWasSelected) {
        m_dir_model->sel->clear();
        m_dir_model->sel->add(index);
        m_dir_model->sel->save();
    }

    if (node->fileInfo.isDir()) {
        const QString &path = node->fileInfo.absoluteFilePath();
        m_dir_model->setPath(path);
    } else {
        if (misc::openLocalFile(node->fileInfo.absoluteFilePath())) {
            // not implemented actually...
            m_dir_model->addTag(index, "recent");
        }
    }
}

/**
 * @brief Called when an item was left-clicked.
 *
 * @param node The left-clicked node.
 *
 * @param modifiers The modifiers pressed at the moment of the mouse
 * button press.
 *
 * @bug (Not necessary to be fixed here). Steps to reproduce:
 * 1. Right click on a file node.
 * 2. Left-click and hold while drawing a sel. rect on a point in empty
 * space.
 */
void RootItemController::node_left_clicked(FileNode *node,
                            const Qt::KeyboardModifiers &modifiers)
{
    FileNode * const clickedItem = node;
    metaDebug(clickedItem->fileInfo.absoluteFilePath());

    int index = view->fileNodes.indexOf(clickedItem);

    if (!modifiers.testFlag(Qt::ControlModifier)) {
        bool clickedItemWasSelected = m_dir_model->selected(index);
        int selc = m_dir_model->sel->count();
        m_dir_model->sel->clear();
        if (!clickedItemWasSelected || selc > 1) {
            m_dir_model->sel->add(index);
        }
    } else {
        bool clickedItemSelected = m_dir_model->selected(index);
        if (!clickedItemSelected) {
            m_dir_model->sel->add(index);
        } else {
            m_dir_model->sel->remove(index);
        }
    }
    m_dir_model->sel->save();
}

/**
 * @brief Called when an item was right-clicked.
 *
 * @param node The right-clicked node.
 *
 * @param modifiers The modifiers pressed at the moment of the mouse
 * button press.
 *
 * @bug The mouse move events are not detected between two consecutive
 * creations of the menu. (bug appeared again, I thought I've fixed it).
 *
 * @todo After fixing the above bug, try to create the menu only once,
 * at the creation of RootItemController, in the constructor. [optimize]
 */
void RootItemController::node_right_clicked(FileNode *node,
                            const Qt::KeyboardModifiers &modifiers)
{ Q_UNUSED(modifiers)
    FileNode * const clickedItem = node;
    int index = view->fileNodes.indexOf(clickedItem);
    bool clickedItemWasSelected = m_dir_model->selected(index);

    if (!clickedItemWasSelected) {
        m_dir_model->sel->clear();
        m_dir_model->sel->add(index);
        m_dir_model->sel->save();
    }

    QApplication::processEvents(); // not sure if it is necessary

    // a try of making multiple selection
    // actually this mechanism should be implemented in the model first
    // the secondary selection should be dimmed and the user should be able
    // to swap the primary selection with the secondary selection
    // eventually, a history of selections would be complex but a nice feature

    /*bool init = item->isHighlighted();
    if (!init) {
        item->setHighlighted(true);
    }*/

    if (m_context_menu != nullptr) {
        delete m_context_menu;
    }
    m_context_menu = new QMenu();
    connect(m_context_menu, &QMenu::triggered,
            this, &RootItemController::context_menu_triggered);
    m_context_menu->addAction("It's important");
    m_context_menu->addAction("Recycle this");
    m_context_menu->addAction("Delete this");
    m_context_menu->popup(QCursor::pos());

    //menu.hide();
    //menu.popup(QCursor::pos());
    // still not working...

    // QApplication::processEvents(); - not working
    //
    // events such as mouse move are not processed
    // and because of this, directly clicking on an unselected
    // node while the context menu for another node is open
    // doesn't change the selection and the new menu opened is for
    // the last selection, not for the current one
    /**
     * @todo search the web for "mouse move [while] popup menu [open]" or
     * similar...
     */

    /*if (!init) {
        item->setHighlighted(init);
    }*/
}

/**
 * @brief Called when an action from the FileNode context menu was
 * triggered.
 *
 * @param action The triggered action.
 */
void RootItemController::context_menu_triggered(QAction *action)
{
    auto main_win_ctrl = MainWindowController::instance();
    auto main_win = main_win_ctrl->view;
    if (action->text() == "It's important") {
        auto list = m_dir_model->selectedAbsolutePaths(false);
        m_dir_model->sel->clear();
        m_dir_model->sel->save();

        for (auto i = list.begin(); i != list.end(); ++i) {
            main_win_ctrl->markPathAsImportant(*i);
        }
    } else if (action->text() == "Delete this") {
        QStringList pathList = m_dir_model->
                selectedAbsolutePaths(true);
        auto data = new FileOperationData("delete", pathList);

        // item handling:
        auto item = new FileOperationItem(data);

        //QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        auto s = pathList.join(",<br>");
        item->setLabel("<strong>" + s + "</strong>.");
        main_win->fileOperationsMenu->addItem(item);
        main_win_ctrl->async_file_op->doAsync(data, item);
    } else if (action->text() == "Recycle this") {
        QStringList pathList = m_dir_model->
                selectedAbsolutePaths(false); // true? this WILL be X-OS

        auto data = new FileOperationData("recycle", pathList);

        // item handling:

        auto item = new FileOperationItem(data);
        //QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        //auto s = pathList.join(",<br>");

        // QDir::nativePathSeparators seems unnecessary at least for
        // now, because this if branch is only called on linux whose
        // native separator is the one provided by TrashModel anyway.
        // auto s = pathList.join(",<br>");

        item->setLabel("<strong>" + s + "</strong>.");
        main_win->fileOperationsMenu->addItem(item);
        main_win_ctrl->async_file_op->doAsync(data, item);
    }
}

/**
 * @brief Called when the rubberBandRect or selection rectangle
 * changes. Updates the selection model.
 */
void RootItemController::sel_rect_changed(QRectF sel_rect)
{
    auto mod = QApplication::keyboardModifiers();

    // items itersecting sel rect
    auto items = m_workspace_ctrl->items_intersecting_rect(sel_rect);

    // nodes itersecting sel rect (RootItem contains only nodes)
    items = misc::filterByAncestor(items, this->view);

    if (!items.isEmpty()) {
        if (!mod.testFlag(Qt::ControlModifier)) {
            m_dir_model->sel->clear();
        }
        for (auto i = items.begin(); i != items.end(); ++i) {
            auto node = static_cast<FileNode*>(*i);
            int index = view->fileNodes.indexOf(node);
            m_dir_model->sel->add(index);
        }
        m_dir_model->sel->save();
    }
}

/**
 * @brief Called when the GraphView's size changes and from the ctor.
 * @see
 * - RootItem()
 * - GraphView::resizeEvent()
 */
void RootItemController::workspace_view_resized()
{
    // update always needed sizes
    m_viewport_height = m_workspace_ctrl->viewport_height();
    view->setWidth(m_workspace_ctrl->viewport_width());

    view->update_layout();
}

void RootItemController::view_position_changed(QPointF new_pos)
{
    // prepare update to boundingRect (depends on m_y)
    m_y = new_pos.y();
}

void RootItemController::before_adding_n(int n)
{
    view->set_future_count(n);
}

void RootItemController::layout_updated()
{
    m_workspace_ctrl->set_scene_rect_to_items_bounding_rect();
}

void RootItemController::before_layout_update(RootItem::Layout layout)
{
    if (layout == RootItem::GRAPH) {
        // update layout-dependant sizes
        view->setHeight(m_viewport_height - m_y);
    }
}

/**
 * @brief Called when the selection model changes.
 *
 * @param added Indices removed from selection.
 *
 * @param removed Indices added to selection.
 */
void RootItemController::sel_model_changed(QSet<int> added,
                                           QSet<int> removed)
{
    int lastAdded = -1;
    foreach (const int &x, added) {
        view->fileNodes[x]->setSelected(true);
        lastAdded = x;
    }
    foreach (const int &x, removed) {
        view->fileNodes[x]->setSelected(false);
    }
    // now that only one type of layout is maintained and is visible
    // at a given moment, I think I could remove the visibility
    // check
    if (view->isVisible() && lastAdded != -1) {
        m_workspace_ctrl->ensure_visible(view->fileNodes[lastAdded]);
    }
}
