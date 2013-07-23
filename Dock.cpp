#include "Dock.h"

#include "mainwindow.h"
#include "view/ListViewItem.h"
#include "view/View.h"
#include "fileinfo.h"
#include "DockModel.h"
#include <functional>
#include "misc.h"

Dock::Dock(DockModel *dock_model) : QDockWidget()
{
    view = new View();
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setWidget(view);
    this->setWindowTitle("Important");

    model = dock_model;

    menu = buildMenu();

    // this->setFeatures(QDockWidget::DockWidgetMovable); ? needed?
    //! @todo "routines" list (macros + places + favourites + ...)
}

QMenu *Dock::buildMenu()
{
    auto m = new QMenu();
    QAction *action = m->addAction("Remove");
    connect(action, &QAction::triggered,
            this, &Dock::removeActionTriggered);
    return m;
}

void Dock::removeItem(FileInfo &info)
{
    bool found = false;
    int c = view->itemCount();
    for (int i = 0; i < c; i++) {
        if (view->itemAt(i)->fileInfo.fileInfo ==
                info.fileInfo) {
            view->removeItem(i);
            found = true;
            break;
        }
    }
    Q_ASSERT(found);
}

// random info: simulating sender() with std::bind
// auto itemDoubleClicked = [] (ListViewItem* item) { };
// connect(item, &ListViewItem::doubleClicked,
// std::function<void()>(std::bind(itemDoubleClicked, item)));

void Dock::itemRightClicked(const QPoint &globalPos)
{ Q_UNUSED(globalPos)
    qDebug() << "begin";
    selectedItem = qobject_cast<ListViewItem*>(sender());

    if (menu != nullptr) {
        delete menu;
    }
    menu = buildMenu();
    menu->popup(QCursor::pos());
    qDebug() << "end";
}

void Dock::itemDoubleClicked()
{
    qDebug() << "begin";
    auto item = qobject_cast<ListViewItem*>(sender());
    auto info = item->fileInfo;
    if (info.isDir()) { // nu exclude isDrive
        auto p = new QProcess();
        QStringList args;
        args << info.absoluteFilePath();
        p->start(qApp->applicationFilePath(), args);
        qDebug() << p->program();
    } else {
        misc::openLocalFile(info.absoluteFilePath());
    }
    qDebug() << "end";
}

void Dock::removeActionTriggered()
{
    qDebug() << "begin";
    model->removePath(selectedItem->fileInfo.absoluteFilePath());
    qDebug() << "end";
}

QSize Dock::sizeHint() const
{
    return QDockWidget::sizeHint();
}
