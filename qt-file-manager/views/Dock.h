#pragma once
#ifndef DOCK_H
#define DOCK_H

#include <QtWidgets>

class View;
class ListViewItem;
class DockModel;
class FileInfo;

class Dock : public QDockWidget
{
    Q_OBJECT
public:
    explicit Dock();

    ListViewItem *selected_item;
    View *view;

signals:
    void remove_action_triggered();

public slots:
    void remove_item(FileInfo &info);
    void item_right_clicked(const QPoint &globalPos);
    void item_double_clicked();

protected:
    virtual QSize sizeHint() const;

private:
    QMenu *build_menu();
    QMenu *m_menu = nullptr;
};

#endif // DOCK_H
