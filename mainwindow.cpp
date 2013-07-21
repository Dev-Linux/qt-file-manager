#include "mainwindow.h"

#include <new>
#include "view/GraphView.h"
#include "breadcrumb.h"
#include "Dock.h"
#include "SearchLineEdit.h"
#include "locationedit.h"
#include "fileoperationsmenu.h"
#include "view/DirController.h"
#include "asyncfileoperation.h"
#include "view/View.h"
#include "view/DirModel.h"
#include "fileoperationdata.h"
#include "fileoperationitem.h"
#include "misc.h"
#include "view/RootItem.h"
//#include "filesystemmodel.h"
//#include <random>

/**
 * @brief The singleton instance of MainWindow.
 */
MainWindow* MainWindow::instance;

/**
 * \brief Connects signals to other signals/slots.
 */
void MainWindow::connectSlots()
{
    connect(breadcrumb, &Breadcrumb::clicked,
            this,       &MainWindow::toggleBreadcrumb);
    connect(breadcrumb, &Breadcrumb::pathChanged,
            this,       &MainWindow::breadcrumbPathChanged);

    connect(dirCtrl, &DirController::pathChanged,
            this, &MainWindow::viewPathChanged);
    connect(dirCtrl, &DirController::searchStarted,
            this, &MainWindow::dirSearchStarted);

    connect(locationEdit, &LocationEdit::focused,
            this,         &MainWindow::locationEditFocused);
    connect(locationEdit, &LocationEdit::returnPressed,
            this,         &MainWindow::locationEditChanged);
    connect(locationEdit, &LocationEdit::buttonClicked,
            this,         &MainWindow::locationEditChanged);

    connect(searchLineEdit, &SearchLineEdit::textEdited,
            this, &MainWindow::searchTextEdited);

    connect(&searchTimer, &QTimer::timeout,
            this, &MainWindow::doSearch);

    /*connect(model, &FileSystemModel::directoryLoaded,
            this,  &MainWindow::modelDirectoryLoaded);*/

    connect(asyncFileOperation, &AsyncFileOperation::done,
            this,               &MainWindow::doneAsync);
    connect(asyncFileOperation, &AsyncFileOperation::progress,
            this,               &MainWindow::fileOperationProgress);
}

/**
 * @brief Opens the settings file, initializes the widgets, calls connectSlots()
 * and locationEditChanged().
 * @param initialPath The initial path to show to the user.
 */
MainWindow::MainWindow(const QString& initialPath) :
    QMainWindow()
{
    //! @todo clipboard view, workspace view
    //! @todo see Customizing QDockWidget in Qt Style Sheet Examples (Qt docs)

    //! @todo sort files according to access count in a time span (e.g. last X days)
    //! @todo clipboard with drag & drop
    //! @todo accesare meniu standard la fisiere, foldere (not x-platform)

    /**
     * \bug ask for confirmation on delete/overwrite in the
     * fileOperationsMenu which should be instantly shown with the relevant
     * file operation highlighted and with action buttons and text clearly
     * explaining what's happening
     *
     * @bug when dragging a file from the app to Windows File Explorer, the
     * file operation works well, but appearantly and surprisingly (because the
     * file operation happens in another thread/process) the app is somehow
     * broken, it hangs, it doesn't work until the file operation finishes. By
     * moving the mouse cursor over my app I can see the file being dragged
     * although I don't press any mouse button.
     */

    settings = new QSettings("settings.ini", QSettings::IniFormat);

    asyncFileOperation = new AsyncFileOperation();
    fileOperations = new QHash<const FileOperationData *, FileOperationItem *>();

    setWindowTitle("Linky Explorer");
    resize(800, 600);

    centralWidget = new QWidget();
    dirCtrl = new DirController();
    dirCtrl->view->hide();

    verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);

    toolBar = new QToolBar("Tool bar");
    searchToolBar = new QToolBar();
    searchLineEdit = new SearchLineEdit();

    stackedWidget = new QStackedWidget();
    stackedWidget->setSizePolicy(QSizePolicy::Preferred,
                                 QSizePolicy::Maximum);

    locationEdit = new LocationEdit();
    locationEdit->setText(initialPath);

    fileOperationsMenu = new FileOperationsMenu();

    fileOperationsButton = new QPushButton("File tasks");
    fileOperationsButton->setMenu(fileOperationsMenu);

    dock = new Dock();
    breadcrumb = new Breadcrumb(dock->model);

    graphView = new GraphView(dirCtrl->model, dock->model);

    auto viewCheckBox = new QPushButton(" auto");
    viewCheckBox->setObjectName("viewButton");
    viewCheckBox->setIcon(QIcon(":/secview.png"));
    connect(viewCheckBox, &QPushButton::clicked,
            this, &MainWindow::viewCheckBoxChanged);

    auto zoomInButton = new QPushButton("zoom +");
    zoomInButton->setAutoRepeat(true);

    auto zoomOutButton = new QPushButton("zoom -");
    zoomOutButton->setAutoRepeat(true);

    connect(zoomInButton, &QPushButton::clicked,
            [this] () {
        graphView->zoomIn();
    });
    connect(zoomOutButton, &QPushButton::clicked,
            [this] () {
        graphView->zoomOut();
    });

    searchTimer.setSingleShot(true);

    addToolBar(Qt::TopToolBarArea, toolBar);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(verticalLayout);
    centralWidget->setParent(this);
    stackedWidget->addWidget(breadcrumb);
    stackedWidget->addWidget(locationEdit);
    toolBar->addWidget(stackedWidget);
    toolBar->addWidget(fileOperationsButton);
    toolBar->addWidget(viewCheckBox);
    searchToolBar->addWidget(searchLineEdit);
    searchToolBar->addWidget(zoomInButton);
    searchToolBar->addWidget(zoomOutButton);
    //verticalLayout->addWidget(toolBar);
    verticalLayout->addWidget(graphView);
    verticalLayout->addWidget(dirCtrl->view);
    verticalLayout->addWidget(searchToolBar);

    connectSlots();

    locationEditChanged();
}

//! @todo show general progress on File tasks menu button
//! @todo keep selection when changing name filters

/**
 * @brief Signals the asyncFileOperation thread that the it must quit, and waits
 * for it to react, then it deleted asyncFileOperation.
 */
MainWindow::~MainWindow()
{
    asyncFileOperation->quit();
    asyncFileOperation->wait();
    delete asyncFileOperation;
}

/**
 * @brief Part of the singleton pattern. Returns the existing instance or creates one
 * if it doesn't exist yet, and returns it.
 * @param initialPath Passed to the MainWindow ctor. Default value: @a ROOT_PATH.
 * @return The existing or newly created MainWindow instance.
 */
MainWindow *MainWindow::getInstance(const QString &initialPath)
{
    QString path;
    if (initialPath.isEmpty()) {
        path = ROOT_PATH;
    } else {
        path = initialPath;
    }
    if (!instance) {
        instance = static_cast<MainWindow*>
                (::operator new(sizeof(MainWindow)));
        new (instance) MainWindow(initialPath);
    }
    return instance;
}

/**
 * @brief Trims the locationEdit text, handles the drives list possibility on
 * Windows, shows an error if the file/folder does not exist, otherwise opens
 * the file or folder.
 */
void MainWindow::locationEditChanged()
{
    QString path = locationEdit->text().trimmed();
    if (path.isEmpty() && path != ROOT_PATH) path = ROOT_PATH;
    else if (path.isEmpty() && path == ROOT_PATH) {
        // windows, QDir::drives....
        locationEdit->setText(path);

        dirCtrl->setPath(path);
        dirCtrl->view->setFocus();
        return;
    }

    QFileInfo file(path);

    if (file.exists()) {
        if (file.isFile()) {
            misc::openLocalFile(path);
            path = file.absolutePath();
        } else {
            path = file.absoluteFilePath();
        }
        locationEdit->setText(path);

        dirCtrl->setPath(path);
        dirCtrl->view->setFocus();
    } else {
        QMessageBox::information(this, "Error",
            "\"" + path + "\" does not exist.");
        locationEdit->selectAll();
    }
}

/**
 * @brief Hides breadcrumb by stacking locationEdit over it and focusing the
 * locationEdit.
 */
void MainWindow::toggleBreadcrumb()
{
    stackedWidget->setCurrentWidget(locationEdit);
    locationEdit->setFocus();
}

/**
 * @brief If !@a focused (?) and the locationEdit's context menu isn't open, stack
 * breadcrumb over locationEdit.
 * @param focused Whether the location edit is focused or not.
 */
void MainWindow::locationEditFocused(bool focused)
{
    if (!focused && !locationEdit->contextMenuOpen) {
        stackedWidget->setCurrentWidget(breadcrumb);
    }
}

/**
 * @brief Also calls locationEditChanged().
 * @param path Breadcrumb's path, to be set as the text of the location edit.
 */
void MainWindow::breadcrumbPathChanged(QString path)
{
    locationEdit->setText(path);
    locationEditChanged();
}

/**
 * @brief Changes the status of the file operation as finished and notifies the
 * user about this.
 * @param data The finished file operation's data.
 */
void MainWindow::doneAsync(FileOperationData *data)
{
    //! @todo bring to top in menu, when done (copy, move & all others)
    if (!fileOperations->contains(data)) {
        // internal file operation (currently recycle does this,
        // the future implementation shouldn't do this anymore,
        // so this check will be useless)
        return;
    }
    FileOperationItem *item = fileOperations->value(data);

    if (data->anyOperationsAborted) {
        item->verb = "Aborted " + item->verb.toLower();
    } else {
        if (data->type() == "copy") {
            item->verb = "Copied";
        } else if (data->type() == "move") {
            item->verb = "Moved";
        } else if (data->type() == "delete") {
            item->verb = "Deleted";
        } else if (data->type() == "recycle") {
            item->verb = "Recycled";
        }
    }

    item->setDateTime(QDateTime::currentDateTime());
    item->dateLabel->show();
    item->updateView();
}

/**
 * @brief Updates the progress shown in the file operation's item.
 * @param data The file operation's data whose progress has changed.
 */
void MainWindow::fileOperationProgress(FileOperationData *data)
{
    FileOperationItem *item = fileOperations->value(data);
    if (data->type() == "copy" || data->type() == "move") {
        item->update();
    }
}

/**
 * @brief @a path is forced on locationEdit and breadcrumb, and the breadcrumb
 * is stacked over locationEdit.
 * @param path The View's changed path.
 */
void MainWindow::viewPathChanged(const QString &path)
{
    locationEdit->setText(path);
    breadcrumb->setPath(path);
    stackedWidget->setCurrentWidget(breadcrumb);
}

/**
 * @brief Sets @a str as the searchLineEdit's text and focuses it.
 * @param str The initial search string.
 */
void MainWindow::dirSearchStarted(const QString &str)
{
    searchLineEdit->setText(str);
    searchLineEdit->setFocus();
}

/**
 * \brief Stores \a str in a buffer and starts searchTimer with 300ms interval.
 * \param str The current text of the searchLineEdit.
 */
void MainWindow::searchTextEdited(const QString &str)
{
    searchBuf = str;
    searchTimer.start(300);
}

/**
 * @brief Changes the layout of the view and updates the viewCheckBox to help
 * reflect that.
 * @see RootItem::setLayout
 */
void MainWindow::viewCheckBoxChanged()
{
    QPushButton *b = qobject_cast<QPushButton*>(sender());

    if (b->text() == " list") {
        b->setText(" auto"); //! @todo Find a better name for 'auto'
        //dirCtrl->view->hide();
        //graphView->show();
        graphView->rootItem->setLayout(RootItem::GRAPH);
    } else { // == " space"
        b->setText(" list");
        //graphView->hide();
        //dirCtrl->view->show();
        graphView->rootItem->setLayout(RootItem::LIST);
    }
}

/**
 * @brief This is called by searchTimer which is started by searchTextEdited().
 * The user either types slowly or has finished typing the search query in which
 * case the the computer is expected to execute the typed query and display the
 * results in the view.
 */
void MainWindow::doSearch()
{
    // why this if? backspace => the same text => useless computation
    if (searchBuf == searchLineEdit->text()) {
        qDebug() << "doSearch," << searchBuf;

        QStringList l;
        QString pat = searchBuf;
        pat = misc::escapeQDirNameFilter(pat);
        l << ("*" + pat + "*");
        dirCtrl->model->setNameFilters(l);
    }
}
