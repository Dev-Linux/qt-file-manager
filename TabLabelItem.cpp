#include "TabLabelItem.h"

#include "misc.h"
#include <QtWidgets>

TabLabelItem::TabLabelItem(const QString &label) :
    QGraphicsObject()
{
    if (!label.isEmpty()) {
        this->label = label;
    }
    setAcceptHoverEvents(true);
}

void TabLabelItem::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QBrush bgBrush;
    if (isActive()) {
        if (property("hovered").toBool()) {
            bgBrush = QBrush("lightblue");
        } else {
            bgBrush = QBrush("lightcyan");
        }
    } else {
        if (property("hovered").toBool()) {
            bgBrush = QBrush("khaki");
        } else {
            bgBrush = QBrush("honeydew");
        }
    }
    painter->setBrush(bgBrush);
    painter->setPen(QPen(QBrush("gray"), 0.5));
    painter->drawRect(misc::unpadRect(option->rect, 0.5));

    painter->setPen(QPen(option->palette.text(), 1));
    painter->drawText(this->padding,
                      this->padding + option->fontMetrics.ascent(),
                      label);
}

QRectF TabLabelItem::boundingRect() const
{
    const auto fm = QApplication::fontMetrics();
    const int ascent = fm.ascent();
    const qreal dp = 2 * padding;

    auto rect = fm.boundingRect(this->label);

    rect.setTop(rect.top() + ascent);
    rect.setWidth(rect.width() + dp);
    rect.setHeight(rect.height() + dp + ascent);

    return rect;
}

void TabLabelItem::setLabel(const QString &label)
{
    this->label = label;
    update();
}

void TabLabelItem::setActive(bool active)
{
    setProperty("active", active);
    update();
}

bool TabLabelItem::isActive() const
{
    return property("active").toBool();
}

int TabLabelItem::expectedHeight()
{
    const auto fm = QApplication::fontMetrics();
    const int dp = 2 * padding;
    const int fh = fm.height();

    return fh + dp;
}

void TabLabelItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setProperty("hovered", true);
    update();
}

void TabLabelItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setProperty("hovered", false);
    update();
}

void TabLabelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit leftClicked();
    }
}
