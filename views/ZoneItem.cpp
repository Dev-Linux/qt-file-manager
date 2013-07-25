#include "ZoneItem.h"

#include "misc.h"

ZoneItem::ZoneItem() :
    QGraphicsObject()
{

}

QRectF ZoneItem::boundingRect() const
{
    return QRectF(0, 0, 400, 400);
}

void ZoneItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)
    Q_UNUSED(painter)



    borderPen.setBrush(option->palette.dark());
    painter->setPen(borderPen);

    painter->setBrush(option->palette.light());
    painter->drawRect(misc::unpadRect(option->rect,
                                      borderPen.widthF() / 2));
}
