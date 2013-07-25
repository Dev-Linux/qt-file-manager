#pragma once
#ifndef VIEWSELECTIONMODEL_H
#define VIEWSELECTIONMODEL_H

#include <QtCore>

class ViewSelectionModel : public QObject
{
    Q_OBJECT
public:
    explicit ViewSelectionModel(QObject *parent = 0);

    QSet<int> savedSet;
    QSet<int> set;
    void add(int index);
    void remove(int index);
    void clear();
    bool isEmpty();
    bool contains(int index);
    int one();
    int count();
    void save();
signals:
    void changed(QSet<int> added, QSet<int> removed);
public slots:
    
};

#endif // VIEWSELECTIONMODEL_H
