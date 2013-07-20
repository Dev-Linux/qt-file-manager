#pragma once
#ifndef DIRCONTROLLER_H
#define DIRCONTROLLER_H

#include <QtWidgets>

#include "fileinfo.h"

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

    void setModel(DirModel *m);
    bool hasModel();

    void setPath(const QString &path);
    QSet<int> &savedSelectedIndices();
    QList<FileInfo> &fileInfoList();
    void openIndex(int index);
    bool reduceAndTranslateSelectionBy(int x);

    void refreshView();

signals:
    void pathChanged(const QString &path);
    void searchStarted(const QString &str);

protected:

    // NOTE: keyboard smooth scrolling (with a secondary cursor to which
    // I can switch... that would help me (search the web)
public slots:
    void itemClicked(const Qt::KeyboardModifiers &modifiers);
    void itemRightClicked();
    void itemDoubleClicked();

    void selectionModelChanged(QSet<int> added,
                               QSet<int> removed);

    void addItem(const FileInfo &info);
    void clearView();

    void addTag(int index, const QString &tag);

    void contextMenuTriggered(QAction *action);

    void search(const QString& str);
    void modelNameFiltersChanged(const QStringList& nf);
};

#endif // DIRCONTROLLER_H
