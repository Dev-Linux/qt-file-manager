#pragma once
#ifndef TABLABELITEM_H
#define TABLABELITEM_H

#include <QGraphicsObject>

class TabLabelItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit TabLabelItem(const QString &label = QString());

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QRectF boundingRect() const;

    void setLabel(const QString& label);
    void setActive(bool active = true);
    bool isActive() const;

    static int expectedHeight();
    
signals:
    void leftClicked();
    
public slots:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QString label;
    static const int padding = 5;
};

#endif // TABLABELITEM_H
