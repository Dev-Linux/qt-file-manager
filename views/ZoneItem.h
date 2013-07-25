#pragma once
#ifndef ZONEITEM_H
#define ZONEITEM_H

#include <QtWidgets>

class ZoneItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ZoneItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);
    QPen borderPen;
signals:
    
public slots:
    
};

#endif // ZONEITEM_H
