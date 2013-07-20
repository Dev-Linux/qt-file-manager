#include "RootItem.h"

#include "mainwindow.h"
#include "DirModel.h"
#include "GraphView.h"
#include "BoostGraph.h"
#include "filenode.h"
#include "ViewSelectionModel.h"
#include "DirController.h"
#include "fileoperationdata.h"
#include "fileoperationsmenu.h"
#include "Dock.h"
#include "View.h"
#include "asyncfileoperation.h"
#include "fileoperationitem.h"
#include "DockModel.h"
#include "fileinfo.h"
#include <functional>
#include "misc.h"

#include <QGraphicsScene>

/**
  * @class RootItem
  * @bug Unusual behavior. Steps to reproduce:
  * 1. clear selection
  * 2. draw a sel rect
  * 3. notice that at least the first selected node by the sel rect (the last node
  * that intersected the sel rect) remains selected although you reduce the sel
  * rect to 0 covered nodes
  * @todo right click on Breadcrumb => FileNode context menu
  */
RootItem::RootItem(DirModel* model,
                   GraphView *view,
                   DockModel *dockModel) :
    QGraphicsObject()
{
    this->m_model = model;
    this->view = view;
    this->dockModel = dockModel;

    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    connect(model, &DirModel::pathChanged,
            this, &RootItem::pathChanged);
    connect(model, &DirModel::nameFiltersChanged,
            this, &RootItem::refresh);
    connect(model->sel, &ViewSelectionModel::changed,
            this, &RootItem::selModelChanged);
    connect(model, &DirModel::cleared,
            this, &RootItem::clearView);
    connect(model, &DirModel::added,
            this, &RootItem::addNode);

    connect(model, &DirModel::tagAdded,
            [this] (int index, const QString &tag) {
        if (tag == "important") {
            auto node = fileNodes[index];
            node->setHighlighted();
        }
    });
    connect(model, &DirModel::tagRemoved,
            [this] (int index, const QString &tag) {
        if (tag == "important") {
            auto node = fileNodes[index];
            node->setHighlighted(false);
        }
    });

    connect(dockModel, &DockModel::added,
            this, &RootItem::importantAdded);

    //! @note **Search the web:** doxygen todo line number.
    //! @todo I should make my own Dir class for more flexibility.

    connect(dockModel, &DockModel::removed,
            this, &RootItem::importantRemoved);

    viewResized();
}

/**
 * @brief Empty. Needed for using QScopedPointer with forward declarations.
 */
RootItem::~RootItem()
{
}

/**
 * @brief Changes the layout of all @ref FileNode "FileNodes", updates their
 * positions and updates the size of the RootItem according to the layout and
 * the @ref FileNode "FileNodes".
 * @param l The new layout.
 */
void RootItem::setLayout(RootItem::Layout l)
{
    this->layout = l;

    FileNode *fn;
    for (auto i = fileNodes.begin(); i != fileNodes.end(); ++i) {
        fn = *i; // current file node
        fn->setLayout(l);
    }

    update_layout();
}

QRectF RootItem::boundingRect() const
{
    m_bounding_rect.setWidth(width());
    m_bounding_rect.setHeight(height());
    return m_bounding_rect;
}

/**
 * @brief Called when the GraphView's size changes and from the ctor.
 * @see
 * - RootItem()
 * - GraphView::resizeEvent()
 */
void RootItem::viewResized()
{
    // update always needed sizes
    this->m_viewport_height = view->viewport()->height();
    setWidth(view->viewport()->width());

    update_layout();
}

/**
 * @brief Observes changes to y coordinate of RootItem and updates private
 * variables used in boundingRect() then calls prepareGeometryChange() which
 * usually calls update() too.
 */
QVariant RootItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                              const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        // prepare update to boundingRect (depends on m_y)
        m_y = value.toPointF().y();

        update_layout();
    }
    return QGraphicsObject::itemChange(change, value);
}

void RootItem::update_layout() {
    if (layout == GRAPH) {
        // update layout-dependant sizes
        setHeight(m_viewport_height - m_y);

        // do layout specific jobs
        this->refresh_graph_positions(); // call even if the layout has not changed

        //view->scene->setSceneRect(this->boundingRect());
        //view->scene->setSceneRect(QRectF());
        view->setSceneRect(view->scene->itemsBoundingRect());
    } else {
        // update layout-dependant sizes
        if (fileNodes.isEmpty()) {
            setHeight(0);
        } else {
            setHeight(fileNodes.last()->y() + fileNodes.last()->listHeight());

            // do layout specific jobs
            this->refresh_list_pos_and_sizes(); // call only if the layout or the w has changed
        }
        view->setSceneRect(view->scene->itemsBoundingRect());
    }
}

/**
 * @brief Removes and destroys all file nodes. If layout == LIST, also sets
 * height to 0 and calls prepareGeometryChange() which usually calls update().
 */
void RootItem::clearView()
{
    // remove the file nodes from the previous calls to setPath
    for (int i = 0; i < fileNodes.length(); i++) {
        delete fileNodes.at(i); // this also destroys the node
        //scene->removeItem(fileNodes[i]);
    }
    // this should also 'delete's the elements but crashes the app
    // (I don't know why)
    fileNodes.clear();

    update_layout();
}

/**
 * @brief Creates a FileNode from @a info, adds it to the RootItem and updates
 * it's size.
 * @param info FileInfo corresponding to the file to be represented by the
 * created FileNode.
 */
void RootItem::addNode(const FileInfo &info)
{
    auto node = new FileNode(info, this->layout, 40);
    node->setParentItem(this);
    connectNode(node);
    fileNodes << node;

    if (layout == LIST) {
        setHeight(m_height + node->listHeight());
        //update_layout();
    }
}

/**
 * @brief Called when the model's path has changed. Updates FileNode positions,
 * the scene rect shown by the GraphView and refreshes tags using refreshTags().
 * @param path The new path of the DirModel.
 */
void RootItem::pathChanged(const QString &path)
{ Q_UNUSED(path)
    update_layout();
    view->setSceneRect(view->scene->itemsBoundingRect());
    refreshTags();
}

/**
 * @brief Refreshes the positions of @ref FileNode "FileNodes" according to a
 * changed size of the viewport or a changed layout, and if the layout permits,
 * also changes the size of the RootItem to fit the file nodes.
 */
void RootItem::refresh_list_pos_and_sizes() // LIST only
{
    int count = m_model->count();

    if (count > 0) {
        QPointF cp(0, 0); // current position
        FileNode *fn; // current file node
        for (auto i = fileNodes.begin();
             i != fileNodes.end(); ++i) {
            fn = *i;
            fn->setPos(cp);
            fn->setListWidth(width());
            cp.setY(cp.y() + fn->listHeight());
        }
    }
}

void RootItem::refresh_graph_positions()
{
    initGraph();
    int count = g->verticesCount();

    if (count > 0) {
        position_map pos;
        vertex_iterator vi, ve;
        vertex_descriptor v;
        int left, top, right, bottom, x, y, index;

        left = top = 0;
        right = width() - FileNode::node_size.width();
        bottom = height() - FileNode::node_size.height();

        g->setRectangle(left, top, right, bottom);
        pos = g->getPos();
        g->doLayout(pos);

        boost::tie(vi, ve) = g->vertices();

        while (vi != ve) {
            v = *vi;

            x = pos[v][0];
            y = pos[v][1];
            index = g->vertexIndex(v);

            auto node = fileNodes[index];
            node->setPos(x, y);

            vi++;
        }
    }
}
/** \todo grid layout */
void RootItem::setWidth(qreal w)
{
    if (m_width != w) {
        prepareGeometryChange();
        m_width = w;
    }
}

void RootItem::setHeight(qreal h)
{
    if (m_height != h) {
        prepareGeometryChange();
        m_height = h;
    }
}

qreal RootItem::width() const
{
    return m_width;
}

qreal RootItem::height() const
{
    return m_height;
}

/** \todo migrate all functionality from DirController to GraphView */
/** \todo show progress on move, < progress on source, > progress on destination */

/*void RootItem::loadAllFilesInModel()
{
    int count = model->count();
    g.reset(new BoostGraph(count));

    if (count > 0) {
        vertex_iterator vi, ve;
        vertices_size_type s;
        QRectF br;
        position_map pos;
        vertex_descriptor v;
        int left, top, right, bottom, w, h,
            x, y, index;

        boost::tie(vi, ve) = g->vertices();

        for (s = 0; s < static_cast<vertices_size_type>(count);
             ++s, ++vi) {
            g->setVertexIndex(*vi, s);
        }

        br = boundingRect();
        w = br.width();
        h = br.height();
        left = top = 0;
        right = w - FileNode::size->width();
        bottom = h - FileNode::size->height();

        g->setRectangle(left, top, right, bottom);
        pos = g->getPos();
        g->doLayout(pos);

        vi = g->vertices().first; // reset iterator

        while (vi != ve) {
            v = *vi;

            x = pos[v][0];
            y = pos[v][1];
            index = g->vertexIndex(v);

            const auto info = model->list[index];
            auto node = new FileNode(info);
            node->setParentItem(this);
            //node->setSelected(model->selected(index));

            connectNode(node);
            fileNodes.append(node);

            node->setPos(x, y);

            ++vi;
        }
    }

    // resize the widget to the size hint (which is also the minimum size)
    //resize(sizeHint());
}*/

/**
 * @brief Connects FileNode signals to RootItem slots.
 * @param node The node to connect with the RootItem.
 */
void RootItem::connectNode(const FileNode *node)
{
    connect(node, &FileNode::doubleClicked,
            this, &RootItem::itemDoubleClicked);
    connect(node, &FileNode::leftClicked,
            this, &RootItem::nodeLeftClicked);
    connect(node, &FileNode::rightClicked,
            this, &RootItem::nodeRightClicked);
}

/**
 * @brief Called when an item was double-clicked.
 */
void RootItem::itemDoubleClicked()
{
    FileNode *node = qobject_cast<FileNode*>(sender());
    int index = fileNodes.indexOf(node);

    bool clickedItemWasSelected = m_model->selected(index);

    if (!clickedItemWasSelected) {
        m_model->sel->clear();
        m_model->sel->add(index);
        m_model->sel->save();
    }

    if (node->fileInfo.isDir()) {
        const QString &path = node->fileInfo.absoluteFilePath();
        m_model->setPath(path);
    } else {
        if (misc::openLocalFile(node->fileInfo.absoluteFilePath())) {
            // not implemented actually...
            m_model->addTag(index, "recent");
        }
    }
}

/**
 * @brief Called when an item was left-clicked.
 * @param modifiers The modifiers pressed at the moment of the mouse button
 * press.
 */
void RootItem::nodeLeftClicked(const Qt::KeyboardModifiers &modifiers)
{ Q_UNUSED(modifiers)
    qDebug() << "";

    auto clickedItem = qobject_cast<FileNode*>(sender());

    int index = fileNodes.indexOf(clickedItem);

    if (!modifiers.testFlag(Qt::ControlModifier)) {
        bool clickedItemWasSelected = m_model->selected(index);
        int selc = m_model->sel->count();
        m_model->sel->clear();
        if (!clickedItemWasSelected || selc > 1) {
            m_model->sel->add(index);
        }
    } else {
        bool clickedItemSelected = m_model->selected(index);
        if (!clickedItemSelected) {
            m_model->sel->add(index);
        } else {
            m_model->sel->remove(index);
        }
    }
    m_model->sel->save();
}

/**
 * @brief Called when an item was right-clicked.
 * @param modifiers The modifiers pressed at the moment of the mouse button
 * press.
 */
void RootItem::nodeRightClicked(const Qt::KeyboardModifiers &modifiers)
{ Q_UNUSED(modifiers)
    auto clickedItem = qobject_cast<FileNode*>(sender());
    int index = fileNodes.indexOf(clickedItem);
    bool clickedItemWasSelected = m_model->selected(index);

    if (!clickedItemWasSelected) {
        m_model->sel->clear();
        m_model->sel->add(index);
        m_model->sel->save();
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

    if (menu != nullptr) {
        delete menu;
    }
    menu = new QMenu();
    connect(menu, &QMenu::triggered,
            this, &RootItem::contextMenuTriggered);
    menu->addAction("It's important");
    menu->addAction("Recycle this");
    menu->addAction("Delete this");
    menu->popup(QCursor::pos());

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
    // TODO: search the web for "mouse move [while] popup menu
    // [open]" or similar...

    /*if (!init) {
        item->setHighlighted(init);
    }*/
}

/**
 * @brief Called when an action from the FileNode context menu was triggered.
 * @param action The triggered action.
 */
void RootItem::contextMenuTriggered(QAction *action)
{
    auto mw = MainWindow::getInstance();
    if (action->text() == "It's important") {
        auto list = m_model->selectedAbsolutePaths(false);
        m_model->sel->clear();
        m_model->sel->save();

        QStringListIterator it(list);
        while (it.hasNext()) {
            mw->dock->model->addPath(it.next());
        }
    } else if (action->text() == "Delete this") {
        QStringList pathList = m_model->selectedAbsolutePaths(true);
        auto data = new FileOperationData("delete", pathList);

        // item handling:
        auto item = new FileOperationItem(data);
        //QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        auto s = pathList.join(",<br>");
        item->setLabel("<strong>" + s + "</strong>.");
        mw->fileOperationsMenu->addItem(item);
        mw->dirCtrl->view->op->doAsync(data, item);
    } else if (action->text() == "Recycle this") {
        QStringList pathList = m_model->selectedAbsolutePaths(false); // true? this WILL be X-OS

        auto data = new FileOperationData("recycle", pathList);

        // item handling:

        auto item = new FileOperationItem(data);
        //QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        //auto s = pathList.join(",<br>");

        // QDir::nativePathSeparators seems unnecessary at least for now,
        // because this if branch is only called on linux whose native
        // separator is the one provided by TrashModel anyway.
        auto s = pathList.join(",<br>");

        // FIXME: view doesn't update on folder change (file recycled)

        item->setLabel("<strong>" + s + "</strong>.");
        mw->fileOperationsMenu->addItem(item);
        mw->dirCtrl->view->op->doAsync(data, item);
    }
}

/**
 * @brief Called when the rubberBandRect or selection rectangle changes. Updates
 * the selection model.
 */
void RootItem::selRectChanged(QRect rubberBandRect,
                              QPointF fromScenePoint,
                              QPointF toScenePoint) {
    Q_UNUSED(fromScenePoint) Q_UNUSED(toScenePoint)
    auto mod = QApplication::keyboardModifiers();

    // items itersecting sel rect
    auto items = view->scene->items(view->mapToScene(rubberBandRect),
                          Qt::IntersectsItemShape);
    // nodes itersecting sel rect (rootItem contains only nodes)
    items = misc::filterByAncestor(items, this);

    if (!items.isEmpty()) {
        if (!mod.testFlag(Qt::ControlModifier)) {
            this->m_model->sel->clear();
        }
        for (auto i = items.begin(); i != items.end(); ++i) {
            auto node = static_cast<FileNode*>(*i);
            int index = fileNodes.indexOf(node);
            this->m_model->sel->add(index);
        }
        this->m_model->sel->save();
    }
}

/**
 * @brief Adds the important tag to files marked as important in the current
 * directory.
 */
void RootItem::refreshTags()
{
    // dock FileInfos in the current folder
    QList<FileInfo> list =
            misc::filterByDirAbsPath(dockModel->list,
                            m_model->dir.absolutePath());

    QList<FileInfo>::iterator it, ei, bi, i;
    for (it = list.begin(); it != list.end(); ++it) {
        FileInfo &info = *it;
        bi = m_model->list.begin();
        ei = m_model->list.end();
        i = qFind(bi, ei, info);

        Q_ASSERT(i != ei);

        const int index = i - bi;

        m_model->addTag(index, "important");
    }
}

/**
 * @brief Paints the background.
 */
void RootItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(widget)

    painter->setPen(Qt::NoPen);
    painter->setBrush(option->palette.midlight());
    painter->drawRect(option->rect);
}

/**
 * @brief Allows qDebugging of FileInfo instances (prints their absolute file
 * path).
 * @param dbg The QDebug instance (returned by calls to qDebug())
 * @param info The FileInfo to output.
 * @return The same QDebug instance @a dbg, for chained calls.
 */
QDebug operator<<(QDebug dbg, const FileInfo &info)
{
    dbg.nospace() << "FileInfo("
                  << info.absoluteFilePath() << ")";
    return dbg.space();
}

/**
 * @brief Called when a folder was marked as important.
 * @param info The file/folder marked as important.
 * @note QDir operator == doesn't work even though they have the
 * same path and absolutePath.
 */
void RootItem::importantAdded(FileInfo &info) {
    // NOTE: also compares sort and filter settings
    if (info.absoluteDir().absolutePath() ==
            m_model->dir.absolutePath()) { //! @todo drives on Win?
        for (int i = 0; i < m_model->count(); i++) {
            if (m_model->list[i] == info) {
                m_model->addTag(i, "important");
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
void RootItem::importantRemoved(FileInfo &info) {
    // NOTE: also compares sort and filter settings
    if (info.absoluteDir().absolutePath() ==
            m_model->dir.absolutePath()) { //! @todo drives on Win?
        for (int i = 0; i < m_model->count(); i++) {
            if (m_model->list[i] == info) {
                m_model->removeTag(i, "important");
                break;
            }
        }
    }
}

/**
 * @brief Replaced by clearView(), addItem() and refreshPos().
 * @bug Still called by search.
 * @deprecated
 */
void RootItem::refresh()
{
    clearView();
    //loadAllFilesInModel();
    Q_ASSERT_X(false, "RootItem::refresh",
               "not sure I should call loadAllFilesInModel");
    view->setSceneRect(view->scene->itemsBoundingRect());
    refreshTags();
}

/**
 * @brief Creates or updates the BoostGraph as needed.
 * @pre m_model valid
 * @post m_model->count() == g->verticesCount()
 */
void RootItem::initGraph()
{
    bool graphRebuilt;
    vertices_size_type count;
    if (g.isNull()) {
        count = m_model->count();
        g.reset(new BoostGraph(count));
        graphRebuilt = true;
    } else {
        count = m_model->count();
        if (g->verticesCount() != count) {
            g.reset(new BoostGraph(count));
            graphRebuilt = true;
        } else {
            graphRebuilt = false;
        }
    }
    if (graphRebuilt) {
        vertex_iterator vi, ve;
        boost::tie(vi, ve) = g->vertices();
        vertices_size_type s;
        for (s = 0; s < count; ++s, ++vi) {
            g->setVertexIndex(*vi, s);
        }
    }
}

/**
 * @brief Called when the selection model changes.
 * @param added Indices removed from selection.
 * @param removed Indices added to selection.
 */
void RootItem::selModelChanged(QSet<int> added, QSet<int> removed) {
    int lastAdded = -1;
    foreach (const int &x, added) {
        fileNodes[x]->setSelected(true);
        lastAdded = x;
    }
    foreach (const int &x, removed) {
        fileNodes[x]->setSelected(false);
    }
    if (isVisible() && lastAdded != -1) {
        view->ensureVisible(fileNodes[lastAdded], 0, 0);
    }
}
