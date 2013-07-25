#pragma once
#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QtWidgets>

class View;

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(View *v);
    View *v;
    
protected:
    virtual QSize sizeHint() const;
    virtual void paintEvent(QPaintEvent *);

signals:
    
public slots:
    
};

#endif // VIEWWIDGET_H
