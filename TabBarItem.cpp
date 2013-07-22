#include "TabBarItem.h"

#include "view/GraphView.h"
#include "fileinfo.h"
#include "TabLabelItem.h"
#include "misc.h"

TabBarItem::TabBarItem(WorkspaceView *view) : QGraphicsObject()
{
    this->view = view;

    addButton.reset(new TabLabelItem("+"));
    addButton->setParentItem(this);
    connect(addButton.data(), &TabLabelItem::leftClicked,
            [this] () {
        // open new tab with some directory
    });

    addTabLabel(QString(), true);

    repositionTabLabels();
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

// random info: QList seems to destroy it's elements in it's
// destructor, so I don't need to use it with QScopedPointer
// elements

QRectF TabBarItem::boundingRect() const
{
    return QRectF(0, 0, view->viewport()->width(),
                  2 * padding + TabLabelItem::expectedHeight());
}

void TabBarItem::addTabLabel(const QString &path,
                             bool active)
{
    TabLabelItem *tl = new TabLabelItem();
    tabLabels.insert(tabLabels.end() - 1, tl);

    if (!path.isEmpty()) {
        const QString label = FileInfo(path).fileName();
        tl->setLabel(label);
    }

    if (active) {
        setActiveIndex(tabLabels.size() - 1);
    }

    tl->setParentItem(this);

    repositionTabLabels();
}

void TabBarItem::setActiveIndex(int index)
{
    if (activeIndex != -1) {
        tabLabels[activeIndex]->setActive(false);
    }
    tabLabels[index]->setActive();
    activeIndex = index;
}

void TabBarItem::modelPathChanged(const QString &path)
{
    auto tl = tabLabels[activeIndex];

    const QString label = FileInfo(path).fileName();
    tl->setLabel(label);

    repositionTabLabels();
}

void TabBarItem::repositionTabLabels()
{
    QPointF pos(padding, padding);
    if (!tabLabels.isEmpty()) {
        for (auto i = tabLabels.begin();
             i != tabLabels.end(); ++i) {
            TabLabelItem &tl = **i;
            tl.setPos(pos);
            pos.setX(pos.x() + tl.boundingRect().width() + spacing);
        }
    }
    addButton->setPos(pos);
    update();
}
