#include "TabBarItem.h"

#include "views/WorkspaceView.h"
#include "views/TabLabelItem.h"

#include "FileInfo.h"
#include "misc.h"

/**
 * @class TabBarItem
 * @note Random info: QList seems to destroy it's elements in it's destructor,
 * so I don't need to use it with @q{core,QScopedPointer} elements.
 */

TabBarItem::TabBarItem(WorkspaceView *view) : QGraphicsObject()
{
    this->m_view = view;

    m_add_button.reset(new TabLabelItem("+"));
    m_add_button->setParentItem(this);
    connect(m_add_button.data(), &TabLabelItem::left_clicked,
            [this] () {
        // open new tab with some directory
    });

    add_tab_label(QString(), true);

    reposition_tab_labels();
}

TabBarItem::~TabBarItem()
{
}

void TabBarItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(painter)
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush("lightblue"));
    painter->drawRect(option->rect);
}

QRectF TabBarItem::boundingRect() const
{
    return QRectF(0, 0, m_view->viewport()->width(),
                  2 * m_padding + TabLabelItem::expected_height());
}

void TabBarItem::add_tab_label(const QString &path,
                             bool active)
{
    TabLabelItem *tl = new TabLabelItem();
    if (!m_tab_labels.isEmpty()) {
        m_tab_labels.insert(m_tab_labels.end() - 1, tl);
    } else {
        m_tab_labels.append(tl);
    }

    if (!path.isEmpty()) {
        const QString label = FileInfo(path).file_name();
        tl->set_label(label);
    }

    if (active) {
        set_active_index(m_tab_labels.size() - 1);
    }

    tl->setParentItem(this);

    reposition_tab_labels();
}

void TabBarItem::set_active_index(int index)
{
    if (m_active_index != -1) {
        m_tab_labels[m_active_index]->set_active(false);
    }
    m_tab_labels[index]->set_active();
    m_active_index = index;
}

void TabBarItem::model_path_changed(const QString &path)
{
    auto tl = m_tab_labels[m_active_index];

    const QString label = FileInfo(path).file_name();
    tl->set_label(label);

    reposition_tab_labels();
}

void TabBarItem::reposition_tab_labels()
{
    QPointF pos(m_padding, m_padding);
    if (!m_tab_labels.isEmpty()) {
        for (auto i = m_tab_labels.begin();
             i != m_tab_labels.end(); ++i) {
            TabLabelItem &tl = **i;
            tl.setPos(pos);
            pos.setX(pos.x() + tl.boundingRect().width() + m_spacing);
        }
    }
    m_add_button->setPos(pos);
    update();
}
