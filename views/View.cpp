#include "View.h"

#include "MainWindow.h"
#include "ViewScrollArea.h"
#include "ViewWidget.h"
#include "ListViewItem.h"

#include "models/ViewSelectionModel.h"
#include "models/DirModel.h"

#include "controllers/DirController.h"
#include "controllers/MainWindowController.h"

#include "misc.h"
#include "AsyncFileOperation.h"
#include "FileInfo.h"

View::View(QWidget *parent) :
    QWidget(parent)
{
    op = MainWindowController::instance()->async_file_op;

    ctrl = 0;
    l = new QVBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);

    auto wl = new QVBoxLayout();
    wl->setSpacing(0);
    wl->setContentsMargins(0, 0, 0, 0);

    s = new ViewScrollArea(this);

    w = new ViewWidget(this);

    // this, l
    //   s
    //     w, wl
    w->setLayout(wl);
    s->setWidget(w);
    l->addWidget(s);
    this->setLayout(l);

    // tab+click(=Qt::StrongFocus)+wheel
    setFocusPolicy(Qt::WheelFocus);

    //setFocusProxy(s);
}

ListViewItem *View::item_at(int index)
{
    //qDebug() << "index" << index;
    auto widget = w->layout()->itemAt(index)->widget();
    return qobject_cast<ListViewItem *>(widget);
}

int View::index_of(ListViewItem *item)
{
    return w->layout()->indexOf(item);
}

void View::add_item(ListViewItem *item)
{
    w->layout()->addWidget(item);
    w->layout()->setAlignment(item, Qt::AlignTop);
}

void View::remove_item(int index)
{
    delete w->layout()->takeAt(index)->widget();
}

int View::item_count() const
{
    return w->layout()->count();
}

void View::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

/*void View::keyPressEvent(QKeyEvent *)
{
    qDebug() << "View::keyPressEvent !";
}*/

QSize View::sizeHint() const
{
    if (ctrl == 0) {
        //qDebug() << "View sizeHint | w size =" << l->sizeHint();
    }
    return l->sizeHint();
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (ctrl != 0) {
        /**
         * @note QWidget::keyPressEvent(event) only closes popup windows if key
         * is Esc
         */

        auto sel = ctrl->model->sel;
        event->ignore();

        if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up) {
            if (sel->is_empty()) {
                if (!ctrl->model->is_empty()) {
                    sel->clear();
                    if (event->key() == Qt::Key_Down) {
                        sel->add(0);
                    } else {
                        Q_ASSERT(event->key() == Qt::Key_Up);
                        sel->add(ctrl->model->count() - 1);
                    }
                    sel->save();
                    event->accept();
                }
                return;
            }
            int x;
            if (event->key() == Qt::Key_Down) {
                x = 1;
            } else {
                Q_ASSERT(event->key() == Qt::Key_Up);
                x = -1;
            }
            if (ctrl->reduce_and_translate_sel_by(x)) {
                event->accept();
            }
        } else if (event->key() == Qt::Key_Return) {
            int selc = sel->count();
            if (selc > 0) {
                bool act;
                // folder selection handling (open other instances of the app)
                // check if this helps: QApplication::arguments().at(0)
                if (selc > 1) {
                    auto answer = QMessageBox::question(this, "Question",
                                          "Are you sure you want to open the " +
                                          QString::number(selc) + "selected file(s)?");
                    if (answer == QMessageBox::Yes) {
                        act = true;
                    } else {
                        act = false;
                    }
                } else {
                    act = true;
                }
                if (act) {
                    foreach (const int &x, sel->set) {
                        ctrl->open_index(x);
                    }
                }
            }
        } else { // page up/down, search etc.
            if (!event->text().isEmpty()) { // search
                if (event->text().length() > 1) {
                    qDebug() << "event->text().length() > 1 ... ?!";
                } else {
                    if (event->text().at(0).isPrint()) {
                        emit search(event->text());
                    }
                }
            } else {
                //qDebug() << "QKeyEvent::text() empty...?!";
                // I don't understand.
            }
            QApplication::postEvent(s, new QKeyEvent(event->type(),
                                                     event->key(),
                                                     event->modifiers()));
            event->accept();
            QWidget::keyPressEvent(event);
        }
        //! @todo ctrl, shift multi-select
        //! @todo the shift in shift-page-up/down to also move selection

        //! @todo alt alphanumeric shortcuts to displayed files (a8 or 9v or 11..)
        //! @todo alt + ctrl: selection of above
    }
}
