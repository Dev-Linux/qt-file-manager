#pragma once
#ifndef FILEOPERATIONSMENU_H
#define FILEOPERATIONSMENU_H

#include <QtWidgets>

class FileOperationItem;

class FileOperationsMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FileOperationsMenu(QWidget *parent = 0);
    FileOperationItem* addItem(FileOperationItem* item);

signals:
    
public slots:

protected:
    virtual QSize sizeHint() const;
    virtual void showEvent(QShowEvent *event);

private:
    QVBoxLayout *layout;
    QLabel *emptyListLabel;
    QWidgetAction *action;
    QWidget *widget;
};

#endif // FILEOPERATIONSMENU_H
