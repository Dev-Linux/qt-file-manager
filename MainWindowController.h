#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include "view/RootItem.h"

class MainWindow;

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowController(const QString &initial_path = QString());
    MainWindow *view;
    
signals:
    
public slots:

private slots:
    void layout_button_clicked();
    void breadcrumb_clicked();

private:
    RootItem::Layout m_layout;
    
};

#endif // MAINWINDOWCONTROLLER_H
