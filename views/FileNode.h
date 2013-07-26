#pragma once
#ifndef FILENODE_H
#define FILENODE_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QFont>
#include <QSize>

#include "FileInfo.h"

// include this just for an enum? search the web!
// well, it's an enum from class RootItem...
#include "RootItem.h"

class FileNode : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit FileNode(const FileInfo &info,
                      RootItem::Layout layout,
                      int line_height);

    void set_selected(bool selected);
    bool is_selected() const;

    bool is_highlighted() const;
    void set_layout(RootItem::Layout layout);

    void set_list_width(qreal w);
    qreal list_height() const;

    static QSize node_size;
    FileInfo file_info;
    QPixmap icon;

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

signals:
    void double_clicked();
    void left_clicked(const Qt::KeyboardModifiers &modifiers);
    void right_clicked(const Qt::KeyboardModifiers &modifiers);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

public slots:
    void set_highlighted(bool set = true);

private:
    void update_icon_size();

    QSize m_list_size;
    QSize m_icon_size;
    mutable QRectF m_bounding_rect_cache; // cache
    bool m_must_update_icon;
    RootItem::Layout m_layout;
    int m_padding = 6;
    int m_spacing = 5;

    static const QFont m_icon_label_font;
    static QSize m_node_icon_size;
};

#endif // FILENODE_H
