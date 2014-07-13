#include "ListViewItem.h"

#include "MainWindow.h"
#include "misc.h"

//QThread *ListViewItem::thread = new QThread();

ListViewItem::ListViewItem(const FileInfo &info) :
    QWidget()
{
    this->file_info = info;

    setMouseTracking(true);
    setAutoFillBackground(true);
    setProperty("selected", false);
    setProperty("hovered", false);

    auto hbox = new QHBoxLayout();
    hbox->setContentsMargins(0, 1, 0, 1);

    label = new QLabel(this->file_info.file_name());

    auto iconLabel = new QLabel();
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSize s(50, 50);

    QPixmap original(this->file_info.icon().pixmap(s));

    auto scaled = original.scaled(s);

    // would be ok for drives too, but floppy disk makes great noise on counting entries
    // and on Win 7 I also have some strange "disk not available" error boxes for all disks
    /**
     * @todo if drive (floppy, especially), don't count directly, but check if
     * available first, somehow
     */
    if (file_info.is_dir() && !file_info.is_drive()) {
        int c = file_info.file_info.absoluteDir()
                .entryList(QDir::AllEntries |
                 QDir::NoDotAndDotDot).count();

        if (c == 0) {
            const auto cs = QString::number(c);

            QPainter p(&scaled);
            p.setFont(QFont(label->font().family(), label->font().pointSize() - 2));
            p.setRenderHint(QPainter::Antialiasing);

            auto m = p.fontMetrics();
            auto br = m.boundingRect(cs);

            int w = br.width();
            int h = br.height();
            int x = (s.width() - w) / 2;
            int y = (s.height() - h) / 2;
            QRect r(x, y, w, h);

            int pad = h / 9;
            QRect rr = misc::pad_rect(pad, r);

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
    hbox->addWidget(label);

    this->setLayout(hbox);
}

void ListViewItem::set_selected(bool selected)
{
    setProperty("selected", selected);
    misc::update_with_style(this, style());
}

void ListViewItem::set_highlighted(bool set)
{
    setProperty("highlighted", set);
    misc::update_with_style(this, style());
}

bool ListViewItem::is_selected()
{
    return property("selected").toBool();
}

bool ListViewItem::is_highlighted()
{
    return property("highlighted").toBool();
}

void ListViewItem::enterEvent(QEvent *)
{
    setProperty("hovered", true);
    misc::update_with_style(this, style());
}

void ListViewItem::leaveEvent(QEvent *)
{
    setProperty("hovered", false);
    misc::update_with_style(this, style());
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
        emit right_clicked(event->globalPos());
        event->accept();
    }
    //QWidget::mousePressEvent(event);
    // not needed because the widget is not a popup
    // which should be shown on clicking outsite of it

}

void ListViewItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit double_clicked();
        event->accept();
    } else {
        event->ignore();
    }
    //QWidget::mouseDoubleClickEvent(event); => segfault...
}
