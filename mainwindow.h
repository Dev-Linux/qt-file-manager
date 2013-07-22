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
class Dock;
class SearchLineEdit;
class LocationEdit;
class FileOperationsMenu;
class FileOperationData;
class FileOperationItem;
class AsyncFileOperation;
class DirController;
class WorkspaceController;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(const QString &initialPath);
    ~MainWindow();
    FileOperationsMenu *fileOperationsMenu;
    QPushButton *fileOperationsButton;
    AsyncFileOperation *asyncFileOperation;
    Dock *dock;
    QPushButton *layout_button;
    LocationEdit *locationEdit;
    QStackedWidget *stackedWidget;

    DirController *dirCtrl;
    WorkspaceController *workspace_ctrl;

    QSettings *settings;
    QString savedSearchBuf;
    QDateTime lastSearchKey;
    Breadcrumb *breadcrumb;
    QHash<const FileOperationData *, FileOperationItem *> *fileOperations;
    SearchLineEdit *searchLineEdit;
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
    void file_op_done(FileOperationData *data);
    void file_op_progressed(FileOperationData *data);

public slots:

private:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QToolBar *toolBar, *searchToolBar;
    void connect_slots();
};

#endif // MAINWINDOW_H
