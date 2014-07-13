#include "quickfilegraphview.h"

QuickFileGraphView::QuickFileGraphView(FileSystemModel *fsModel) :
    QQuickView()
{
    this->fsModel = fsModel;
    this->setResizeMode(QQuickView::SizeRootObjectToView);

    // qmlRegisterType<QFileInfo>();
    this->rootContext()->setContextProperty("fileSystemModel", fsModel);
    this->rootContext()->setContextProperty("fileGraphView", this);
    this->setSource(QUrl::fromLocalFile("../OlympicExplorer/filegraph.qml"));
}

QQmlListProperty<QFileInfo> QuickFileGraphView::driveList()
{
    drives = new QFileInfoList(QDir::drives());

    drivesPointers.clear();
    for (int i = 0; i < drives->length(); i++) {
        QFileInfo fi = drives->at(i);
        QFileInfo*  fi2 = &fi;
        drivesPointers.append(fi2);
    }
    // nu merge fiindca nu se poate qmlRegisterType-iza QFileInfo
    // fiindca nu deriva din QObject ceea ce este necesar pt QML
    return QQmlListProperty<QFileInfo>(this, drivesPointers);
}

QWidget *QuickFileGraphView::containerWidget()
{
    QWidget *container = QWidget::createWindowContainer(this);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return container;
}
