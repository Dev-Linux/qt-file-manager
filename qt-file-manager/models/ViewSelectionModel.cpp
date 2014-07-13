#include "ViewSelectionModel.h"

ViewSelectionModel::ViewSelectionModel() :
    QObject()
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

bool ViewSelectionModel::is_empty()
{
    return this->set.isEmpty();
}

bool ViewSelectionModel::contains(int index)
{
    return this->saved_set.contains(index);
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
    QSet<int> removed(this->saved_set);
    added.subtract(this->saved_set);
    removed.subtract(this->set);
    this->saved_set = this->set;
    emit changed(added, removed);
}
