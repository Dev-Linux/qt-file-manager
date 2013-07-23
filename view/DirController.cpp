#include "DirController.h"

#include "mainwindow.h"
#include "fileoperationsmenu.h"
#include "View.h"
#include "DirModel.h"
#include "ListViewItem.h"
#include "ViewSelectionModel.h"
#include "ViewWidget.h"
#include "ViewScrollArea.h"
#include "asyncfileoperation.h"
#include "fileoperationdata.h"
#include "fileoperationitem.h"

#include "DockController.h"
#include "DockModel.h"

#include "misc.h"
#include "MainWindowController.h"

DirController::DirController(QObject *parent) :
    QObject(parent)
{
    model = 0;

    view = new View();
    view->ctrl = this;
    connect(view, &View::search,
            this, &DirController::search);

    setModel(new DirModel());
}

void DirController::setModel(DirModel *m)
{
    bool re = hasModel();
    if (re) {
        disconnect(model->sel, &ViewSelectionModel::changed,
                this, &DirController::selectionModelChanged);
        disconnect(model, &DirModel::added,
                this, &DirController::addItem);
        disconnect(model, &DirModel::cleared,
                this, &DirController::clearView);
        disconnect(model, &DirModel::tagAdded,
                this, &DirController::addTag);

        disconnect(model, &DirModel::pathChanged,
                this, &DirController::pathChanged);
        disconnect(model, &DirModel::nameFiltersChanged,
                this, &DirController::modelNameFiltersChanged);
    }
    model = m;
    if (m != 0) {
        connect(model->sel, &ViewSelectionModel::changed,
                this, &DirController::selectionModelChanged);
        connect(model, &DirModel::added,
                this, &DirController::addItem);
        connect(model, &DirModel::cleared,
                this, &DirController::clearView);
        connect(model, &DirModel::tagAdded,
                this, &DirController::addTag);

        connect(model, &DirModel::pathChanged,
                this, &DirController::pathChanged);
        connect(model, &DirModel::nameFiltersChanged,
                this, &DirController::modelNameFiltersChanged);
    }
    if (re) {
        refreshView();
    }
}

bool DirController::hasModel()
{
    return model != 0;
}

void DirController::setPath(const QString &path)
{
    model->setPath(path);
}

QSet<int> &DirController::savedSelectedIndices()
{
    return model->sel->savedSet;
}

QList<FileInfo> &DirController::fileInfoList()
{
    return model->list;
}

void DirController::itemClicked(const Qt::KeyboardModifiers &modifiers)
{
    auto clickedItem = qobject_cast<ListViewItem*>(sender());
    int index = view->indexOf(clickedItem);

    if (!modifiers.testFlag(Qt::ControlModifier)) {
        bool clickedItemWasSelected = model->selected(index);
        int selc = model->sel->count();
        model->sel->clear();
        if (!clickedItemWasSelected || selc > 1) {
            model->sel->add(index);
        }
    } else {
        bool clickedItemSelected = model->selected(index);
        if (!clickedItemSelected) {
            model->sel->add(index);
        } else {
            model->sel->remove(index);
        }
    }
    model->sel->save();
}

void DirController::itemRightClicked()
{
    ListViewItem* clickedItem = qobject_cast<ListViewItem*>(sender());
    int index = view->indexOf(clickedItem);
    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    // a try of making multiple selection
    // actually this mechanism should be implemented in the model first
    // the secondary selection should be dimmed and the user should be able
    // to swap the primary selection with the secondary selection
    // eventually, a history of selections would be complex but a nice feature

    /*bool init = item->isHighlighted();
    if (!init) {
        item->setHighlighted(true);
    }*/

    QMenu menu;
    menu.addAction("It's important");
#ifdef Q_OS_WIN32
    menu.addAction("Recycle this");
#else
    menu.addAction("Delete this");
#endif

    connect(&menu, &QMenu::triggered,
            this, &DirController::contextMenuTriggered);
    menu.exec(QCursor::pos());

    /*if (!init) {
        item->setHighlighted(init);
    }*/
}

void DirController::itemDoubleClicked()
{
    ListViewItem* item = qobject_cast<ListViewItem*>(sender());
    int index = view->indexOf(item);
    bool clickedItemWasSelected = model->selected(index);

    if (!clickedItemWasSelected) {
        model->sel->clear();
        model->sel->add(index);
        model->sel->save();
    }

    openIndex(index);
}

void DirController::addItem(const FileInfo &info)
{
    auto item = new ListViewItem(info);

    connect(item, &ListViewItem::clicked,
            this, &DirController::itemClicked);

    connect(item, &ListViewItem::doubleClicked,
            this, &DirController::itemDoubleClicked);

    connect(item, &ListViewItem::rightClicked,
            this, &DirController::itemRightClicked);

    view->addItem(item);
}

void DirController::clearView()
{
    misc::clearQLayout(view->w->layout());
}

void DirController::selectionModelChanged(QSet<int> added,
                                          QSet<int> removed)
{
    int lastAdded = -1;
    foreach (const int &x, added) {
        view->itemAt(x)->setSelected(true);
        lastAdded = x;
    }
    foreach (const int &x, removed) {
        view->itemAt(x)->setSelected(false);
    }
    if (lastAdded != -1) {
        view->s->scrollTo(lastAdded);
    }
}

void DirController::refreshView()
{
    clearView();
    if (hasModel()) {
        foreach (const FileInfo &info, model->list) {
            addItem(info);
        }
        foreach (const int &x, savedSelectedIndices()) {
            view->itemAt(x)->setSelected(true);
        }
    }
}

void DirController::addTag(int index, const QString &tag)
{
    if (tag == "recent") {
        auto item = view->itemAt(index);
        item->setHighlighted(true);
    }
}

void DirController::contextMenuTriggered(QAction *action)
{
    MainWindowController *main_win_ctrl = MainWindowController::instance();
    if (action->text() == "It's important") {
        foreach (const int &x, model->sel->savedSet) {
            auto path = view->itemAt(x)->fileInfo.absoluteFilePath();
            main_win_ctrl->view->dock_ctrl->model->addPath(path);
        }
    } else if (action->text() == "Delete this" ||
               action->text() == "Recycle this") {
        auto list = model->sel->savedSet;
        QStringList pathList;
        foreach (const int& index, list) {
            QString path = model->list[index].absoluteFilePath();
            pathList << QDir::toNativeSeparators(path);
        }

        auto data = new FileOperationData("delete", pathList);
        auto item = new FileOperationItem(data);
        QPainter p(item);
        //auto s = pathList.join(", ");
        //s = p.fontMetrics().elidedText(s, Qt::ElideRight, 300);
        //item->setLabel("<strong>" + s + "</strong>.");
        auto s = pathList.join(",<br>");
        main_win_ctrl->view->fileOperationsMenu->addItem(item);
        view->op->doAsync(data, item);
    }
}

void DirController::search(const QString &str)
{
    emit searchStarted(str);
}

void DirController::modelNameFiltersChanged(const QStringList &nf)
{
    Q_UNUSED(nf);
}

void DirController::openIndex(int index)
{
    auto item = view->itemAt(index);
    QString path = item->fileInfo.filePath();

    if (item->fileInfo.isDir()) {
        setPath(path);
    } else {
        if (misc::openLocalFile(path)) {
            model->addTag(index, "recent");
        }
    }
}

bool DirController::reduceAndTranslateSelectionBy(int x)
{
    auto sel = model->sel;

    int sel1 = sel->one();
    bool check;
    if (x > 0) {
        check = sel1 < model->count() - 1;
    } else {
        Q_ASSERT(x != 0);
        check = sel1 > 0;
    }

    if (check) {
        sel1 += x;

        sel->clear();
        sel->add(sel1);
        sel->save();

        return true;
    }
    return false;
}
