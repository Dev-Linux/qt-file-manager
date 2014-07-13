#ifndef GRAPHVIEWWIDGET_H
#define GRAPHVIEWWIDGET_H

#include <QtWidgets>
#include <random>

#include "DirModel.h"

class GraphView;

class GraphViewWidget : public QObject
{
    Q_OBJECT
public:
    explicit GraphViewWidget(DirModel *model, GraphView *view);

    GraphView *view;

    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

protected:
    virtual QSize sizeHint() const;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void paintEvent(QPaintEvent *);

signals:
    
public slots:

private:
    QPoint dragStartPosition;
};

#endif // GRAPHVIEWWIDGET_H
