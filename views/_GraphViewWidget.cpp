#include "GraphViewWidget.h"
#include "mainwindow.h"

/*void GraphViewWidget::loadStart()
{
    // initial vertical box containing a help label
    auto vb = new QVBoxLayout();
    vb->setContentsMargins(0, 0, 0, 0);
    auto label = new QLabel("Drag here to begin.");
    label->setFont(QFont("sans-serif", 25));
    label->setAlignment(Qt::AlignCenter);

    vb->addWidget(label);

    setLayout(vb);
}*/

QRectF GraphViewWidget::boundingRect() const
{
    auto sh = sizeHint();
    return QRectF(0, 0, sh.width(), sh.height());
}

void GraphViewWidget::paint(QPainter *p,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    p->fillRect(option->rect, QColor("lightyellow"));
}

GraphViewWidget::GraphViewWidget(DirModel *model, GraphView *view) :
    QObject()
{
    // set the object name which is used from qss to style this widget
    //setObjectName("graphView");
    // TODO: graphics item with qss? possible?

    this->view = view;

    // MinimumExpanding means that the widget will expand as much as it is able
    // to do and never fall below the size hint which is considered the minimum
    // width
    this->setSizePolicy(QSizePolicy::MinimumExpanding,
                        QSizePolicy::MinimumExpanding);

    // the widget accepts drops (from the top-panel - list view and from the
    // rest of the environment)
    this->model = model;


}

