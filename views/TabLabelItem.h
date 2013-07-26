#pragma once
#ifndef TABLABELITEM_H
#define TABLABELITEM_H

#include <QGraphicsObject>

class TabLabelItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit TabLabelItem(const QString &label = QString());

    void set_label(const QString& label);
    void set_active(bool active = true);
    bool is_active() const;

    static int expected_height();

    virtual void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    virtual QRectF boundingRect() const;
    
signals:
    void left_clicked();
    
public slots:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QString m_label;
    static const int m_padding = 5;
};

#endif // TABLABELITEM_H
