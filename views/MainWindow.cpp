#include "MainWindow.h"

#include "views/WorkspaceView.h"
#include "views/Dock.h"
#include "views/View.h"
#include "views/SearchLineEdit.h"
#include "views/LocationEdit.h"
#include "views/FileOperationsMenu.h"
#include "views/Breadcrumb.h"
#include "views/RootItem.h"

#include "controllers/WorkspaceController.h"
#include "controllers/DockController.h"
#include "controllers/DirController.h"

#include "misc.h"

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
    connect(location_edit, &LocationEdit::focused,
            this, &MainWindow::location_edit_focused);
    connect(location_edit, &LocationEdit::returnPressed,
            this, &MainWindow::location_edit_return_pressed);
    connect(location_edit, &LocationEdit::button_clicked,
            this, &MainWindow::location_edit_button_clicked);

    connect(search_line_edit, &SearchLineEdit::textEdited,
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

    m_central_widget = new QWidget();

    m_vertical_layout = new QVBoxLayout();
    m_vertical_layout->setContentsMargins(0, 0, 0, 0);
    m_vertical_layout->setSpacing(0);

    tool_bar = new QToolBar("Tool bar");
    search_tool_bar = new QToolBar();
    search_line_edit = new SearchLineEdit();

    stacked_widget = new QStackedWidget();
    stacked_widget->setSizePolicy(QSizePolicy::Preferred,
                                 QSizePolicy::Maximum);

    location_edit = new LocationEdit();

    file_ops_menu = new FileOperationsMenu();

    file_ops_button = new QPushButton("File tasks");
    file_ops_button->setMenu(file_ops_menu);

    layout_button = new QPushButton(" auto");
    layout_button->setObjectName("viewButton");
    layout_button->setIcon(QIcon(":/secview.png"));

    zoom_in_button = new QPushButton("zoom +");
    zoom_in_button->setAutoRepeat(true);

    zoom_out_button = new QPushButton("zoom -");
    zoom_out_button->setAutoRepeat(true);

    addToolBar(Qt::TopToolBarArea, tool_bar);
    addDockWidget(Qt::RightDockWidgetArea, dock_view);
    setCentralWidget(m_central_widget);
    m_central_widget->setLayout(m_vertical_layout);
    m_central_widget->setParent(this);
    stacked_widget->addWidget(breadcrumb_view);
    stacked_widget->addWidget(location_edit);
    tool_bar->addWidget(stacked_widget);
    tool_bar->addWidget(file_ops_button);
    tool_bar->addWidget(layout_button);
    search_tool_bar->addWidget(search_line_edit);
    search_tool_bar->addWidget(zoom_in_button);
    search_tool_bar->addWidget(zoom_out_button);
    //verticalLayout->addWidget(toolBar);
    m_vertical_layout->addWidget(workspace_view);
    m_vertical_layout->addWidget(search_tool_bar);

    connect_slots();
}
