#include "MainWindowController.h"

#include "mainwindow.h"
#include "view/RootItem.h"
#include "view/WorkspaceController.h"
#include "view/WorkspaceView.h"
#include "locationedit.h"

MainWindowController::MainWindowController(const QString &initial_path) :
    QObject()
{
    view = MainWindow::getInstance(initial_path);
    m_layout = RootItem::GRAPH;

    connect(view, &MainWindow::layout_button_clicked,
            this, &MainWindowController::layout_button_clicked);
    connect(view, &MainWindow::breadcrumb_clicked,
            this, &MainWindowController::breadcrumb_clicked);
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
