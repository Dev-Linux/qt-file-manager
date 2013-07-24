#include "RootItemController.h"
#include "RootItem.h"

#include "DirModel.h"
#include "ViewSelectionModel.h"
#include "DockModel.h"
#include "WorkspaceView.h"

#include "FileNode.h"
#include "misc.h"

RootItemController::RootItemController(DirModel* dir_model,
                                       WorkspaceView *workspace_view,
                                       DockModel *dock_model) :
    QObject()
{
    m_dock_model = dock_model;
    m_dir_model = dir_model;
    m_workspace_view = workspace_view;
    view = new RootItem(dir_model, workspace_view);

    connect(dock_model, &DockModel::added,
            this, &RootItemController::important_added);
    connect(dock_model, &DockModel::removed,
            this, &RootItemController::important_removed);

    connect(dir_model, &DirModel::pathChanged,
            this, &RootItemController::path_changed);
    connect(dir_model, &DirModel::nameFiltersChanged,
            this, &RootItemController::name_filters_changed);
    connect(dir_model->sel, &ViewSelectionModel::changed,
            view, &RootItem::selModelChanged);
    connect(dir_model, &DirModel::cleared,
            view, &RootItem::clearView);
    connect(dir_model, &DirModel::added,
            view, &RootItem::addNode);
    connect(dir_model, &DirModel::file_system_change,
            this, &RootItemController::file_system_change);

    connect(dir_model, &DirModel::tagAdded,
            this, &RootItemController::dir_model_tag_added);
    connect(dir_model, &DirModel::tagRemoved,
            this, &RootItemController::dir_model_tag_removed);
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
    m_workspace_view->setSceneRect(m_workspace_view->scene->itemsBoundingRect());
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
    m_workspace_view->setSceneRect(m_workspace_view->
                                   scene->itemsBoundingRect());
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
