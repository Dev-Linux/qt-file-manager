#include "RootItem.h"

#include "MainWindow.h"
#include "DirModel.h"
#include "WorkspaceView.h"
#include "BoostGraph.h"
#include "FileNode.h"
#include "ViewSelectionModel.h"
#include "DirController.h"
#include "FileOperationData.h"
#include "FileOperationsMenu.h"

#include "DockController.h"
#include "Dock.h"

#include "View.h"
#include "AsyncFileOperation.h"
#include "FileOperationItem.h"
#include "DockModel.h"
#include "FileInfo.h"
#include <functional>
#include "misc.h"
#include "MainWindowController.h"

#include <QGraphicsScene>

/**
 * @class RootItem
 *
 * @bug Unusual behavior. Steps to reproduce:
 * 1. clear selection
 * 2. draw a sel rect
 * 3. notice that at least the first selected node by the sel rect (the
 * last node that intersected the sel rect) remains selected although
 * you reduce the sel rect to 0 covered nodes.
 *
 * @todo right click on Breadcrumb => FileNode context menu
 *
 * @todo When going "up" in the breadcrumb, scroll to the last position
 * and highlight for a few seconds the folder from which the user
 * comes.
 *
 * @todo migrate all functionality from DirController to WorkspaceView
 *
 * @todo show progress on move, < progress on source, > progress on
 * destination
 *
 * @note **Search the web:** doxygen todo line number.
 *
 * @todo I should make my own Dir class for more flexibility.
 */
RootItem::RootItem() :
    QGraphicsObject()
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

/**
 * @brief Empty. Needed for using QScopedPointer with forward
 * declarations.
 */
RootItem::~RootItem()
{
}

/**
 * @brief Changes the layout of all @ref FileNode "FileNodes", updates
 * their positions and updates the size of the RootItem according to
 * the layout and the @ref FileNode "FileNodes".
 *
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
 * @brief Observes changes to y coordinate of RootItem and updates
 * private variables used in boundingRect() then calls
 * prepareGeometryChange() which usually calls update() too.
 */
QVariant RootItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                              const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionHasChanged) {
        emit position_changed(value.toPointF());

        update_layout();
    }
    return QGraphicsObject::itemChange(change, value);
}

void RootItem::update_layout() {
    emit before_layout_update(layout);
    if (layout == GRAPH) {
        // do layout specific jobs
        this->refresh_graph_positions(); // call even if the layout has not changed

        //view->scene->setSceneRect(this->boundingRect());
        //view->scene->setSceneRect(QRectF());
    } else {
        // update layout-dependant sizes
        if (fileNodes.isEmpty()) {
            setHeight(0);
        } else {
            setHeight(fileNodes.last()->y() + fileNodes.last()->listHeight());

            // do layout specific jobs
            this->refresh_list_pos_and_sizes(); // call only if the layout or the w has changed
        }
    }
    emit layout_updated();
}

/**
 * @brief Removes and destroys all file nodes. If layout == LIST, also
 * sets height to 0 and calls prepareGeometryChange() which usually
 * calls update().
 */
void RootItem::clearView()
{
    m_future_count = 0;
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
 * @brief Creates a FileNode from @a info, adds it to the RootItem and
 * updates it's size.
 *
 * @param info FileInfo corresponding to the file to be represented by
 * the created FileNode.
 */
void RootItem::addNode(const FileInfo &info)
{
    auto node = new FileNode(info, this->layout, 40);
    node->setParentItem(this);
    connectNode(node);
    fileNodes << node;

    if (layout == LIST) {
        setHeight(height() + node->listHeight());
        //update_layout();
    }
}

/**
 * @brief Refreshes the positions of @ref FileNode "FileNodes" according to a
 * changed size of the viewport or a changed layout, and if the layout permits,
 * also changes the size of the RootItem to fit the file nodes.
 */
void RootItem::refresh_list_pos_and_sizes() // LIST only
{
    int count = fileNodes.size();

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
/** @todo grid layout */
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

void RootItem::set_future_count(int n)
{
    m_future_count = n;
}

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
void RootItem::connectNode(FileNode *node)
{
    // inefficient signal forwarding and inline creation of slots
    connect(node, &FileNode::doubleClicked,
            [=] () {
        emit this->node_dbl_clicked(node);
    });
    connect(node, &FileNode::leftClicked,
            [=] (const Qt::KeyboardModifiers &modifiers) {
        emit this->node_left_clicked(node, modifiers);
    });
    connect(node, &FileNode::rightClicked,
            [=] (const Qt::KeyboardModifiers &modifiers) {
        emit this->node_right_clicked(node, modifiers);
    });
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
 * @brief Creates or updates the BoostGraph as needed.
 * @pre m_model valid
 * @post m_model->count() == g->verticesCount()
 */
void RootItem::initGraph()
{
    bool graphRebuilt;
    vertices_size_type count;
    if (g.isNull()) {
        count = m_future_count;
        g.reset(new BoostGraph(count));
        graphRebuilt = true;
    } else {
        count = m_future_count;
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
