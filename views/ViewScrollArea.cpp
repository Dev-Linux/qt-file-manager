#include "ViewScrollArea.h"

#include "View.h"
#include "ListViewItem.h"

#include "models/ViewSelectionModel.h"
#include "models/DirModel.h"

ViewScrollArea::ViewScrollArea(View* view) :
    QScrollArea()
{
    setWidgetResizable(true);
    setFocusProxy(view);
}

void ViewScrollArea::scroll_to(int index)
{
    View *view = qobject_cast<View*>(parentWidget());
    this->ensureWidgetVisible(view->item_at(index), 0, 0);
}

void ViewScrollArea::keyReleaseEvent(QKeyEvent *)
{
    //! @todo modifier key(s) holded => detail view
}

void ViewScrollArea::focusInEvent(QFocusEvent *)
{

}
