#include "ViewWidget.h"

#include "View.h"

ViewWidget::ViewWidget(View *v) :
    QWidget()
{
    this->v = v;
    setStyleSheet("ViewWidget { background: white; }");
    setSizePolicy(QSizePolicy::Preferred,
                  QSizePolicy::Maximum);
    resize(200, 1);
}

void ViewWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QSize ViewWidget::sizeHint() const
{
    auto sh = QWidget::sizeHint();
    auto sh2 = QSize(qMax(200, sh.width()), sh.height());
    if (v->ctrl == 0) {
        //qDebug() << "ViewWidget size (sizeHint):" << sh2;
    }
    return sh2;
}
