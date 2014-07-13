#include "MainWindowController.h"

#include "controllers/WorkspaceController.h"
#include "controllers/DirController.h"
#include "controllers/DockController.h"

#include "views/MainWindow.h"
#include "views/RootItem.h"
#include "views/WorkspaceView.h"
#include "views/LocationEdit.h"
#include "views/Breadcrumb.h"
#include "views/SearchLineEdit.h"
#include "views/View.h"
#include "views/FileOperationItem.h"

#include "models/FileOperationData.h"
#include "models/DockModel.h"
#include "models/DirModel.h"

#include "misc.h"
#include "AsyncFileOperation.h"

/**
 * @brief The singleton instance of MainWindowController.
 */
MainWindowController* MainWindowController::m_instance = nullptr;

/**
 * @brief Part of the singleton pattern. Returns the existing instance or
 * creates one if it doesn't exist yet, and returns it.
 *
 * @param initial_path Passed to the MainWindow ctor. Default value: @a
 * ROOT_PATH.
 *
 * @return The existing or newly created MainWindowController instance.
 *
 * @note I'm using the placement new because I've noticed that the program
 * doesn't work without it. It's use here is connected to the fact that in the
 * constructor MainWindowController() I am also using placement new and to the
 * reasons of it's use there.
 */
MainWindowController *MainWindowController::instance
                                        (const QString &initial_path)
{
    if (m_instance == nullptr) {
        QString path;
        if (initial_path.isEmpty()) {
            path = ROOT_PATH;
        } else {
            path = initial_path;
        }
        m_instance = static_cast<MainWindowController*>
                (::operator new(sizeof(MainWindowController)));
        new (m_instance) MainWindowController(initial_path);
    }
    return m_instance;

}

/**
 * @brief Creates a MainWindowController.
 *
 * @param initial_path The initial path to show to the user.
 *
 * @note I'm using placement new because the constructor of MainWindow calls
 * indirectly the instance() static method of MainWindowController. I haven't
 * found a better way to do this.
 */
MainWindowController::MainWindowController(const QString &initial_path) :
    QObject()
{
    m_dir_ctrl = new DirController();
    m_dock_ctrl = new DockController();
    m_workspace_ctrl = new WorkspaceController(m_dir_ctrl->model,
                                             m_dock_ctrl->model);
    m_breadcrumb_ctrl = new Breadcrumb(m_dock_ctrl->model);

    view = static_cast<MainWindow*>
            (::operator new(sizeof(MainWindow)));
    new (view) MainWindow(m_dock_ctrl->view,
                          m_workspace_ctrl->view, m_breadcrumb_ctrl);

    m_layout = RootItem::GRAPH;
    file_ops = new QHash<const FileOperationData *,
            FileOperationItem *>();
    async_file_op = new AsyncFileOperation();

    m_search_timer.setSingleShot(true);

    connect(m_breadcrumb_ctrl, &Breadcrumb::clicked,
            this, &MainWindowController::breadcrumb_clicked);
    connect(m_breadcrumb_ctrl, &Breadcrumb::path_changed,
            this, &MainWindowController::breadcrumb_path_changed);

    connect(m_dir_ctrl, &DirController::path_changed,
            this, &MainWindowController::dir_ctrl_path_changed);
    connect(m_dir_ctrl, &DirController::search_started,
            this, &MainWindowController::dir_search_started);

    connect(view, &MainWindow::layout_button_clicked,
            this, &MainWindowController::layout_button_clicked);
    connect(view, &MainWindow::location_edit_focused,
            this, &MainWindowController::location_edit_focused);

    connect(view, &MainWindow::location_edit_return_pressed,
            this, &MainWindowController::location_edit_changed);
    connect(view, &MainWindow::location_edit_button_clicked,
            this, &MainWindowController::location_edit_changed);

    connect(view, &MainWindow::search_text_edited,
            this, &MainWindowController::search_text_edited);

    connect(&m_search_timer, &QTimer::timeout,
            this, &MainWindowController::search_timeout);

    connect(async_file_op, &AsyncFileOperation::progress,
            this, &MainWindowController::file_op_progressed);
    connect(async_file_op, &AsyncFileOperation::done,
            this, &MainWindowController::file_op_done);

    connect(view, &MainWindow::zoom_in_requested,
            m_workspace_ctrl, &WorkspaceController::zoom_in);
    connect(view, &MainWindow::zoom_out_requested,
            m_workspace_ctrl, &WorkspaceController::zoom_out);

    view->location_edit->setText(initial_path);
    location_edit_changed();
}

/**
 * @brief Signals the asyncFileOperation thread that the it must quit, and waits
 * for it to react, then it deleted asyncFileOperation.
 */
MainWindowController::~MainWindowController()
{
    async_file_op->quit();
    async_file_op->wait();
    delete async_file_op;
}

bool MainWindowController::is_showing_drives() const
{
    return m_dir_ctrl->model->drives;
}

void MainWindowController::mark_path_as_important(const QString &path)
{
    m_dock_ctrl->model->add_path(path);
}

/**
 * @brief Changes the layout of the view and updates the layout_button to
 * reflect that.
 * @see RootItem::setLayout
 */
void MainWindowController::layout_button_clicked()
{
    if (m_layout == RootItem::LIST) {
        //! @todo Find a better name for 'auto'
        view->layout_button->setText(" auto");
        //dirCtrl->view->hide();
        //graphView->show();
        m_workspace_ctrl->set_layout(RootItem::GRAPH);
        m_layout = RootItem::GRAPH;
    } else { // == RootItem::GRAPH
        view->layout_button->setText(" list");
        //graphView->hide();
        //dirCtrl->view->show();
        m_workspace_ctrl->set_layout(RootItem::LIST);
        m_layout = RootItem::LIST;
    }
}
/**
 * @brief Hides breadcrumb by stacking locationEdit over it and focusing the
 * locationEdit.
 */
void MainWindowController::breadcrumb_clicked()
{
    view->stacked_widget->setCurrentWidget(view->location_edit);
    view->location_edit->setFocus();
}

/**
 * @brief Also calls locationEditChanged().
 * @param path Breadcrumb's path, to be set as the text of the location edit.
 */
void MainWindowController::breadcrumb_path_changed(const QString &path)
{
    view->location_edit->setText(path);
    location_edit_changed();
}

/**
 * @brief @a path is forced on locationEdit and breadcrumb, and the breadcrumb
 * is stacked over locationEdit.
 * @param path The View's changed path.
 */
void MainWindowController::dir_ctrl_path_changed(const QString &path)
{
    view->location_edit->setText(path);
    m_breadcrumb_ctrl->set_path(path);
    view->stacked_widget->setCurrentWidget(m_breadcrumb_ctrl);
}

/**
 * @brief Sets @a str as the searchLineEdit's text and focuses it.
 * @param str The initial search string.
 */
void MainWindowController::dir_search_started(const QString &str)
{
    view->search_line_edit->setText(str);
    view->search_line_edit->setFocus();
}

/**
 * @brief If locationEdit is not @a focused and locationEdit's context menu
 * isn't open, stack breadcrumb over locationEdit.
 * @param focused Whether the location edit is focused.
 */
void MainWindowController::location_edit_focused(bool focused)
{
    if (!focused && !view->location_edit->context_menu_open) {
        view->stacked_widget->setCurrentWidget(m_breadcrumb_ctrl);
    }
}

/**
 * @brief Trims the locationEdit text, handles the drives list possibility on
 * Windows, shows an error if the file/folder does not exist, otherwise opens
 * the file or folder.
 */
void MainWindowController::location_edit_changed()
{
    QString path = view->location_edit->text().trimmed();
    if (path.isEmpty() && path != ROOT_PATH) {
        path = ROOT_PATH;
    } else if (path.isEmpty() && path == ROOT_PATH) {
        // windows, QDir::drives....
        view->location_edit->setText(path);

        m_dir_ctrl->set_path(path);
        m_dir_ctrl->view->setFocus();
        return;
    }

    QFileInfo file(path);

    if (file.exists()) {
        if (file.isFile()) {
            misc::open_local_file(path);
            path = file.absolutePath();
        } else {
            path = file.absoluteFilePath();
        }
        view->location_edit->setText(path);

        m_dir_ctrl->set_path(path);
        m_dir_ctrl->view->setFocus();
    } else {
        QMessageBox::information(view, "Error",
            QString("\"%1\" does not exist.").arg(path));
        view->location_edit->selectAll();
    }
}

/**
 * @brief Stores @a text in a buffer and starts searchTimer with 300ms interval.
 * @param text The current text of the searchLineEdit.
 */
void MainWindowController::search_text_edited(const QString &text)
{
    m_search_buf = text;
    m_search_timer.start(300);
}

/**
 * @brief This is called by searchTimer which is started by searchTextEdited().
 * The user either types slowly or has finished typing the search query in which
 * case the the computer is expected to execute the typed query and display the
 * results in the view.
 */
void MainWindowController::search_timeout()
{
    // why this if exists? backspace => the same text => useless computation
    if (m_search_buf == view->search_line_edit->text()) {
        QStringList l;
        QString pat = m_search_buf;
        pat = misc::escape_QDir_name_filter(pat);
        l << QString("*%1*").arg(pat);
        m_dir_ctrl->model->set_name_filters(l);
    }
}

/**
 * @brief Updates the progress shown in the file operation's item.
 * @param data The file operation's data whose progress has changed.
 */
void MainWindowController::file_op_progressed(FileOperationData *data)
{
    FileOperationItem *item = file_ops->value(data);
    if (data->type() == "copy" || data->type() == "move") {
        item->update();
    }
}

/**
 * @brief Changes the status of the file operation as finished and notifies the
 * user about this.
 * @param data The finished file operation's data.
 */
void MainWindowController::file_op_done(FileOperationData *data)
{
    //! @todo bring to top in menu, when done (copy, move & all others)
    if (!file_ops->contains(data)) {
        // internal file operation (currently recycle does this,
        // the future implementation shouldn't do this anymore,
        // so this check will be useless)
        return;
    }
    FileOperationItem *item = file_ops->value(data);

    if (data->any_ops_aborted) {
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

    item->set_date_time(QDateTime::currentDateTime());
    item->date_label->show();
    item->update_view();
}
