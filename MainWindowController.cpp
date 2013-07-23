#include "MainWindowController.h"

#include "mainwindow.h"
#include "view/RootItem.h"
#include "view/WorkspaceController.h"
#include "view/WorkspaceView.h"
#include "locationedit.h"
#include "breadcrumb.h"
#include "SearchLineEdit.h"
#include "misc.h"
#include "view/DirController.h"
#include "view/View.h"
#include "fileoperationdata.h"
#include "fileoperationitem.h"
#include "view/DirModel.h"
#include "asyncfileoperation.h"

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
    view = static_cast<MainWindow*>
            (::operator new(sizeof(MainWindow)));
    new (view) MainWindow();

    m_layout = RootItem::GRAPH;
    file_ops = new QHash<const FileOperationData *,
            FileOperationItem *>();
    async_file_op = new AsyncFileOperation();

    m_search_timer.setSingleShot(true);

    connect(view, &MainWindow::layout_button_clicked,
            this, &MainWindowController::layout_button_clicked);
    connect(view, &MainWindow::breadcrumb_clicked,
            this, &MainWindowController::breadcrumb_clicked);
    connect(view, &MainWindow::breadcrumb_path_changed,
            this, &MainWindowController::breadcrumb_path_changed);
    connect(view, &MainWindow::dir_ctrl_path_changed,
            this, &MainWindowController::dir_ctrl_path_changed);
    connect(view, &MainWindow::dir_search_started,
            this, &MainWindowController::dir_search_started);
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

    view->locationEdit->setText(initial_path);
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
        view->workspace_ctrl->set_layout(RootItem::GRAPH);
        m_layout = RootItem::GRAPH;
    } else { // == RootItem::GRAPH
        view->layout_button->setText(" list");
        //graphView->hide();
        //dirCtrl->view->show();
        view->workspace_ctrl->set_layout(RootItem::LIST);
        m_layout = RootItem::LIST;
    }
}
/**
 * @brief Hides breadcrumb by stacking locationEdit over it and focusing the
 * locationEdit.
 */
void MainWindowController::breadcrumb_clicked()
{
    view->stackedWidget->setCurrentWidget(view->locationEdit);
    view->locationEdit->setFocus();
}

/**
 * @brief Also calls locationEditChanged().
 * @param path Breadcrumb's path, to be set as the text of the location edit.
 */
void MainWindowController::breadcrumb_path_changed(const QString &path)
{
    view->locationEdit->setText(path);
    location_edit_changed();
}

/**
 * @brief @a path is forced on locationEdit and breadcrumb, and the breadcrumb
 * is stacked over locationEdit.
 * @param path The View's changed path.
 */
void MainWindowController::dir_ctrl_path_changed(const QString &path)
{
    view->locationEdit->setText(path);
    view->breadcrumb->setPath(path);
    view->stackedWidget->setCurrentWidget(view->breadcrumb);
}

/**
 * @brief Sets @a str as the searchLineEdit's text and focuses it.
 * @param str The initial search string.
 */
void MainWindowController::dir_search_started(const QString &str)
{
    view->searchLineEdit->setText(str);
    view->searchLineEdit->setFocus();
}

/**
 * @brief If locationEdit is not @a focused and locationEdit's context menu
 * isn't open, stack breadcrumb over locationEdit.
 * @param focused Whether the location edit is focused.
 */
void MainWindowController::location_edit_focused(bool focused)
{
    if (!focused && !view->locationEdit->contextMenuOpen) {
        view->stackedWidget->setCurrentWidget(view->breadcrumb);
    }
}

/**
 * @brief Trims the locationEdit text, handles the drives list possibility on
 * Windows, shows an error if the file/folder does not exist, otherwise opens
 * the file or folder.
 */
void MainWindowController::location_edit_changed()
{
    QString path = view->locationEdit->text().trimmed();
    if (path.isEmpty() && path != ROOT_PATH) {
        path = ROOT_PATH;
    } else if (path.isEmpty() && path == ROOT_PATH) {
        // windows, QDir::drives....
        view->locationEdit->setText(path);

        view->dirCtrl->setPath(path);
        view->dirCtrl->view->setFocus();
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
        view->locationEdit->setText(path);

        view->dirCtrl->setPath(path);
        view->dirCtrl->view->setFocus();
    } else {
        QMessageBox::information(view, "Error",
            QString("\"%1\" does not exist.").arg(path));
        view->locationEdit->selectAll();
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
    if (m_search_buf == view->searchLineEdit->text()) {
        QStringList l;
        QString pat = m_search_buf;
        pat = misc::escapeQDirNameFilter(pat);
        l << QString("*%1*").arg(pat);
        view->dirCtrl->model->setNameFilters(l);
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
