#include "listview.h"
#include "mainwindow.h"

ListView::ListView(QWidget *parent) :
    QListView(parent)
{
    // TODO: secondary/backup selection
    // TODO: pt. linux: delete to trash
    // NOTE: momentan, toate eforturile pt. Windows
    op = MainWindow::getInstance()->asyncFileOperation;

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setDragEnabled(true);
    this->viewport()->setAcceptDrops(true);
    this->setDropIndicatorShown(true);
    this->setDragDropMode(QAbstractItemView::DragDrop);
}

ListView::~ListView()
{
}

void ListView::showMenu(QPoint globalPos) {
    QModelIndex index = indexAt(mapFromGlobal(globalPos));

    if (index.isValid()) {
        QMenu *menu = new QMenu(this);
        connect(menu, &QMenu::aboutToHide,
                this, &ListView::aboutToHideContextMenu);

        QAction *action = new QAction("Delete", menu);
        connect(action, &QAction::triggered,
                this, &ListView::deleteTriggered);
        menu->addAction(action);

        menu->exec(globalPos);
        delete menu;
        menu = 0;
    }
}

void ListView::aboutToHideContextMenu()
{
    //mousePressEvent(new QMouseEvent(QMouseEvent::MouseButtonPress,
    //                                mapFromGlobal(QCursor::pos()), Qt::RightButton, 0, 0));
}

void ListView::deleteTriggered()
{
    QModelIndexList list = selectedIndexes();
    QStringList pathList;
    FileSystemModel *m = dynamic_cast<FileSystemModel*>(model());
    foreach (const QModelIndex& index, list) {
        QString path = m->filePath(index);
        pathList << QDir::toNativeSeparators(path);
    }

    FileOperationData *data = new FileOperationData("delete", pathList, "");
    FileOperationItem *item = MainWindow::getInstance()->fileOperationsMenu->addItem(new FileOperationItem(data,
        "<strong>" + pathList.join(", ") + "</strong>."));
    op->doAsync(data, item);
}

void ListView::mousePressEvent(QMouseEvent *event)
{
    // TODO: meniu contextual ca-n File Explorer - clic dreapta in afara meniului pe un
    // fisier nu numai ca defocalizeaza si inchide meniul deschis in prezent ci si
    // deschide meniul contextual pt. fisierul de sub cursor in prezent
    // vezi:
    // https://www.google.com/search?q=qmenu+outside+click&aq=f&oq=qmenu+outside+click&sourceid=chrome&ie=UTF-8
    // https://bugreports.qt-project.org/browse/QTBUG-8573 mai exact: „Jason
    // Haslam added a comment - 07/Feb/11 11:39 PM I think that, on OS X at
    // least, the mouse event needs to replay only if it was a right-click.
    // Left-click should just cancel the popup. I'm working around this by
    // extending QMenu to store the position of the last right-click outside of
    // the menu's client area and then replaying it after the popup finishes.
    // Andy Shaw (closed Nokia identity) added a comment - 21/Mar/11 1:59 PM
    // When a right click is done in the same window then it should be
    // replayed, however if it is not in the same window then it shouldn't show
    // the popup. Xcode and Firefox on Mac verify this behavior.”


    if (event->button() == Qt::RightButton) {
        QModelIndex index = indexAt(event->pos());
        if (selectedIndexes().contains(index)) { // daca deasupra selectiei
            // lucrez cu selectia, nu o modific
        } else {
            // selectez elementul de sub cursor, lucrez cu el
            setCurrentIndex(index);
        }

        showMenu(event->globalPos());

        /*qDebug() << (int)menu;// << menu->activeAction();
        if (menu != 0 && menu->activeAction() == 0) {
            showMenu(event->globalPos());
        }*/
    }

    QListView::mousePressEvent(event);
}
