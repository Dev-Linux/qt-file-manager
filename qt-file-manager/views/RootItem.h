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
    /** RootItem and FileNode layout types */
    enum Layout {
        //! List layout
        LIST,
        //! Graph-like (auto) layout
        GRAPH
    };

    explicit RootItem();
    ~RootItem();

    void view_resized();
    void set_layout(Layout layout);
    void update_layout();

    void set_width(qreal w);
    void set_height(qreal h);
    qreal width() const;
    qreal height() const;
    void set_future_count(int n);

    //! The file nodes in this RootItem.
    QList<FileNode *> file_nodes;

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    
signals:
    void node_dbl_clicked(FileNode *node);
    void node_left_clicked(FileNode *node,
                           const Qt::KeyboardModifiers &modifiers);
    void node_right_clicked(FileNode *node,
                            const Qt::KeyboardModifiers &modifiers);
    void position_changed(QPointF new_pos);
    void before_layout_update(Layout m_layout);
    void layout_updated();
    
public slots:
    void clear_view();
    void add_node(const FileInfo &info);

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

private slots:

private:
    void init_graph();
    void connect_node(FileNode *node);
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
    Layout m_layout = GRAPH;
    //! The graph for GRAPH layout.
    QScopedPointer<BoostGraph> m_g;
};

#endif // ROOTITEM_H
