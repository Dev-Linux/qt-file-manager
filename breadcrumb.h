#pragma once
#ifndef BREADCRUMB_H
#define BREADCRUMB_H

#include <QtWidgets>

class DockModel;

class Breadcrumb : public QWidget
{
    Q_OBJECT
public:
    explicit Breadcrumb(DockModel *dockModel);
    void setPath(QString path);
    
signals:
    void clicked();
    void pathChanged(QString);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

public slots:

private slots:
    void buttonClicked();
    
private:
    QHBoxLayout *hbox;
    QString path;
    DockModel *dockModel;
};

#endif // BREADCRUMB_H
