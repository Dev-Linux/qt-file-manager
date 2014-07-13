#include "Dock.h"

#include "MainWindow.h"
#include "ListViewItem.h"
#include "View.h"

#include "FileInfo.h"
#include "misc.h"

#include <functional>

/**
 * @class Dock
 * @todo "routines" list (macros + places + favourites + ...)
 */

Dock::Dock() : QDockWidget()
{
    view = new View();
    view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setWidget(view);
    this->setWindowTitle("Important");

    m_menu = build_menu();

    // this->setFeatures(QDockWidget::DockWidgetMovable); ? needed?
}

QMenu *Dock::build_menu()
{
    auto m = new QMenu();
    QAction *action = m->addAction("Remove");
    connect(action, &QAction::triggered,
            this, &Dock::remove_action_triggered);
    return m;
}

void Dock::remove_item(FileInfo &info)
{
    bool found = false;
    int c = view->item_count();
    for (int i = 0; i < c; i++) {
        if (view->item_at(i)->file_info == info) {
            view->remove_item(i);
            found = true;
            break;
        }
    }
    Q_ASSERT(found);
}

/**
 * @brief Dock::itemRightClicked
 * @note Random info: simulating `QObject::sender()` with `std::bind`
 * @code
 * auto itemDoubleClicked = [] (ListViewItem* item) { };
 * connect(item, &ListViewItem::doubleClicked,
 *     std::function<void()>(std::bind(itemDoubleClicked, item)));
 * @endcode
 */
void Dock::item_right_clicked(const QPoint &globalPos)
{ Q_UNUSED(globalPos)
    qDebug() << "begin";
    selected_item = qobject_cast<ListViewItem*>(sender());

    if (m_menu != nullptr) {
        delete m_menu;
    }
    m_menu = build_menu();
    m_menu->popup(QCursor::pos());
    qDebug() << "end";
}

void Dock::item_double_clicked()
{
    qDebug() << "begin";
    auto item = qobject_cast<ListViewItem*>(sender());
    auto info = item->file_info;
    if (info.is_dir()) { // nu exclude isDrive
        auto p = new QProcess();
        QStringList args;
        args << info.abs_file_path();
        p->start(qApp->applicationFilePath(), args);
        qDebug() << p->program();
    } else {
        misc::open_local_file(info.abs_file_path());
    }
    qDebug() << "end";
}

QSize Dock::sizeHint() const
{
    return QDockWidget::sizeHint();
}
