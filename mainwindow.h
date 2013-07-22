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
    ~MainWindow();
    static MainWindow *getInstance(const QString &initialPath = QString());
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
    QTimer searchTimer;
    QString searchBuf, savedSearchBuf;
    QDateTime lastSearchKey;
    QHash<const FileOperationData *, FileOperationItem *> *fileOperations;
    //std::random_device randomDevice;

signals:
    void layout_button_clicked();
    void breadcrumb_clicked();

public slots:
    void locationEditChanged();
    void locationEditFocused(bool focused);
    void breadcrumbPathChanged(QString path);

    // file tasks
    void doneAsync(FileOperationData *data);
    void fileOperationProgress(FileOperationData *data);

    void viewPathChanged(const QString &path);
    void dirSearchStarted(const QString &str);
    void searchTextEdited(const QString &str);

    void doSearch();
private:
    static MainWindow* instance;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QToolBar *toolBar, *searchToolBar;
    SearchLineEdit *searchLineEdit;
    void connectSlots();
    Breadcrumb *breadcrumb;
    MainWindow(const QString &initialPath); // it's a singleton so it's private
};

#endif // MAINWINDOW_H
