#include "ViewScrollArea.h"

#include "View.h"
#include "ListViewItem.h"
#include "DirModel.h"
#include "ViewSelectionModel.h"

ViewScrollArea::ViewScrollArea(View* view) :
    QScrollArea()
{
    setWidgetResizable(true);
    setFocusProxy(view);
}

void ViewScrollArea::scrollTo(int index)
{
    View *view = qobject_cast<View*>(parentWidget());
    this->ensureWidgetVisible(view->itemAt(index), 0, 0);
}

void ViewScrollArea::keyReleaseEvent(QKeyEvent *)
{
    // TODO: modifier key(s) holded => detail view
}

void ViewScrollArea::focusInEvent(QFocusEvent *)
{

}
