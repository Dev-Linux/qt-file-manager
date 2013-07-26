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
class Dock;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(Dock *dock_view,
               WorkspaceView *workspace_view,
               Breadcrumb *breadcrumb_view);

    FileOperationsMenu *file_ops_menu;
    QPushButton *file_ops_button;
    QPushButton *layout_button;
    LocationEdit *location_edit;
    QStackedWidget *stacked_widget;
    SearchLineEdit *search_line_edit;

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
    void zoom_in_requested();
    void zoom_out_requested();

public slots:

private:
    QWidget *m_central_widget;
    QVBoxLayout *m_vertical_layout;
    QToolBar *tool_bar, *search_tool_bar;
    QPushButton *zoom_in_button, *zoom_out_button;
    void connect_slots();
};

#endif // MAINWINDOW_H
