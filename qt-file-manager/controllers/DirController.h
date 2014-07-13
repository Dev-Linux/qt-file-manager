#pragma once
#ifndef DIRCONTROLLER_H
#define DIRCONTROLLER_H

#include <QtWidgets>

#include "FileInfo.h"

class View;
class DirModel;
class View;

class DirController : public QObject
{
    Q_OBJECT
public:
    explicit DirController(QObject *parent = 0);

    View *view;
    DirModel *model;

    void set_model(DirModel *m);
    bool has_model();

    void set_path(const QString &path);
    QSet<int> &saved_selected_indices();
    QList<FileInfo> &file_info_list();
    void open_index(int index);
    bool reduce_and_translate_sel_by(int x);

    void refresh_view();

signals:
    void path_changed(const QString &path);
    void search_started(const QString &str);

protected:

    /**
     * @todo keyboard smooth scrolling (with a secondary cursor to
     * which I can switch... that would help me (search the web)
     */
public slots:
    void item_clicked(const Qt::KeyboardModifiers &modifiers);
    void item_right_clicked();
    void item_double_clicked();

    void sel_model_changed(QSet<int> added,
                           QSet<int> removed);

    void add_item(const FileInfo &info);
    void clear_view();

    void add_tag(int index, const QString &tag);

    void context_menu_triggered(QAction *action);

    void search(const QString& str);
    void model_name_filters_changed(const QStringList& nf);
};

#endif // DIRCONTROLLER_H
