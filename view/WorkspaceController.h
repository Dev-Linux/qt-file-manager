#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include <QObject>
#include "RootItem.h"

class WorkspaceView;
class DirModel;
class DockModel;

class WorkspaceController : public QObject
{
    Q_OBJECT
public:
    explicit WorkspaceController(DirModel *model,
                                 DockModel *dockModel);
    WorkspaceView *view;
    void set_layout(RootItem::Layout layout);
signals:
    
public slots:
    void zoom_in();
    void zoom_out();
};

#endif // WORKSPACECONTROLLER_H
