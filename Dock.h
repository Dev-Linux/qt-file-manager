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
    explicit Dock(DockModel *dock_model);
    DockModel *model;
    ListViewItem *selectedItem;
    View *view;

signals:

public slots:
    void removeItem(FileInfo &info);
    void itemRightClicked(const QPoint &globalPos);
    void itemDoubleClicked();
    void removeActionTriggered();

protected:
    virtual QSize sizeHint() const;

private:
    QMenu *buildMenu();
    QMenu *menu = nullptr;
};

#endif // DOCK_H
