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
    explicit WorkspaceView(RootItem *root_item_view);
    QGraphicsScene *scene;
    QPoint dragStartPosition;

    RootItem *root_item_view;
    TabBarItem *tabBarItem;
    FileNode *draggedNode = nullptr;
    QDrag *drag;
    QGraphicsRectItem *sel_rect;
    QSet<int> nodes_to_drag;

signals:
    void resized();
    void sel_rect_changed(QRectF sel_rect);
    void clicked_on_empty_space();
    void before_drag(FileNode *drag_start_node);
    void escape_key_pressed();

public slots:

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void scrollContentsBy(int dx, int dy);

private:
    bool movedWhileDragPossible;
    bool movedWhileSelRectPossible;
    bool m_last_press_is_dbl_click = false;
    QHash<int, QPointF> selDeltas;
    QMouseEvent *mousePressEvt = nullptr;
};

#endif // WORKSPACEVIEW_H
