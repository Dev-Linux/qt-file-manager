#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#ifdef Q_OS_WIN32
#define ROOT_PATH ("")
#else
#define ROOT_PATH ("/")
#endif

class GraphView;
class Breadcrumb;
class Dock;
class SearchLineEdit;
class LocationEdit;
class FileOperationsMenu;
class FileOperationData;
class FileOperationItem;
class AsyncFileOperation;
class DirController;

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
    DirController *dirCtrl;
    QSettings *settings;
    QTimer searchTimer;
    QString searchBuf, savedSearchBuf;
    QDateTime lastSearchKey;
    QHash<const FileOperationData *, FileOperationItem *> *fileOperations;
    //std::random_device randomDevice;
public slots:
    void locationEditChanged();
    void toggleBreadcrumb();
    void locationEditFocused(bool focused);
    void breadcrumbPathChanged(QString path);

    // file tasks
    void doneAsync(FileOperationData *data);
    void fileOperationProgress(FileOperationData *data);

    void viewPathChanged(const QString &path);
    void dirSearchStarted(const QString &str);
    void searchTextEdited(const QString &str);

    void viewCheckBoxChanged();

    void doSearch();
private:
    static MainWindow* instance;
    Breadcrumb *breadcrumb;
    LocationEdit *locationEdit;
    QStackedWidget *stackedWidget;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QToolBar *toolBar, *searchToolBar;
    SearchLineEdit *searchLineEdit;
    GraphView *graphView;
    void connectSlots();
    MainWindow(const QString &initialPath); // it's a singleton so it's private
};

#endif // MAINWINDOW_H
