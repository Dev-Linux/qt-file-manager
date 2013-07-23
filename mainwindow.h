#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#ifdef Q_OS_WIN32
#define ROOT_PATH ("")
#else
#define ROOT_PATH ("/")
#endif

class WorkspaceView;
class Breadcrumb;
class SearchLineEdit;
class LocationEdit;
class FileOperationsMenu;
class FileOperationData;
class FileOperationItem;
class AsyncFileOperation;
class DirController;
class WorkspaceController;
class DockController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    FileOperationsMenu *fileOperationsMenu;
    QPushButton *fileOperationsButton;
    DockController *dock_ctrl;
    QPushButton *layout_button;
    LocationEdit *locationEdit;
    QStackedWidget *stackedWidget;
    Breadcrumb *breadcrumb;
    SearchLineEdit *searchLineEdit;

    DirController *dirCtrl;
    WorkspaceController *workspace_ctrl;

    //std::random_device randomDevice;

signals:
    void layout_button_clicked();
    void breadcrumb_clicked();
    void breadcrumb_path_changed(const QString &path);
    void dir_ctrl_path_changed(const QString &path);
    void dir_search_started(const QString &str);
    void location_edit_focused(bool focused);
    void location_edit_return_pressed();
    void location_edit_button_clicked();
    void search_text_edited(const QString &text);

public slots:

private:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QToolBar *toolBar, *searchToolBar;
    void connect_slots();
};

#endif // MAINWINDOW_H
