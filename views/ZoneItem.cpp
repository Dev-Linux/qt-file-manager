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
{ Q_UNUSED(widget)
    border_pen.setBrush(option->palette.dark());
    painter->setPen(border_pen);

    painter->setBrush(option->palette.light());
    painter->drawRect(misc::unpad_rect(option->rect,
                                      border_pen.widthF() / 2));
}
