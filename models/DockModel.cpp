#include "DockModel.h"

#include "views/MainWindow.h"

#include "controllers/MainWindowController.h"

#include "Application.h"

/**
 * @class DockModel
 * @note Does not allow duplicate paths.
 */

DockModel::DockModel(QObject *parent) :
    QObject(parent)
{
    s = Application::instance()->settings;
    Q_ASSERT(s != 0);
    reading_settings = false;
}

void DockModel::read_settings()
{
    reading_settings = true;
    int c = s->beginReadArray("important");
    for (int i = 0; i < c; i++) {
        s->setArrayIndex(i);
        auto path = s->value("path").toString();
        add_path(path);
    }
    s->endArray();
    reading_settings = false;
}

void DockModel::write_settings()
{
    s->beginWriteArray("important");
    s->remove(""); // reset array
    for (int i = 0; i < list.size(); i++) {
        s->setArrayIndex(i);
        s->setValue("path", list[i].abs_file_path());
    }
    s->endArray();
}

bool DockModel::add_path(const QString &path)
{
    for (int i = 0; i < list.size(); i++) {
        if (list[i].abs_file_path() == path) {
            return false;
        }
    }

    FileInfo info(path);
    list << info;

    if (!reading_settings) {
        write_settings();
    }

    emit added(info);

    return true;
}

void DockModel::remove_path(const QString &path)
{
    FileInfo info;
    int index = -1;
    for (int i = 0; i < list.size(); i++) {
        if (list[i].abs_file_path() == path) {
            index = i;
            info = list[i];
            break;
        }
    }

    qDebug() << "index" << index;

    Q_ASSERT(index != -1);

    list.removeAt(index);

    write_settings();

    emit removed(info);
}

bool DockModel::contains_path(const QString &path)
{
    FileInfo info(path);
    for (auto i = list.begin(); i != list.end(); ++i) {
        if (*i == info) {
            return true;
        }
    }
    return false;
}
