#pragma once
#ifndef FILEOPERATIONITEM_H
#define FILEOPERATIONITEM_H

#include <QtWidgets>

class FileOperationData;

class FileOperationItem : public QWidget
{
    Q_OBJECT

public:
    explicit FileOperationItem(FileOperationData* data,
                               const QString &str = QString(),
                               QWidget *parent = 0);

    QString verb;
    //bool dateVisible = false;
    QLabel *date_label;
    FileOperationData* file_op_data;

    void set_label(QString str);
    QString label();

    void set_date_time(QDateTime date_time);
    QDateTime date_time();

    void update_view();

protected:
    virtual void paintEvent(QPaintEvent *event);

signals:
    
public slots:

private:
    QString m_label;
    QDateTime m_dateTime;
    QLabel *m_label_widget;
};

#endif // FILEOPERATIONITEM_H
