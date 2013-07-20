#pragma once
#ifndef FILENODE_H
#define FILENODE_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QFont>
#include <QSize>

#include "fileinfo.h"

#include "RootItem.h" // TODO: just for an enum? search the web
// well, it's an enum from class RootItem...

class FileNode : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit FileNode(const FileInfo &info,
                      RootItem::Layout layout,
                      int line_height);
    FileInfo fileInfo;
    QPixmap icon;
    void setSelected(bool selected);
    void setLayout(RootItem::Layout l);
    void setListWidth(qreal w);
    //void setListHeight(qreal h);
    qreal listHeight() const;
    bool isSelected() const;
    bool isHighlighted() const;

    static QSize node_size;

    QRectF boundingRect() const;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

signals:
    void doubleClicked();
    void leftClicked(const Qt::KeyboardModifiers &modifiers);
    void rightClicked(const Qt::KeyboardModifiers &modifiers);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

public slots:
    void setHighlighted(bool set = true);

private:
    bool must_update_icon;
    RootItem::Layout layout;
    static const QFont iconLabelFont;
    int padding = 6;
    int spacing = 5;
    void update_icon_size();

    QSize list_size;
    QSize icon_size;
    mutable QRectF bounding_rect_cache; // cache
    static QSize node_icon_size;
};

#endif // FILENODE_H
