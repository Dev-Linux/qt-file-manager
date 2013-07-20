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
    DirModel *model;
    GraphView *view;
    QList<FileNode *> fileNodes;
    QScopedPointer<BoostGraph> g;
    DockModel *dockModel;
    QMenu *menu = nullptr;

    /** RootItem and FileNode layout types */
    enum Layout {
        LIST, /**< List layout */
        GRAPH /**< Graph-like (auto) layout */
    };
    void setLayout(Layout l);

    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    void viewResized();
    
signals:
    
public slots:
    void refreshPos();
    //void loadAllFilesInModel();
    void connectNode(const FileNode *node);

    void itemDoubleClicked();
    void nodeLeftClicked(const Qt::KeyboardModifiers &modifiers);
    void nodeRightClicked(const Qt::KeyboardModifiers &modifiers);
    void contextMenuTriggered(QAction *action);
    void selRectChanged(QRect rubberBandRect, QPointF fromScenePoint,
                        QPointF toScenePoint);
    void selModelChanged(QSet<int> added, QSet<int> removed);
    void refresh();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private slots:
    void clearView();
    void addNode(const FileInfo &info);
    void pathChanged(const QString &path);

private:
    void initGraph();
    void refreshTags();
    qreal m_width, m_height, m_viewport_height, m_y;
    mutable QRectF m_bounding_rect; // cache
    Layout layout = GRAPH;
};

#endif // ROOTITEM_H
