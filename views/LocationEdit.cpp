#include "LocationEdit.h"

LocationEdit::LocationEdit(QWidget *parent) :
    QLineEdit(parent)
{
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    button = new QToolButton(this);

    contextMenuOpen = false;
    button->setText("⇒");
    button->setCursor(Qt::ArrowCursor);
    button->setStyleSheet(QString("QToolButton { padding: 0px; }"));
    setStyleSheet(QString("QLineEdit { padding-right: %1px } ").arg(button->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), button->sizeHint().height() + frameWidth * 2 + 2));

    connect(button, &QToolButton::clicked,
            this, &LocationEdit::buttonClicked);
}


void LocationEdit::focusInEvent(QFocusEvent * event)
{
    emit focused(true);
    QLineEdit::focusInEvent(event);
}

void LocationEdit::focusOutEvent(QFocusEvent * event)
{
    emit focused(false);
    QLineEdit::focusOutEvent(event);
}

void LocationEdit::mouseDoubleClickEvent(QMouseEvent *)
{
    selectAll();
}

void LocationEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = button->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    button->move(rect().right() - sz.width(), rect().top());
    button->resize(sz.width() + frameWidth, this->height());
}

void LocationEdit::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenuOpen = true;
    QMenu *menu = createStandardContextMenu();
    //menu->addAction(tr("Test"));
    menu->exec(event->globalPos());
    contextMenuOpen = false;
    delete menu;
}
