#pragma once
#ifndef LISTVIEWITEM_H
#define LISTVIEWITEM_H

#include <QtWidgets>
#include "FileInfo.h"

/*class Worker : public QObject {
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0) : QObject(parent) { }
signals:
    void work();
public slots:
    void trigger() {
        emit work();
    }
};*/

class ListViewItem : public QWidget
{
    Q_OBJECT
public:
    explicit ListViewItem(const FileInfo &fileInfo);
    FileInfo fileInfo;
    QLabel* l;
    void setSelected(bool selected);
    void setHighlighted(bool set);
    bool isSelected();
    bool isHighlighted();

    //static QThread *thread;
    //static Worker worker;
protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
signals:
    void clicked(const Qt::KeyboardModifiers &modifiers);
    void rightClicked(const QPoint &globalPos);
    void doubleClicked();

public slots:
};

#endif // LISTVIEWITEM_H
