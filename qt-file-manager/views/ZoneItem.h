#pragma once
#ifndef ZONEITEM_H
#define ZONEITEM_H

#include <QtWidgets>

class ZoneItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ZoneItem();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0);
    QPen border_pen;

signals:
    
public slots:
    
};

#endif // ZONEITEM_H
