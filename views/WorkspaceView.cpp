#include "WorkspaceView.h"

#include "models/DirModel.h"
#include "models/ViewSelectionModel.h"

#include "controllers/RootItemController.h"

#include "MainWindow.h"
#include "RootItem.h"
#include "FileNode.h"
#include "FileInfo.h"
#include "misc.h"
#include "TabBarItem.h"

/**
 * @class WorkspaceView
 *
 * @todo Check zoom out behavior on RootItem space.
 *
 * @todo Idea: moving/copying shows progress on file node (background
 * eventually green, just like a progress bar). when moving, the source
 * and the target have different progress direction (source, left;
 * target, right).
 *
 * @todo When there is not enough space to show the whole name of a
 * file, the FileNode should put '...' at the end.
 *
 * @bug In RootItem::LIST layout, I'm not sure if it's visible that if
 * the user clicks on the tab bar, the selection is cleared. This is
 * the only way the user can clear the selection, I think, and this is
 * a problem.
 *
 * @bug The resizing method I choose (through signals) can be optimized.
 * Although better architectured for maintainability, it is not as
 * efficient as before and the slow down can be feeled by resizing the
 * window at a speed expected from normal users.
 */

WorkspaceView::WorkspaceView(RootItem *root_item_view) :
    QGraphicsView()
{
    scene = new QGraphicsScene();
    //bgRect = scene->addRect(QRectF(0, 0, 0, 0),
    //               QPen(Qt::NoPen), QBrush(QColor("lightyellow")));
    //scene->setBackgroundBrush(palette().midlight());

    // by default, true:
    //setInteractive(true);

    tabBarItem = new TabBarItem(this);

    /**
     * @bug I think that when the current directory contains more than
     * ~999999 files, some files could cover the TabBarItem. As the z
     * value is changed in the Graph layout while dragging and after.
     */
    // It's a hack but otherwise, when scrolling, RootItem covers
    // TabBarItem.
    tabBarItem->setZValue(999999);

    tabBarItem->setPos(0, 0);

    scene->addItem(tabBarItem);

    this->root_item_view = root_item_view;
    auto tabBarRect = tabBarItem->boundingRect();
    auto tabBarSceneRect =
            tabBarItem->mapToScene(tabBarRect).boundingRect();
    root_item_view->setPos(QPointF(0, tabBarSceneRect.height()));

    scene->addItem(root_item_view);
    //setWidgetResizable(true);
    setAcceptDrops(true);
    setScene(scene);

    //setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // The default value is AlignCenter.
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    sel_rect = new QGraphicsRectItem();

    QColor sel_rect_bg_col = palette().highlight().color();
    sel_rect_bg_col.setAlphaF(0.3);
    QColor sel_rect_pen_col = palette().light().color();

    sel_rect->setBrush(QBrush(sel_rect_bg_col));
    sel_rect->setPen(QPen(sel_rect_pen_col));

    // This way it is below the TabBarItem.
    sel_rect->setZValue(999998);
}

void WorkspaceView::resizeEvent(QResizeEvent *evt)
{
    emit resized();
    QGraphicsView::resizeEvent(evt);
}

void WorkspaceView::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();

        QPointF scene_drag_start_pos = mapToScene(dragStartPosition);
        // items under cursor
        auto items = scene->items(scene_drag_start_pos);

        // nodes under cursor (rootItem has only nodes as children)
        items = misc::filterByAncestor(items, root_item_view);

        if (items.isEmpty()) {
            draggedNode = nullptr;

            // queueing event because of the sel rect posibility
            movedWhileSelRectPossible = false;
            sel_rect->setRect(scene_drag_start_pos.x(),
                              scene_drag_start_pos.y(),
                              0, 0);
            scene->addItem(sel_rect);
            //QGraphicsView::mousePressEvent(event);

            // Not needed because mouseReleaseEvent doesn't post
            // this event again in this case:
            // Update: Why it's still needed and can't make sel rect
            // without it?
            mousePressEvt = event;
        } else {
            // the item directly under the cursor
            draggedNode = dynamic_cast<FileNode*>(items.first());

            items = scene->items(draggedNode->
                                 mapToScene(draggedNode->shape()));

            // items intersecting draggedNode
            items = misc::filterByAncestor(items, root_item_view);
            items.removeOne(draggedNode);

            // stack draggedNode over intersecting nodes
            if (!items.isEmpty()) {
                draggedNode->setZValue(items[0]->zValue() + 1);
            }

            movedWhileDragPossible = false;

            // calls nodeLeftClicked which unselects any other
            // if !ctrl
            //QGraphicsView::mousePressEvent(event);

            // if !move
            //      call nodeLeftClicked
            // else
            //      if move is drag
            //          do drag sel
            //      else
            //          call nodeleftclicked

            mousePressEvt = event;
        }
        //event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void WorkspaceView::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        if (draggedNode != nullptr) {
            // last mouse press was over a FileNode

            if (!movedWhileDragPossible) {
                if (mousePressEvt != nullptr) {
//                    if (m_last_press_is_dbl_click) {
//                        qDebug() << "last press is dbl click";
                        // this will call RootItemController::
                        // node_left_clicked:
                        QGraphicsView::mousePressEvent(mousePressEvt);
//                        m_last_press_is_dbl_click = false;
//                    } else {
//                        emit root_item_view->
//                            node_left_clicked(draggedNode,
//                                              event->modifiers());
//                    }
                } else {
                    // this case shouldn't happen and is not handled
                    Q_ASSERT(false);
                }
            }
            draggedNode = nullptr;
        } else {
            // last mouse press wasn't over a FileNode, it was over
            // empty space

            if (!movedWhileSelRectPossible) {
                emit clicked_on_empty_space();
            } else {
                scene->removeItem(sel_rect);
                emit sel_rect_changed(QRectF());
            }
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void WorkspaceView::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_last_press_is_dbl_click = true;
    QGraphicsView::mouseDoubleClickEvent(event);
}

void WorkspaceView::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();

    if (!event->buttons().testFlag(Qt::LeftButton)) {
        if (mousePressEvt != nullptr) {
            QGraphicsView::mousePressEvent(mousePressEvt);
            mousePressEvt = nullptr;
        }
        // left button not pressed
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    // from now, left button is pressed

    movedWhileSelRectPossible = true;

    if (draggedNode == nullptr) {
        if (mousePressEvt != nullptr) {
            QGraphicsView::mousePressEvent(mousePressEvt);
            mousePressEvt = nullptr;
        }

        // sel rect, drag possible
        QPointF current_pos = mapToScene(event->pos());
        QPointF origin = mapToScene(dragStartPosition);
        QRectF sel_rect_rect;

        if (origin.x() < current_pos.x() ||
                origin.y() < current_pos.y()) {
            sel_rect_rect.setTopLeft(origin);
            sel_rect_rect.setBottomRight(current_pos);
        } else {
            sel_rect_rect.setTopLeft(current_pos);
            sel_rect_rect.setBottomRight(origin);
        }
        sel_rect_rect = sel_rect_rect.normalized();

        sel_rect->setRect(sel_rect_rect);

        emit sel_rect_changed(sel_rect_rect);

        //QGraphicsView::mouseMoveEvent(event);
        return;
    }

    movedWhileDragPossible = true;

    // from now, I have a dragged node

    if ((event->pos() - dragStartPosition).manhattanLength() <
            QApplication::startDragDistance()) {
        if (mousePressEvt != nullptr) {
            QGraphicsView::mousePressEvent(mousePressEvt);
            mousePressEvt = nullptr;
        }
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    // from now, drag distance touched, starting drag

    nodes_to_drag.clear();

    emit before_drag(draggedNode);

    if (nodes_to_drag.isEmpty()) {
        int index = root_item_view->
                fileNodes.indexOf(draggedNode);
        nodes_to_drag.insert(index);
    }

    // calculate deltas for all selected nodes
    selDeltas.clear();
    foreach (int index, nodes_to_drag) {
        auto node = root_item_view->fileNodes[index];
        auto delta = node->mapFromScene(
                        mapToScene(dragStartPosition));
        selDeltas[index] = delta;
    }

    // stack selection over all other nodes
    QHashIterator<int, QPointF> it(selDeltas);
    qreal z = root_item_view->fileNodes.size() + 5;
    while (it.hasNext()) {
        it.next();
        //qDebug() << "z value" << z;
        //! @bug Buggy Z, Buggy.
        root_item_view->fileNodes[it.key()]->setZValue(z);
    }

    // set drag data
    drag = new QDrag(this);
    auto mimeData = new QMimeData();

    //qDebug() << "is loaded" << draggedNode->fileInfo.fileInfo.absoluteFilePath();
    //mimeData->setText(draggedNode->fileInfo.fileName());

    QList<QUrl> urls;
    //urls << QUrl::fromLocalFile(draggedNode->
    //                            fileInfo.absoluteFilePath());
    mimeData->setUrls(urls);

    drag->setMimeData(mimeData);

    // create drag pixmap
    QPixmap x(1, 1);
    QPainter *painter = new QPainter(&x);
    QFontMetrics fm(painter->fontMetrics());
    painter->end();
    delete painter;

    QString s;
    if (event->modifiers() & Qt::ShiftModifier) {
        s = "Move";
    } else if (event->modifiers() & Qt::AltModifier) {
        s = "Link";
    } else {
        s = "Copy";
    }
    int width = fm.width(s) + 4, height = fm.height() + 4;

    QPixmap dragPixmap(width, height);
    painter = new QPainter(&dragPixmap);

    painter->fillRect(0,0,width,height, Qt::white);

    painter->setPen(Qt::black);
    painter->drawText(QPoint(2,2 + fm.ascent()), s);

    painter->setPen(QColor("orange"));
    painter->drawRect(0, 0, width - 1, height - 1);

    painter->end();
    delete painter;

    drag->setPixmap(dragPixmap);

    // superclass m.move handler (IMO this creates the sel rect)
    QGraphicsView::mouseMoveEvent(event);

    Qt::DropAction dropAction = drag->exec();
    // drag finished

    event->accept();

    Q_UNUSED(dropAction);
    // draggedNode->icon->pixmap(64, 64)
}

void WorkspaceView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        drag->setDragCursor(QPixmap(), event->proposedAction());
        event->acceptProposedAction();
    }
}

void WorkspaceView::dragMoveEvent(QDragMoveEvent *event)
{
    QPointF p = event->pos();
    QPointF scenep = mapToScene(p.x(), p.y());

    QHashIterator<int, QPointF> it(selDeltas);
    while (it.hasNext()) {
        it.next();
        int index = it.key();
        const QPointF &delta = it.value();

        QPointF fin = scenep - delta;

        root_item_view->fileNodes[index]->setPos(fin);
    }
}

void WorkspaceView::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
}

void WorkspaceView::dropEvent(QDropEvent *event)
{
    QPointF p = event->pos();
    auto scenep = mapToScene(p.x(), p.y());
    QHashIterator<int, QPointF> it(selDeltas);
    while (it.hasNext()) {
        it.next();
        int index = it.key();
        auto delta = it.value();
        root_item_view->fileNodes[index]->setPos(scenep - delta);
    }

    setSceneRect(scene->itemsBoundingRect());

    auto items = scene->items(draggedNode->
                              mapToScene(draggedNode->shape()));
    items.removeOne(draggedNode);
    if (!items.isEmpty()) {
        draggedNode->setZValue(items[0]->zValue() + 1);
    }

    //draggedNode = nullptr;

    event->acceptProposedAction();
}

void WorkspaceView::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
    if (event->modifiers().testFlag(Qt::NoModifier) &&
            event->key() == Qt::Key_Escape) {
        emit escape_key_pressed();
        event->accept();
    }
    QGraphicsView::keyPressEvent(event);
}

void WorkspaceView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    QPointF p = this->mapToScene(0, 0);
    //metaDebug(p);
    tabBarItem->setPos(p);
}
