#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QtWidgets>

#include "filesystemmodel.h"
#include "asyncfileoperation.h"

class MainWindow;

class ListView : public QListView
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = 0);
    ~ListView();
    
signals:
    
public slots:
    void aboutToHideContextMenu();
    void deleteTriggered();

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:

private:
    void showMenu(QPoint globalPos);
    AsyncFileOperation *op;
};

#endif // LISTVIEW_H
