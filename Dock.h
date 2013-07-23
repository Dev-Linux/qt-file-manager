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
    ListViewItem *selectedItem;
    View *view;

signals:
    void remove_action_triggered();

public slots:
    void removeItem(FileInfo &info);
    void itemRightClicked(const QPoint &globalPos);
    void itemDoubleClicked();

protected:
    virtual QSize sizeHint() const;

private:
    QMenu *buildMenu();
    QMenu *menu = nullptr;
};

#endif // DOCK_H
