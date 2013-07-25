#include "FileOperationsMenu.h"

#include "views/MainWindow.h"
#include "views/FileOperationItem.h"

#include "models/FileOperationData.h"

#include "controllers/MainWindowController.h"

FileOperationsMenu::FileOperationsMenu(QWidget *parent) :
    QMenu(parent)
{
    widget = new QWidget(this);
    layout = new QVBoxLayout(widget);
    action = new QWidgetAction(this);

    widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    widget->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    action->setDefaultWidget(widget);
    widget->setLayout(layout);
    emptyListLabel = new QLabel("Empty list");
    layout->addWidget(emptyListLabel);
    addAction(action);
}

QSize FileOperationsMenu::sizeHint() const
{
    return widget->sizeHint();
}

void FileOperationsMenu::showEvent(QShowEvent *event)
{
    /**
     * @todo If it isn't able to completely show up right aligned, then show it
     * left aligned.
     */
    QPushButton *button = MainWindowController::instance()->
            view->fileOperationsButton;
    move(button->mapToGlobal(QPoint(0, 0)).x() + button->width() - width(), this->pos().y());
    QMenu::showEvent(event);
}

FileOperationItem *FileOperationsMenu::addItem(FileOperationItem* item)
{
    layout->insertWidget(0, item);
    if (layout->indexOf(emptyListLabel) != -1) {
        delete emptyListLabel;
    }
    return item;
}
