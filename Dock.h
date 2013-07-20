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
    explicit Dock(QWidget *parent = 0);
    DockModel *model;
    ListViewItem *selectedItem;
    QMenu *menu = nullptr;
    View *view;
    QMenu *buildMenu();
    
protected:
    virtual QSize sizeHint() const;
signals:
    
public slots:
    void removeItem(FileInfo &info);
    void addItem(const FileInfo &info);
    void itemRightClicked(const QPoint &globalPos);
    void itemDoubleClicked();
    void removeActionTriggered();
};

#endif // DOCK_H
