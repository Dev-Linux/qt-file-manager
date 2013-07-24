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
    QLabel *dateLabel;
    FileOperationData* fileOperationData;

    void setLabel(QString str);
    QString label();

    void setDateTime(QDateTime dateTime);
    QDateTime dateTime();

    void updateView();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    
public slots:

private:
    QString m_label;
    QDateTime m_dateTime;
    QLabel *labelWidget;
};

#endif // FILEOPERATIONITEM_H
