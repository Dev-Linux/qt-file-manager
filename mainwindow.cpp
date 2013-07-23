#include "mainwindow.h"

#include "view/WorkspaceView.h"
#include "view/WorkspaceController.h"

#include "DockController.h"
#include "Dock.h"

#include "view/DirController.h"
#include "view/View.h"

#include "SearchLineEdit.h"
#include "locationedit.h"
#include "fileoperationsmenu.h"
#include "breadcrumb.h"
#include "misc.h"
#include "view/RootItem.h"

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
    connect(locationEdit, &LocationEdit::focused,
            this, &MainWindow::location_edit_focused);
    connect(locationEdit, &LocationEdit::returnPressed,
            this, &MainWindow::location_edit_return_pressed);
    connect(locationEdit, &LocationEdit::buttonClicked,
            this, &MainWindow::location_edit_button_clicked);

    connect(searchLineEdit, &SearchLineEdit::textEdited,
            this, &MainWindow::search_text_edited);

    connect(zoom_in_button, &QPushButton::clicked,
            this, &MainWindow::zoom_in_requested);
    connect(zoom_out_button, &QPushButton::clicked,
            this, &MainWindow::zoom_out_requested);

    connect(layout_button, &QPushButton::clicked,
            this, &MainWindow::layout_button_clicked);
}

/**
 * @brief Opens the settings file, initializes the widgets, calls connectSlots()
 * and locationEditChanged().
 *
 * @bug ask for confirmation on delete/overwrite in the
 * fileOperationsMenu which should be instantly shown with the relevant
 * file operation highlighted and with action buttons and text clearly
 * explaining what's happening.
 *
 * @bug when dragging a file from the app to Windows File Explorer, the
 * file operation works well, but appearantly and surprisingly (because the
 * file operation happens in another thread/process) the app is somehow
 * broken, it hangs, it doesn't work until the file operation finishes. By
 * moving the mouse cursor over my app I can see the file being dragged
 * although I don't press any mouse button.
 *
 * @todo clipboard view, workspace view.
 * @todo see Customizing QDockWidget in Qt Style Sheet Examples (Qt docs).
 * @todo sort files according to access count in a time span (e.g. last X days).
 * @todo clipboard with drag & drop.
 * @todo accesare meniu standard la fisiere, foldere (not x-platform).
 */
MainWindow::MainWindow(Dock *dock_view,
                       WorkspaceView *workspace_view,
                       Breadcrumb *breadcrumb_view) :
    QMainWindow()
{
    setWindowTitle("Linky Explorer");
    resize(800, 600);

    centralWidget = new QWidget();

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

    fileOperationsMenu = new FileOperationsMenu();

    fileOperationsButton = new QPushButton("File tasks");
    fileOperationsButton->setMenu(fileOperationsMenu);

    layout_button = new QPushButton(" auto");
    layout_button->setObjectName("viewButton");
    layout_button->setIcon(QIcon(":/secview.png"));

    zoom_in_button = new QPushButton("zoom +");
    zoom_in_button->setAutoRepeat(true);

    zoom_out_button = new QPushButton("zoom -");
    zoom_out_button->setAutoRepeat(true);

    addToolBar(Qt::TopToolBarArea, toolBar);
    addDockWidget(Qt::RightDockWidgetArea, dock_view);
    setCentralWidget(centralWidget);
    centralWidget->setLayout(verticalLayout);
    centralWidget->setParent(this);
    stackedWidget->addWidget(breadcrumb_view);
    stackedWidget->addWidget(locationEdit);
    toolBar->addWidget(stackedWidget);
    toolBar->addWidget(fileOperationsButton);
    toolBar->addWidget(layout_button);
    searchToolBar->addWidget(searchLineEdit);
    searchToolBar->addWidget(zoom_in_button);
    searchToolBar->addWidget(zoom_out_button);
    //verticalLayout->addWidget(toolBar);
    verticalLayout->addWidget(workspace_view);
    verticalLayout->addWidget(searchToolBar);

    connect_slots();
}
