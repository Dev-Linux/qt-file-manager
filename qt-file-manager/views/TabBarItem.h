#pragma once
#ifndef TABBARITEM_H
#define TABBARITEM_H

#include <QGraphicsObject>

class WorkspaceView;
class TabLabelItem;

class TabBarItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit TabBarItem(WorkspaceView *view);
    ~TabBarItem();

    void add_tab_label(const QString &path,
                       bool active);
    void set_active_index(int index);

    virtual void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    virtual QRectF boundingRect() const;

signals:
    
public slots:
    void model_path_changed(const QString &path);

private:
    WorkspaceView *m_view;
    QList<TabLabelItem*> m_tab_labels;
    int m_active_index = -1;
    const int m_padding = 5;
    const int m_spacing = 5;
    QScopedPointer<TabLabelItem> m_add_button;

    void reposition_tab_labels();
};

#endif // TABBARITEM_H
