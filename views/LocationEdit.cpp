#include "LocationEdit.h"

LocationEdit::LocationEdit(QWidget *parent) :
    QLineEdit(parent)
{
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    m_button = new QToolButton(this);

    context_menu_open = false;
    m_button->setText("⇒");
    m_button->setCursor(Qt::ArrowCursor);
    m_button->setStyleSheet(QString("QToolButton { padding: 0px; }"));
    setStyleSheet(QString("QLineEdit { padding-right: %1px } ").arg(m_button->sizeHint().width() + frameWidth + 1));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), m_button->sizeHint().height() + frameWidth * 2 + 2),
                   qMax(msz.height(), m_button->sizeHint().height() + frameWidth * 2 + 2));

    connect(m_button, &QToolButton::clicked,
            this, &LocationEdit::button_clicked);
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
    QSize sz = m_button->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

    m_button->move(rect().right() - sz.width(), rect().top());
    m_button->resize(sz.width() + frameWidth, this->height());
}

void LocationEdit::contextMenuEvent(QContextMenuEvent *event)
{
    context_menu_open = true;
    QMenu *menu = createStandardContextMenu();
    //menu->addAction(tr("Test"));
    menu->exec(event->globalPos());
    context_menu_open = false;
    delete menu;
}
