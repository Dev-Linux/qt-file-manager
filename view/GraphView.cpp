#include "GraphView.h"

#include "mainwindow.h"
#include "DirModel.h"
#include "RootItem.h"
#include "filenode.h"
#include "ViewSelectionModel.h"

//#include <random>

// http://qt-project.org/forums/viewthread/22993/
// https://bugreports.qt-project.org/browse/QTBUG-29331
// http://stackoverflow.com/questions/15056834/qt5-migration-and-boost-if-hpp-macro-argument-mismatch-bug
#ifndef Q_MOC_RUN

#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/point_traits.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>

#endif // Q_MOC_RUN

#include "fileinfo.h"
#include "misc.h"
#include "TabBarItem.h"

GraphView::GraphView(DirModel *model, DockModel *dockModel) :
    QGraphicsView()
{
    scene = new QGraphicsScene();
    //bgRect = scene->addRect(QRectF(0, 0, 0, 0),
    //               QPen(Qt::NoPen), QBrush(QColor("lightyellow")));
    //scene->setBackgroundBrush(palette().midlight());

    // better is the default (center)
    //setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // by default, true:
    //setInteractive(true);

    //w = new GraphViewWidget(model, this);
    //scene->addWidget(w);

    //resizeTimer.setSingleShot(true);

    tabBarItem = new TabBarItem(this);
    tabBarItem->setPos(0, 0);

    connect(model, &DirModel::pathChanged,
            tabBarItem, &TabBarItem::modelPathChanged);

    scene->addItem(tabBarItem);

    rootItem = new RootItem(model, this, dockModel);
    auto tabBarRect = tabBarItem->boundingRect();
    auto tabBarSceneRect =
            tabBarItem->mapToScene(tabBarRect).boundingRect();
    rootItem->setPos(QPointF(0, tabBarSceneRect.height()));

    scene->addItem(rootItem);
    //setWidgetResizable(true);
    this->model = model;
    setAcceptDrops(true);
    setScene(scene);

    //setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    //setRubberBandSelectionMode(Qt::ContainsItemShape);
    setDragMode(QGraphicsView::RubberBandDrag);
    //connect(&resizeTimer, &QTimer::timeout,
    //        this, &GraphView::resizeTimeout);

    connect(this, &GraphView::rubberBandChanged,
            rootItem, &RootItem::selRectChanged);
}

/*void GraphView::resizeTimeout()
{
    rootItem->refreshPos();
    setSceneRect(scene->itemsBoundingRect());
}*/

void GraphView::resizeEvent(QResizeEvent *evt)
{
    //auto sh = sizeHint2();
    //scene->setSceneRect(QRect(0, 0, sh.width(), sh.height()));
    //scene->setSceneRect(scene->itemsBoundingRect());

    //resizeTimer.start(25);
    rootItem->viewResized();
    setSceneRect(rootItem->boundingRect());
    QGraphicsView::resizeEvent(evt);
}

// calculates the sizeHint (which through a SizePolicy set in the ctor is also
// the minimum size of the widget)
/*QSize GraphView::sizeHint2() const
{
    int c = fileNodes.count();

    //if there are no file nodes, the sizeHint/minimum size is (0,0)
    if (c == 0) {
        return QSize(0, 0);
    }

    // by going through every file node and making comparisons, get the bigges x
    // and y of the file nodes which are the coords which would be a big enough
    // size to fit all the nodes
    int maxx = 0, maxy = 0;
    for (int i = 0; i < c; i++) {
        if (fileNodes[i]->x() > maxx) {
            maxx = fileNodes[i]->x();
        }
        if (fileNodes[i]->y() > maxy) {
            maxy = fileNodes[i]->y();
        }
    }
    // ?
    qDebug() << "width" << width() << "maxx + 100" << maxx + 100;
    qDebug() << "height" << height() << "maxy + 100" << maxy + 100;
    QSize hint(qMax(width(), maxx + 100),
               qMax(height(), maxy + 100));
    return hint;
}

bool GraphView::randomTry(int &x, int &y)
{
    // the thing from which we get random numbers (used for repeaded trying
    // of random coordinates)
    std::random_device rd;

    // if the file node with the current coordinates overlaps with other
    // existing file nodes, or it gets outside of the view with it's
    // (100, 100) size
    int olc = 0;
    bool overlaps;
    do {
        if (olc == 10) break;
        // initially, everything's fine
        overlaps = false;

        // these rd() expressions return random x and y
        x = rd() % width() + 1;
        y = rd() % height() + 1;

        // if it escapes the view
        if (width() - x < 100 || height() - y < 100) {
            overlaps = true;
        } / *else {
            // the rect of the file node with the current coordinates
            QRect r(x, y, 100, 100);

            // for each existing file node, verify if it doesn't overlap
            // with the current file node
            foreach (FileNode* crtNode, fileNodes) {
                if (r.intersects(crtNode->frameGeometry())) {
                    overlaps = true;
                    break;
                }
            }
        }* /
        if (overlaps) {
            olc++;
        }
    } while (overlaps);

    if (overlaps) return false;
    return true;
}*/

void GraphView::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();

        // items under cursor
        auto items = scene->items(mapToScene(dragStartPosition));

        // nodes under cursor (rootItem has only nodes as children)
        items = misc::filterByAncestor(items, rootItem);

        if (items.isEmpty()) {
            draggedNode = nullptr;

            // queueing event because of the sel rect posibility
            movedWhileSelRectPossible = false;
            //QGraphicsView::mousePressEvent(event);
        } else {
            // don't change selection with sel square
            this->setDragMode(QGraphicsView::NoDrag);

            // the item directly under the cursor
            draggedNode = dynamic_cast<FileNode*>(items.first());

            items = scene->items(draggedNode->
                                    mapToScene(draggedNode->shape()));

            // items intersecting draggedNode
            items = misc::filterByAncestor(items, rootItem);
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
        }
        mousePressEvt = event;
        // event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

// FIXME: clicking on the itersection of two nodes selects the
// node behind
// NOTE: check zoom out behavior on RootItem space
// NOTE: idea: moving/copying shows progress on file node (background
// eventually green, just like a progress bar). when moving, the source
// and the target have different progress direction (source, left;
// target, right).

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    if (draggedNode != nullptr) {
        if (!movedWhileDragPossible) {
            if (mousePressEvt != nullptr) {
                QGraphicsView::mousePressEvent(mousePressEvt);
            } else {
                qDebug() << "nullptr mousePressEvt";
            }
        }
        draggedNode = nullptr;
    } else {
        if (!movedWhileSelRectPossible) {
            if (mousePressEvt != nullptr) {
                if (!model->sel->isEmpty()) {
                    model->sel->clear();
                    model->sel->save();
                }
                QGraphicsView::mousePressEvent(mousePressEvt);
            } else {
                qDebug() << "nullptr mousePressEvt 2";
            }
        }
    }
    setDragMode(QGraphicsView::RubberBandDrag);
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
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
        QGraphicsView::mouseMoveEvent(event); // sel rect, drag possible
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

    qDebug() << "---";
    qDebug() << draggedNode->fileInfo.fileName();

    // change drag selection
    int index = rootItem->fileNodes.indexOf(draggedNode);
    bool draggedNodeWasSelected = model->selected(index);

    if (!draggedNodeWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    // calculate deltas for all selected nodes
    selDeltas.clear();
    foreach (int index, model->sel->savedSet) {
        auto node = rootItem->fileNodes[index];
        qDebug() << "- " << node->fileInfo.fileName();
        auto delta = node->mapFromScene(
                        mapToScene(dragStartPosition));
        selDeltas[index] = delta;
    }

    // stack selection over all other nodes
    QHashIterator<int, QPointF> it(selDeltas);
    qreal z = rootItem->fileNodes.size() + 5;
    while (it.hasNext()) {
        it.next();
        //qDebug() << "z value" << z;
        // TODO: Buggy Z, Buggy.
        rootItem->fileNodes[it.key()]->setZValue(z);
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

    // make the sel rect available again
    setDragMode(QGraphicsView::RubberBandDrag);

    event->accept();

    // TODO: if mouse press is followed by a dragging move and the node is selected
    // don't unselect in the slots called by FileNode signal leftClicked(modifiers)

    Q_UNUSED(dropAction);
    // draggedNode->icon->pixmap(64, 64)
}

void GraphView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        drag->setDragCursor(QPixmap(), event->proposedAction());
        event->acceptProposedAction();
    }
}

void GraphView::dragMoveEvent(QDragMoveEvent *event)
{
    QPointF p = event->pos();
    QPointF scenep = mapToScene(p.x(), p.y());

    QHashIterator<int, QPointF> it(selDeltas);
    while (it.hasNext()) {
        it.next();
        int index = it.key();
        const QPointF &delta = it.value();

        QPointF fin = scenep - delta;

        rootItem->fileNodes[index]->setPos(fin);
    }
}

void GraphView::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event);
}

void GraphView::dropEvent(QDropEvent *event)
{
    QPointF p = event->pos();
    auto scenep = mapToScene(p.x(), p.y());
    QHashIterator<int, QPointF> it(selDeltas);
    while (it.hasNext()) {
        it.next();
        int index = it.key();
        auto delta = it.value();
        rootItem->fileNodes[index]->setPos(scenep - delta);
    }

    setSceneRect(scene->itemsBoundingRect());

    auto items = scene->items(draggedNode->mapToScene(draggedNode->shape()));
    items.removeOne(draggedNode);
    if (!items.isEmpty()) {
        draggedNode->setZValue(items[0]->zValue() + 1);
    }

    //draggedNode = nullptr;

    event->acceptProposedAction();
}

void GraphView::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
    if (event->modifiers().testFlag(Qt::NoModifier) &&
            event->key() == Qt::Key_Escape) {
        model->sel->clear();
        model->sel->save();
        event->accept();
    }
    QGraphicsView::keyPressEvent(event);
}
