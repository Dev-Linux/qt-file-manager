#include "FileOperationsMenu.h"

#include "views/MainWindow.h"
#include "views/FileOperationItem.h"

#include "models/FileOperationData.h"

#include "controllers/MainWindowController.h"

FileOperationsMenu::FileOperationsMenu(QWidget *parent) :
    QMenu(parent)
{
    m_widget = new QWidget(this);
    m_layout = new QVBoxLayout(m_widget);
    m_action = new QWidgetAction(this);

    m_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_widget->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    m_action->setDefaultWidget(m_widget);
    m_widget->setLayout(m_layout);
    m_empty_list_label = new QLabel("Empty list");
    m_layout->addWidget(m_empty_list_label);
    addAction(m_action);
}

QSize FileOperationsMenu::sizeHint() const
{
    return m_widget->sizeHint();
}

void FileOperationsMenu::showEvent(QShowEvent *event)
{
    /**
     * @todo If it isn't able to completely show up right aligned, then show it
     * left aligned.
     */
    QPushButton *button = MainWindowController::instance()->
            view->file_ops_button;
    move(button->mapToGlobal(QPoint(0, 0)).x() + button->width() - width(), this->pos().y());
    QMenu::showEvent(event);
}

FileOperationItem *FileOperationsMenu::add_item(FileOperationItem* item)
{
    m_layout->insertWidget(0, item);
    if (m_layout->indexOf(m_empty_list_label) != -1) {
        delete m_empty_list_label;
    }
    return item;
}
