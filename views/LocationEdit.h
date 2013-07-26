#pragma once
#ifndef LOCATIONEDIT_H
#define LOCATIONEDIT_H

#include <QtWidgets>

class LocationEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LocationEdit(QWidget *parent = 0);

    bool context_menu_open;
    
signals:
    void focused(bool has_focus);
    void button_clicked();

protected:
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);
    virtual void resizeEvent(QResizeEvent *);
    virtual void contextMenuEvent(QContextMenuEvent *event);

public slots:
    
private:
    QToolButton *m_button;
};

#endif // LOCATIONEDIT_H
