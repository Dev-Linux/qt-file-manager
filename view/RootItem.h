#pragma once
#ifndef ROOTITEM_H
#define ROOTITEM_H

#include <QtWidgets>

class DirModel;
class WorkspaceView;
class BoostGraph;
class FileNode;
class DockModel;
class FileInfo;

class RootItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RootItem(DirModel* model,
                      WorkspaceView *workspace_view);
    ~RootItem();
    DirModel *m_model;
    //! The GraphView which contains this RootItem.
    WorkspaceView *workspace_view;
    //! The file nodes in this RootItem.
    QList<FileNode *> fileNodes;

    /** RootItem and FileNode layout types */
    enum Layout {
        //! List layout
        LIST,
        //! Graph-like (auto) layout
        GRAPH
    };

    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void viewResized();
    void setLayout(Layout l);
    void update_layout();
    
signals:
    
public slots:
    void clearView();
    void addNode(const FileInfo &info);
    void itemDoubleClicked();
    void nodeLeftClicked(const Qt::KeyboardModifiers &modifiers);
    void nodeRightClicked(const Qt::KeyboardModifiers &modifiers);
    void contextMenuTriggered(QAction *action);
    void selRectChanged(QRect rubberBandRect, QPointF fromScenePoint,
                        QPointF toScenePoint);
    void selModelChanged(QSet<int> added, QSet<int> removed);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private slots:
    void connectNode(const FileNode *node);

private:
    void initGraph();

    void refresh_list_pos_and_sizes();
    void refresh_graph_positions();

    void setWidth(qreal w);
    void setHeight(qreal h);
    qreal width() const;
    qreal height() const;

    //void loadAllFilesInModel();

    //! Caches for bounding rect calculation.
    qreal m_width, m_height, m_viewport_height, m_y;
    //! Cache.
    mutable QRectF m_bounding_rect;
    //! The initial layout is GRAPH.
    Layout layout = GRAPH;
    //! The context menu.
    QMenu *menu = nullptr;
    //! The graph for GRAPH layout.
    QScopedPointer<BoostGraph> g;
};

#endif // ROOTITEM_H
