#include "mainwindow.h"

#include "view/WorkspaceView.h"
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
#include "view/WorkspaceController.h"
//#include <random>

/**
 * @class MainWindow
 * @todo Show general progress on File tasks menu button.
 * @todo Keep selection when changing name filters.
 */

/**
 * @brief Connects signals to other signals/slots.
 */
void MainWindow::connect_slots()
{
    connect(breadcrumb, &Breadcrumb::clicked,
            this, &MainWindow::breadcrumb_clicked);
    connect(breadcrumb, &Breadcrumb::pathChanged,
            this, &MainWindow::breadcrumb_path_changed);

    connect(dirCtrl, &DirController::pathChanged,
            this, &MainWindow::dir_ctrl_path_changed);
    connect(dirCtrl, &DirController::searchStarted,
            this, &MainWindow::dir_search_started);

    connect(locationEdit, &LocationEdit::focused,
            this, &MainWindow::location_edit_focused);
    connect(locationEdit, &LocationEdit::returnPressed,
            this, &MainWindow::location_edit_return_pressed);
    connect(locationEdit, &LocationEdit::buttonClicked,
            this, &MainWindow::location_edit_button_clicked);

    connect(searchLineEdit, &SearchLineEdit::textEdited,
            this, &MainWindow::search_text_edited);
}

/**
 * @brief Opens the settings file, initializes the widgets, calls connectSlots()
 * and locationEditChanged().
 * @param initialPath The initial path to show to the user.
 * @bug ask for confirmation on delete/overwrite in the
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
 *
 * @todo clipboard view, workspace view
 * @todo see Customizing QDockWidget in Qt Style Sheet Examples (Qt docs)
 * @todo sort files according to access count in a time span (e.g. last X days)
 * @todo clipboard with drag & drop
 * @todo accesare meniu standard la fisiere, foldere (not x-platform)
 */
MainWindow::MainWindow(const QString& initialPath) :
    QMainWindow()
{
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

    workspace_ctrl = new WorkspaceController(dirCtrl->model,
                                             dock->model);

    layout_button = new QPushButton(" auto");
    layout_button->setObjectName("viewButton");
    layout_button->setIcon(QIcon(":/secview.png"));
    connect(layout_button, &QPushButton::clicked,
            this, &MainWindow::layout_button_clicked);

    auto zoomInButton = new QPushButton("zoom +");
    zoomInButton->setAutoRepeat(true);

    auto zoomOutButton = new QPushButton("zoom -");
    zoomOutButton->setAutoRepeat(true);

    connect(zoomInButton, &QPushButton::clicked,
            workspace_ctrl, &WorkspaceController::zoom_in);
    connect(zoomOutButton, &QPushButton::clicked,
            workspace_ctrl, &WorkspaceController::zoom_out);

    addToolBar(Qt::TopToolBarArea, toolBar);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(verticalLayout);
    centralWidget->setParent(this);
    stackedWidget->addWidget(breadcrumb);
    stackedWidget->addWidget(locationEdit);
    toolBar->addWidget(stackedWidget);
    toolBar->addWidget(fileOperationsButton);
    toolBar->addWidget(layout_button);
    searchToolBar->addWidget(searchLineEdit);
    searchToolBar->addWidget(zoomInButton);
    searchToolBar->addWidget(zoomOutButton);
    //verticalLayout->addWidget(toolBar);
    verticalLayout->addWidget(workspace_ctrl->view);
    verticalLayout->addWidget(dirCtrl->view);
    verticalLayout->addWidget(searchToolBar);

    connect_slots();
}
