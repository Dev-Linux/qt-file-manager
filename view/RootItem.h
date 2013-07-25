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
    explicit RootItem();
    ~RootItem();

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

    void setWidth(qreal w);
    void setHeight(qreal h);
    qreal width() const;
    qreal height() const;
    void set_future_count(int n);
    
signals:
    void node_dbl_clicked(FileNode *node);
    void node_left_clicked(FileNode *node,
                           const Qt::KeyboardModifiers &modifiers);
    void node_right_clicked(FileNode *node,
                            const Qt::KeyboardModifiers &modifiers);
    void position_changed(QPointF new_pos);
    void before_layout_update(Layout layout);
    void layout_updated();
    
public slots:
    void clearView();
    void addNode(const FileInfo &info);

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

private slots:

private:
    void initGraph();
    void connectNode(FileNode *node);

    void refresh_list_pos_and_sizes();
    void refresh_graph_positions();

    //void loadAllFilesInModel();

    //! Caches for bounding rect calculation.
    qreal m_width, m_height;
    // = 0 needed here because otherwise initGraph get's called before
    // it's initialized with model's count through before_adding_n()
    // and it will start creating a graph with infinite vertices.
    int m_future_count = 0;
    //! Cache.
    mutable QRectF m_bounding_rect;
    //! The initial layout is GRAPH.
    Layout layout = GRAPH;
    //! The graph for GRAPH layout.
    QScopedPointer<BoostGraph> g;
};

#endif // ROOTITEM_H
