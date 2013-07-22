#include "DockModel.h"

#include "mainwindow.h"
#include "MainWindowController.h"

/**
 * @class DockModel
 * @note Does not allow duplicate paths.
 */

DockModel::DockModel(QObject *parent) :
    QObject(parent)
{
    s = MainWindowController::instance()->view->settings;
    Q_ASSERT(s != 0);
    readingSettings = false;
}

void DockModel::readSettings()
{
    readingSettings = true;
    int c = s->beginReadArray("important");
    for (int i = 0; i < c; i++) {
        s->setArrayIndex(i);
        auto path = s->value("path").toString();
        addPath(path);
    }
    s->endArray();
    readingSettings = false;
}

void DockModel::writeSettings()
{
    s->beginWriteArray("important");
    s->remove(""); // reset array
    for (int i = 0; i < list.size(); i++) {
        s->setArrayIndex(i);
        s->setValue("path", list[i].absoluteFilePath());
    }
    s->endArray();
}

bool DockModel::addPath(const QString &path)
{
    for (int i = 0; i < list.size(); i++) {
        if (list[i].absoluteFilePath() == path) {
            return false;
        }
    }

    FileInfo info(path);
    list << info;

    if (!readingSettings) {
        writeSettings();
    }

    emit added(info);

    return true;
}

void DockModel::removePath(const QString &path)
{
    FileInfo info;
    int index = -1;
    for (int i = 0; i < list.size(); i++) {
        if (list[i].absoluteFilePath() == path) {
            index = i;
            info = list[i];
            break;
        }
    }

    qDebug() << "index" << index;

    Q_ASSERT(index != -1);

    list.removeAt(index);

    writeSettings();

    emit removed(info);
}

bool DockModel::containsPath(const QString &path)
{
    FileInfo info(path);
    qDebug() << info.absoluteFilePath();
    for (auto i = list.begin(); i != list.end(); i++) {
        qDebug() << "|" << i->absoluteFilePath();
        if (*i == info) {
            return true;
        }
    }
    return false;
}
