#pragma once
#ifndef VIEW_H
#define VIEW_H

#include <QtWidgets>

/**
 * @bug MSVC (cl.exe) high CPU usage (over 50%) (disabled jom because it was
 * running 2 instances of cl.exe at the same time ... bad)
 */

class DirController;
class ViewWidget;
class ViewScrollArea;
class AsyncFileOperation;
class ListViewItem;

class View : public QWidget
{
    Q_OBJECT
public:
    explicit View(QWidget *parent = 0);

    ListViewItem *item_at(int index);
    int index_of(ListViewItem *item);
    void add_item(ListViewItem *item);
    void remove_item(int index);
    int item_count() const;

    ViewWidget* w;
    ViewScrollArea *s;
    DirController *ctrl;
    QVBoxLayout *l;

    AsyncFileOperation *op;

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual QSize sizeHint() const;

signals:
    void search(const QString& str);

public slots:

};

#endif // VIEW_H
