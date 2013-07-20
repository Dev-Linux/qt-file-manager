#include "breadcrumb.h"

#include "DockModel.h"

Breadcrumb::Breadcrumb(DockModel *dockModel) : QWidget()
{
    this->dockModel = dockModel;

    hbox = new QHBoxLayout();
    hbox->setSpacing(2);
    hbox->setContentsMargins(0, 0, 0, 0);

    QIcon icon(":/root.png");
    QPushButton* root = new QPushButton(icon, "");
    root->setIconSize(QSize(20, root->fontMetrics().height()));
    root->setObjectName("root");
#ifdef Q_OS_WIN32
    root->setProperty("path", QVariant(""));
#else
    root->setProperty("path", QVariant("/"));
#endif

    connect(root, &QPushButton::clicked,
            this, &Breadcrumb::buttonClicked);

    hbox->addWidget(root);
    hbox->addSpacerItem(
                new QSpacerItem(0, 0, QSizePolicy::Expanding,
                                QSizePolicy::Fixed));
    this->setLayout(hbox);
}

void Breadcrumb::setPath(QString path)
{
    this->path = path;

    QLayoutItem* item;
    while (hbox->count() > 2) {
        item = hbox->takeAt(1);
        delete item->widget();
        delete item;
    }

    if (path != "") {
        QFileInfo fi(path);
        path = fi.absoluteFilePath();
    }
    QStringList list = path.split('/',
                    QString::SkipEmptyParts);
#ifdef Q_OS_WIN32
    QString sum("");
#else
    QString sum("/");
#endif
    QPushButton *button;
    auto bi = list.begin();
    for (auto i = bi; i != list.end(); ++i) {
        sum += *i + "/";

        button = new QPushButton(*i);

        if (dockModel->containsPath(sum)) {
            button->setProperty("highlighted", true);
        }

        button->setProperty("path", sum);
        connect(button, &QPushButton::clicked,
                this, &Breadcrumb::buttonClicked);

        hbox->insertWidget(i - bi + 1, button);
    }
}

void Breadcrumb::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    if (event->button() == Qt::LeftButton) {
        emit clicked();
        event->accept();
    }
}

void Breadcrumb::buttonClicked()
{
    QPushButton *b = dynamic_cast<QPushButton*>(sender());
    this->path = b->property("path").toString();
    emit pathChanged(this->path);
}
