#pragma once
#ifndef BREADCRUMB_H
#define BREADCRUMB_H

#include <QtWidgets>

class DockModel;

class Breadcrumb : public QWidget
{
    Q_OBJECT
public:
    explicit Breadcrumb(DockModel *dock_model);
    void set_path(QString path);
    
signals:
    void clicked();
    void path_changed(QString);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

public slots:

private slots:
    void button_clicked();
    
private:
    QHBoxLayout *m_hbox;
    QString m_path;
    DockModel *m_dock_model;
};

#endif // BREADCRUMB_H
