#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include "view/RootItem.h"

class MainWindow;
class FileOperationData;

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    static MainWindowController *instance(const QString &initial_path = QString());
    explicit MainWindowController(const QString &initial_path);
    MainWindow *view;
    
signals:
    
public slots:

private slots:
    void layout_button_clicked();
    void breadcrumb_clicked();
    void breadcrumb_path_changed(const QString &path);
    void dir_ctrl_path_changed(const QString &path);
    void dir_search_started(const QString &str);
    void location_edit_focused(bool focused);
    void location_edit_changed();
    void search_text_edited(const QString &text);
    void search_timeout();
    void file_op_progressed(FileOperationData *data);
    void file_op_done(FileOperationData *data);

private:
    RootItem::Layout m_layout;
    static MainWindowController *m_instance;
    QTimer m_search_timer;
    QString m_search_buf;
};

#endif // MAINWINDOWCONTROLLER_H
