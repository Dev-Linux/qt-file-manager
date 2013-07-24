#pragma once
#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QtWidgets>

class DirModel;
class RootItem;
class FileNode;
class DockModel;
class TabBarItem;

class WorkspaceView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit WorkspaceView(DirModel *model,
                       DockModel *dockModel);
    QGraphicsScene *scene;
    QPoint dragStartPosition;

    RootItem *root_item_view;
    TabBarItem *tabBarItem;
    FileNode *draggedNode = nullptr;
    QDrag *drag;
    DirModel *model;

signals:

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void scrollContentsBy(int dx, int dy);

private:
    bool movedWhileDragPossible;
    bool movedWhileSelRectPossible;
    QHash<int, QPointF> selDeltas;
    QMouseEvent *mousePressEvt = nullptr;
    //QPointF dragDelta;
};

#endif // WORKSPACEVIEW_H
