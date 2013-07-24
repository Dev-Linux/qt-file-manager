#include "RootItemController.h"
#include "RootItem.h"

#include "DirModel.h"
#include "ViewSelectionModel.h"

#include "WorkspaceView.h"

#include "DockModel.h"

#include "misc.h"

RootItemController::RootItemController(DirModel* dir_model,
                                       WorkspaceView *workspace_view,
                                       DockModel *dock_model) :
    QObject()
{
    m_dock_model = dock_model;
    m_dir_model = dir_model;
    view = new RootItem(dir_model, workspace_view, dock_model);

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
    view->view->setSceneRect(view->view->scene->itemsBoundingRect());
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
    view->view->setSceneRect(view->view->scene->itemsBoundingRect());
    refresh_tags();
}

/**
 * @brief Called when the model encounters a file system change.
 */
void RootItemController::file_system_change()
{
    view->update_layout();
}
