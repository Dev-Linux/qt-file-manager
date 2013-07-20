#include "filenode.h"

#include "misc.h"
#include "RootItem.h" // just for enum Layout?...

const QFont FileNode::iconLabelFont("sans-serif", 12);
QSize FileNode::node_size(100, 100);
QSize FileNode::node_icon_size(50, 50);

FileNode::FileNode(const FileInfo &info,
                   RootItem::Layout layout,
                   int list_height)
    : QGraphicsObject()
{
    this->layout = layout;
    this->list_size.setHeight(list_height);
    setProperty("selected", false);
    setProperty("hovered", false);

    //setFlags(QGraphicsItem::ItemIsSelectable);
             //QGraphicsItem::ItemIsMovable |
             //QGraphicsItem::ItemIsFocusable);

    setAcceptHoverEvents(true);

    this->fileInfo = info;

    update_icon_size();
}

void FileNode::setSelected(bool selected)
{
    setProperty("selected", selected);

    qDebug() << "update";
    update();
}

void FileNode::setLayout(RootItem::Layout l)
{
    this->layout = l;
    update_icon_size();
    qDebug() << "update";
    update();
}

void FileNode::setListWidth(qreal w)
{
    if (list_size.width() != w) {
        list_size.setWidth(w);
        update_icon_size();
    }
}

/*void FileNode::setListHeight(qreal h)
{
    if (list_size.height() != h) {
        list_size.setHeight(h);
        qDebug() << "updateIconSize";
        updateIconSize();
    }
}*/

qreal FileNode::listHeight() const
{
    return list_size.height();
}

bool FileNode::isSelected() const
{
    return property("selected").toBool();
}

bool FileNode::isHighlighted() const
{
    return property("highlighted").toBool();
}

QRectF FileNode::boundingRect() const
{
    if (layout == RootItem::GRAPH) { // && br.size() != node_size) {
        bounding_rect_cache.setSize(node_size);
    } else { //if (br.size() != list_size) { // LIST
        bounding_rect_cache.setSize(list_size);
    }
    return bounding_rect_cache;
}

void FileNode::paint(QPainter *p,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{ Q_UNUSED(widget)
    const int iw = icon_size.width();
    const int ih = icon_size.height();
    const int x = option->rect.x();
    const int y = option->rect.y();
    const int w = option->rect.width();
    const int h = option->rect.height();
    const QFontMetrics &fm = option->fontMetrics;
    const bool is_graph = layout == RootItem::GRAPH;

    if (must_update_icon) {
        icon = fileInfo.icon().pixmap(icon_size).scaled(icon_size);
        // would be ok for drives too, but floppy disk makes great noise on counting entries
        // and on Win 7 I also have some strange "disk not available" error boxes for all disks
        // TODO: if drive (floppy, especially), don't count directly, but check if
        // available first, somehow
        if (fileInfo.isDir() && !fileInfo.isDrive()) {
            const int c = QDir(fileInfo.absoluteFilePath())
                    .entryList(QDir::AllEntries |
                     QDir::NoDotAndDotDot).count();
            if (c == 0) {
                const QString cs = QString::number(c);

                QPainter ip(&icon); // icon painter
                const QFontMetrics ifm = ip.fontMetrics(); // icon font metrics
                const QFontInfo ifi = ip.fontInfo(); // icon font info
                ip.setFont(QFont(ifi.family(),
                                ifi.pointSize() - 2));
                ip.setRenderHint(QPainter::Antialiasing);

                const auto br = ifm.boundingRect(cs);

                const int w = br.width();
                const int h = br.height();

                const int x = (iw - w) / 2;
                const int y = (ih - h) / 2;
                const QRect r(x, y, w, h);
                //qDebug() << "'0' boundingRect" << r;

                const int pad = h / 9;
                const QRect rr = misc::padRectangle(pad, r);

                QColor col(Qt::white);
                col.setAlphaF(0.35);

                ip.setPen(Qt::NoPen);
                ip.setBrush(QBrush(col));
                ip.drawRoundedRect(rr, 50, 50, Qt::RelativeSize);

                ip.setPen(Qt::black);
                ip.drawText(x, y + ifm.ascent(), cs);
                ip.end();
            }
        }
        must_update_icon = false;
    }

    p->setRenderHint(QPainter::Antialiasing);

    QBrush bg_brush, pen_brush;
    if (!isSelected()) {
        if (!property("hovered").toBool()) {
            if (!isHighlighted()) {
                bg_brush = option->palette.background();
            } else {
                bg_brush = QBrush("lightyellow");
            }
        } else {
            bg_brush = option->palette.midlight();
        }
        pen_brush = option->palette.text();
    } else {
        bg_brush = option->palette.highlight();
        pen_brush = option->palette.brightText();
    }
    p->setBrush(bg_brush);

    qreal pen_width;
    if (is_graph) {
        pen_width = 1;
    } else {
        pen_width = 0.3; // TODO: borders only on top/bottom
    }
    // TODO: scrolling in LIST layout

    p->setPen(QPen(pen_brush, pen_width));
    if (!is_graph) {
        // here borders on top/bottom
        p->drawLine(0, 0, w, 0);
        p->drawLine(0, h, w, h);

        p->setPen(Qt::NoPen);
    }

    if (is_graph) {
        p->drawRoundedRect(option->rect, 50, 50, Qt::RelativeSize);
    } else { // LIST
        p->drawRect(option->rect);
        p->setPen(QPen(pen_brush, pen_width));
    }

    QPoint iconPos;
    if (is_graph) {
        iconPos.setX(x + (w - iw) / 2);
        iconPos.setY(y + 8);
    } else { // LIST
        iconPos.setX(padding); // just a padding
        iconPos.setY(y + (h - ih) / 2);
    }
    p->drawImage(iconPos, icon.toImage());

    p->setFont(iconLabelFont);

    QString text = fileInfo.fileName();
    text = fm.elidedText(text, Qt::ElideRight, w);

    const QSize text_size = fm.boundingRect(text).size();

    QPoint text_pos;
    if (is_graph) {
        text_pos.setX(x + (w - text_size.width()) / 2);
        text_pos.setY(y + 8 + ih + fm.ascent());
    } else { // LIST
        text_pos.setX(x + iconPos.x() + iw + spacing);
        text_pos.setY(y + (h - text_size.height()) / 2 + fm.ascent());
    }

    p->drawText(text_pos, text);
}

void FileNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    //QGraphicsObject::mouseDoubleClickEvent(evt);
    // default implementation calls mousePressEvent
    // uncommented gave segfault
}

void FileNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        emit leftClicked(event->modifiers());
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(event->modifiers());
    }
    QGraphicsObject::mousePressEvent(event);
}

void FileNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setProperty("hovered", true);
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void FileNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    setProperty("hovered", false);
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

QVariant FileNode::itemChange(QGraphicsItem::GraphicsItemChange change,
                              const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange) {
        //qDebug() << value.toBool();
        //update();
    }
    return QGraphicsObject::itemChange(change, value);
}

void FileNode::setHighlighted(bool set)
{
    setProperty("highlighted", set);
    update();
}

void FileNode::update_icon_size()
{
    //DebugFilter::set("FileNode::updateIconSize");
    const int dp = 2 * padding; // double padding

    QSize new_icon_size;
    if (layout == RootItem::GRAPH) {
        new_icon_size = node_icon_size;
    } else { // LIST
        new_icon_size = icon_size.scaled(list_size - QSize(dp, dp),
                                         Qt::KeepAspectRatio);
    }
    if (new_icon_size != icon_size || icon_size.isNull()) {
        icon_size = new_icon_size;
        must_update_icon = true;
        qDebug() << "prepareGeometryChange (update)";
        prepareGeometryChange();
        metaDebug(new_icon_size);
    }
}
