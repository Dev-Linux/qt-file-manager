#include "ViewSelectionModel.h"

ViewSelectionModel::ViewSelectionModel(QObject *parent) :
    QObject(parent)
{
}

void ViewSelectionModel::add(int index)
{
    this->set << index;
}

void ViewSelectionModel::remove(int index)
{
    this->set.remove(index);
}

void ViewSelectionModel::clear()
{
    this->set.clear();
}

bool ViewSelectionModel::isEmpty()
{
    return this->set.isEmpty();
}

bool ViewSelectionModel::contains(int index)
{
    return this->savedSet.contains(index);
}

int ViewSelectionModel::one()
{
    QSetIterator<int> i(this->set);
    return i.next();
}

int ViewSelectionModel::count()
{
    return this->set.count();
}

void ViewSelectionModel::save()
{
    QSet<int> added(this->set);
    QSet<int> removed(this->savedSet);
    added.subtract(this->savedSet);
    removed.subtract(this->set);
    this->savedSet = this->set;
    emit changed(added, removed);
}
