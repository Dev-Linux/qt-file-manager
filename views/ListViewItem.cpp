#include "ListViewItem.h"

#include "MainWindow.h"
#include "misc.h"

//QThread *ListViewItem::thread = new QThread();

ListViewItem::ListViewItem(const FileInfo &info) :
    QWidget()
{
    this->fileInfo = info;

    setMouseTracking(true);
    setAutoFillBackground(true);
    setProperty("selected", false);
    setProperty("hovered", false);

    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 1, 0, 1);

    l = new QLabel(this->fileInfo.fileName());

    auto iconLabel = new QLabel();
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSize s(50, 50);

    QPixmap original(this->fileInfo.icon().pixmap(s));

    auto scaled = original.scaled(s);

    // would be ok for drives too, but floppy disk makes great noise on counting entries
    // and on Win 7 I also have some strange "disk not available" error boxes for all disks
    /**
     * @todo if drive (floppy, especially), don't count directly, but check if
     * available first, somehow
     */
    if (fileInfo.isDir() && !fileInfo.isDrive()) {
        int c = fileInfo.fileInfo.absoluteDir()
                .entryList(QDir::AllEntries |
                 QDir::NoDotAndDotDot).count();

        if (c == 0) {
            const auto cs = QString::number(c);

            QPainter p(&scaled);
            p.setFont(QFont(l->font().family(), l->font().pointSize() - 2));
            p.setRenderHint(QPainter::Antialiasing);

            auto m = p.fontMetrics();
            auto br = m.boundingRect(cs);

            int w = br.width();
            int h = br.height();
            int x = (s.width() - w) / 2;
            int y = (s.height() - h) / 2;
            QRect r(x, y, w, h);

            int pad = h / 9;
            QRect rr = misc::padRectangle(pad, r);

            QColor col(Qt::white);
            col.setAlphaF(0.35);
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(col));
            p.drawRoundedRect(rr, 50, 50, Qt::RelativeSize);

            p.setPen(Qt::black);
            p.drawText(x, y + m.ascent(), cs);
        }
    }

    iconLabel->setPixmap(scaled);

    hbox->addWidget(iconLabel);
    hbox->addWidget(l);

    this->setLayout(hbox);
}

void ListViewItem::setSelected(bool selected)
{
    setProperty("selected", selected);
    misc::updateWithStyle(this, style());
}

void ListViewItem::setHighlighted(bool set)
{
    setProperty("highlighted", set);
    misc::updateWithStyle(this, style());
}

bool ListViewItem::isSelected()
{
    return property("selected").toBool();
}

bool ListViewItem::isHighlighted()
{
    return property("highlighted").toBool();
}

void ListViewItem::enterEvent(QEvent *)
{
    setProperty("hovered", true);
    misc::updateWithStyle(this, style());
}

void ListViewItem::leaveEvent(QEvent *)
{
    setProperty("hovered", false);
    misc::updateWithStyle(this, style());
}

void ListViewItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    /*QPainter p(this);
    p.fillRect(event->rect(), Qt::red);
    p.end();*/
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //QWidget::paintEvent(event);
}

void ListViewItem::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        emit clicked(event->modifiers());
        event->accept();
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(event->globalPos());
        event->accept();
    }
    //QWidget::mousePressEvent(event);
    // not needed because the widget is not a popup
    // which should be shown on clicking outsite of it

}

void ListViewItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
        event->accept();
    } else {
        event->ignore();
    }
    //QWidget::mouseDoubleClickEvent(event); => segfault...
}
