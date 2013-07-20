#pragma once
#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QtWidgets>

class DirModel;
class RootItem;
class FileNode;
class DockModel;
class TabBarItem;

class GraphView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphView(DirModel *model,
                       DockModel *dockModel);
    QGraphicsScene *scene;
    QPoint dragStartPosition;

    RootItem *rootItem;
    TabBarItem *tabBarItem;
    FileNode *draggedNode = nullptr;
    QDrag *drag;
    DirModel *model;

    //bool randomTry(int &x, int &y);
    // TODO: if item is marked important, highlight it in some way

signals:

public slots:
    //void resizeTimeout();
    void zoomIn() { scale(1.2, 1.2); }
    void zoomOut() { scale(1 / 1.2, 1 / 1.2); }

protected:
    virtual void resizeEvent(QResizeEvent *);
    //virtual QSize sizeHint2() const;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    bool movedWhileDragPossible;
    bool movedWhileSelRectPossible;
    QHash<int, QPointF> selDeltas;
    QMouseEvent *mousePressEvt = nullptr;
    //QTimer resizeTimer;
    //QPointF dragDelta;
};

#endif // GRAPHVIEW_H
