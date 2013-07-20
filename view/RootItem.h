#pragma once
#ifndef ROOTITEM_H
#define ROOTITEM_H

#include <QtWidgets>

class DirModel;
class GraphView;
class BoostGraph;
class FileNode;
class DockModel;
class FileInfo;

class RootItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RootItem(DirModel* model,
                      GraphView *view,
                      DockModel *dockModel);
    ~RootItem();
    DirModel *m_model;
    //! The GraphView which contains this RootItem.
    GraphView *view;
    //! The file nodes in this RootItem.
    QList<FileNode *> fileNodes;
    //! The DockModel which stores the items marked as important.
    DockModel *dockModel;

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
    
signals:
    
public slots:
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
    void clearView();
    void addNode(const FileInfo &info);
    void pathChanged(const QString &path);
    void connectNode(const FileNode *node);

    void importantAdded(FileInfo &info);
    void importantRemoved(FileInfo &info);
    void refresh();
private:
    void initGraph();
    void refreshTags();

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
    void update_layout();
};

#endif // ROOTITEM_H
