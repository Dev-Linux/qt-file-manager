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

    FileOperationItem* add_item(FileOperationItem* item);

signals:
    
public slots:

protected:
    virtual QSize sizeHint() const;
    virtual void showEvent(QShowEvent *event);

private:
    QVBoxLayout *m_layout;
    QLabel *m_empty_list_label;
    QWidgetAction *m_action;
    QWidget *m_widget;
};

#endif // FILEOPERATIONSMENU_H
