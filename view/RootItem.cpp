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
 * @brief Adds the important tag to files marked as important in the current
 * directory.
 */
void RootItem::refreshTags()
{
    // dock FileInfos in the current folder
    QList<FileInfo> list =
            misc::filterByDirAbsPath(dockModel->list,
                            model->dir.absolutePath());

    QList<FileInfo>::iterator it, i, bi;
    for (it = list.begin(); it != list.end(); ++it) {
        FileInfo &info = *it;
        int index = -1;
        bi = model->list.begin();
        for (i = bi; i != model->list.end(); ++i) {
            if (i->fileInfo == info.fileInfo) {
                index = i - bi;
                break;
            }
        }
        //qDebug() << info << index;
        Q_ASSERT(index != -1);
        model->addTag(index, "important");
    }
}
/**
  * \class RootItem
  * \bug Unusual behavior. Steps to reproduce:
  * 1. clear selection
  * 2. draw a sel rect
  * 3. notice that at least the first selected node by the sel rect (the last node
  * that intersected the sel rect) remains selected although you reduce the sel
  * rect to 0 covered nodes
  */

RootItem::RootItem(DirModel* model,
                   GraphView *view,
                   DockModel *dockModel) :
    QGraphicsObject()
{
    this->model = model;
    this->view = view;
    this->dockModel = dockModel;

    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    /** \todo right click on Breadcrumb => FileNode context menu */

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
        //qDebug() << "model tagAdded";
        if (tag == "important") {
            //qDebug() << "important tagAdded :P";
            auto node = fileNodes[index];
            node->setHighlighted();
        }
    });
    connect(model, &DirModel::tagRemoved,
            [this] (int index, const QString &tag) {
        //qDebug() << "model tagRemoved";
        if (tag == "important") {
            //qDebug() << "important tagRemoved";
            auto node = fileNodes[index];
            node->setHighlighted(false);
        }
    });

    connect(dockModel, &DockModel::added,
            [this] (FileInfo &info) {
        //qDebug() << "dockModel added";
        /** \note == QDir operator == doesn't work... even though they have the
         * same path and absolutePath */
        // NOTE: also compares sort and filter settings
        if (info.absoluteDir().absolutePath() ==
                this->model->dir.absolutePath()) { // TODO: drives on Win?
            qDebug() << "from current dir";
            for (int i = 0; i < this->model->count(); i++) {
                if (this->model->list[i].fileInfo ==
                        info.fileInfo) {
                    qDebug() << "file found";
                    this->model->addTag(i, "important");
                    break;
                }
            }
        }
    });

    /** \todo **Search the web:** Doxygen and lambdas; todo line number. */
    /** \todo I should make my own Dir class for more flexibility. */

    connect(dockModel, &DockModel::removed,
            [this] (FileInfo &info) {
        //qDebug() << "dockModel added";
        /** \note == QDir operator == doesn't work... even though they have the
         * same path and absolutePath */
        // NOTE: also compares sort and filter settings
        if (info.absoluteDir().absolutePath() ==
                this->model->dir.absolutePath()) { // TODO: drives on Win?
            qDebug() << "from current dir";
            for (int i = 0; i < this->model->count(); i++) {
                if (this->model->list[i].fileInfo ==
                        info.fileInfo) {
                    qDebug() << "file found";
                    this->model->removeTag(i, "important");
                    break;
                }
            }
        }
    });

    viewResized();
}

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
    for (auto i = fileNodes.begin(); i != fileNodes.end(); ++i) {
        FileNode *fn = *i; // current file node
        fn->setLayout(l);
    }
    refreshPos();
    if (l == LIST) {
        //qDebug() << "before if";
        const qreal list_bounding_rect_h = fileNodes.last()->pos().y() +
                fileNodes.last()->listHeight();

        qDebug() << list_bounding_rect_h;
        //if (m_height != list_bounding_rect_h) { update anyway, we are changing
        // the layout here
            //qDebug() << "m_height != cp.y()";
            //view->scene->setSceneRect(0,0,1000, 2000);

            m_height = list_bounding_rect_h;
            prepareGeometryChange();
            //update();
            //view->scene->setSceneRect(50, 50, 300, 400);
            view->scene->setSceneRect(view->scene->itemsBoundingRect());
            //update();
            //view->viewport()->update();

            //view->scene->update();
            //view->updateSceneRect(view->scene->sceneRect());

            //view->setSceneRect();
            //qDebug() << "prepareGeometryChange()";
        //}
    } else { // GRAPH
        // handled by refreshPos(). Graph-like view respects the size of the
        // viewport
    }
}

QRectF RootItem::boundingRect() const
{
    m_bounding_rect.setWidth(m_width);
    m_bounding_rect.setHeight(m_height);
    return m_bounding_rect;
}

void RootItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(widget)

    painter->setPen(Qt::NoPen);
    painter->setBrush(option->palette.midlight());
    painter->drawRect(option->rect);
}

void RootItem::viewResized()
{
    this->prepareGeometryChange();
    this->m_viewport_height = view->viewport()->height();

    this->m_width = view->viewport()->width();
    this->m_height = m_viewport_height - m_y;
    // pos() întoarce coordonate ale părintelui dar este ceea ce îmi trebuie

    this->refreshPos();
}

/** \note *random info:* unblocks GUI: QApplication::processEvents(); */

/**
 * @brief Replaced by clearView(), addItem() and refreshPos().
 * @bug Still called by search.
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
 * @brief Observes changes to y coordinate of RootItem and updates private
 * variables used in boundingRect() then calls prepareGeometryChange() which
 * usually calls update() too.
 */
QVariant RootItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                              const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        // prepare update to boundingRect (depends on m_y)
        prepareGeometryChange();
        m_y = value.toPointF().y();
        m_height = m_viewport_height - m_y;
    }
    return QGraphicsObject::itemChange(change, value);
}

/**
 * @brief Creates or updates the BoostGraph @a g as needed.
 */
void RootItem::initGraph()
{
    bool graphRebuilt;
    vertices_size_type count;
    if (g.isNull()) {
        count = model->count();
        g.reset(new BoostGraph(count));
        graphRebuilt = true;
    } else {
        count = model->count();
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
    if (layout == LIST) {
        this->prepareGeometryChange();
        m_height = 0;
    }
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
    //node->hide();
    node->setParentItem(this);
    connectNode(node);
    fileNodes << node;
    //node->setPos();
    if (layout == LIST) {
        prepareGeometryChange();
        m_height += node->listHeight();
    }
}

/**
 * @brief Called when the model's path has changed. Updates FileNode positions,
 * the scene rect shown by the GraphView and refreshes tags using refreshTags().
 * @param path The new path of the DirModel.
 */
void RootItem::pathChanged(const QString &path)
{ Q_UNUSED(path)
    refreshPos();
    view->setSceneRect(view->scene->itemsBoundingRect());
    refreshTags();
}

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
            this->model->sel->clear();
        }
        for (auto i = items.begin(); i != items.end(); ++i) {
            auto node = static_cast<FileNode*>(*i);
            int index = fileNodes.indexOf(node);
            this->model->sel->add(index);
        }
        this->model->sel->save();
    }
}

/**
 * @brief Refreshes the positions of @ref FileNode "FileNodes" according to a
 * changed size of the viewport or a changed layout, and if the layout permits,
 * also changes the size of the RootItem to fit the file nodes.
 */
void RootItem::refreshPos()
{
    int count;
    if (layout == GRAPH) {
        initGraph();
        count = g->verticesCount();
    } else {
        count = model->count();
    }

    if (count > 0) {
        if (layout == GRAPH) {
            position_map pos;
            vertex_iterator vi, ve;
            vertex_descriptor v;
            QRectF br = boundingRect();
            int left, top, right, bottom, w, h,
                x, y, index;

            w = br.width();
            h = br.height();
            left = top = 0;
            right = w - FileNode::node_size.width();
            bottom = h - FileNode::node_size.height();

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
        } else { // if (layout == LIST)
            QPointF cp(0, 0); // current position
            FileNode *fn; // current file node
            for (auto i = fileNodes.begin();
                 i != fileNodes.end(); ++i) {
                fn = *i;
                fn->setPos(cp);
                fn->setListWidth(m_width);
                cp.setY(cp.y() + fn->listHeight());
            }
        } /** \todo grid layout */
    }
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

void RootItem::connectNode(const FileNode *node)
{
    connect(node, &FileNode::doubleClicked,
            this, &RootItem::itemDoubleClicked);
    connect(node, &FileNode::leftClicked,
            this, &RootItem::nodeLeftClicked);
    connect(node, &FileNode::rightClicked,
            this, &RootItem::nodeRightClicked);
}

void RootItem::itemDoubleClicked()
{
    FileNode *node = qobject_cast<FileNode*>(sender());
    int index = fileNodes.indexOf(node);

    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    if (node->fileInfo.isDir()) {
        const QString &path = node->fileInfo.absoluteFilePath();
        model->setPath(path);
    } else {
        if (misc::openLocalFile(node->fileInfo.absoluteFilePath())) {
            // not implemented actually...
            model->addTag(index, "recent");
        }
    }
}

void RootItem::nodeLeftClicked(const Qt::KeyboardModifiers &modifiers)
{ Q_UNUSED(modifiers)
    qDebug() << "";

    auto clickedItem = qobject_cast<FileNode*>(sender());

    int index = fileNodes.indexOf(clickedItem);

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

void RootItem::nodeRightClicked(const Qt::KeyboardModifiers &modifiers)
{ Q_UNUSED(modifiers)
    auto clickedItem = qobject_cast<FileNode*>(sender());
    int index = fileNodes.indexOf(clickedItem);
    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
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

void RootItem::contextMenuTriggered(QAction *action)
{
    auto mw = MainWindow::getInstance();
    if (action->text() == "It's important") {
        auto list = model->selectedAbsolutePaths(false);
        model->sel->clear();
        model->sel->save();

        QStringListIterator it(list);
        while (it.hasNext()) {
            mw->dock->model->addPath(it.next());
        }
    } else if (action->text() == "Delete this") {
        QStringList pathList = model->selectedAbsolutePaths(true);
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
        QStringList pathList = model->selectedAbsolutePaths(false); // true? this WILL be X-OS

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
