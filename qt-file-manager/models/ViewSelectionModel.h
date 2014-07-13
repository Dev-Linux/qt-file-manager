#pragma once
#ifndef VIEWSELECTIONMODEL_H
#define VIEWSELECTIONMODEL_H

#include <QtCore>

class ViewSelectionModel : public QObject
{
    Q_OBJECT
public:
    explicit ViewSelectionModel();

    void add(int index);
    void remove(int index);
    void clear();
    bool is_empty();
    bool contains(int index);
    int one();
    int count();
    void save();

    QSet<int> saved_set;
    QSet<int> set;

signals:
    void changed(QSet<int> added, QSet<int> removed);

public slots:
    
};

#endif // VIEWSELECTIONMODEL_H
