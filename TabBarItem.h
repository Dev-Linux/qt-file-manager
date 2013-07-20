#pragma once
#ifndef TABBARITEM_H
#define TABBARITEM_H

#include <QGraphicsObject>

class GraphView;
class TabLabelItem;

class TabBarItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit TabBarItem(GraphView *view);
    ~TabBarItem();
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QRectF boundingRect() const;
    void addTabLabel(const QString &path,
                     bool active);
    void setActiveIndex(int index);

signals:
    
public slots:
    void modelPathChanged(const QString &path);
private:
    GraphView *view;
    QList<TabLabelItem*> tabLabels;
    int activeIndex = -1;
    int padding = 5;
    int spacing = 5;
    QScopedPointer<TabLabelItem> addButton;

    void repositionTabLabels();
};

#endif // TABBARITEM_H
