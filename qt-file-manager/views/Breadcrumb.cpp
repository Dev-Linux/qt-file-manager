#include "Breadcrumb.h"

#include "models/DockModel.h"

Breadcrumb::Breadcrumb(DockModel *dockModel) : QWidget()
{
    this->m_dock_model = dockModel;

    m_hbox = new QHBoxLayout();
    m_hbox->setSpacing(2);
    m_hbox->setContentsMargins(0, 0, 0, 0);

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
            this, &Breadcrumb::button_clicked);

    m_hbox->addWidget(root);
    m_hbox->addSpacerItem(
                new QSpacerItem(0, 0, QSizePolicy::Expanding,
                                QSizePolicy::Fixed));
    this->setLayout(m_hbox);
}

void Breadcrumb::set_path(QString path)
{
    this->m_path = path;

    QLayoutItem* item;
    while (m_hbox->count() > 2) {
        item = m_hbox->takeAt(1);
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

        if (m_dock_model->contains_path(sum)) {
            button->setProperty("highlighted", true);
        }

        button->setProperty("path", sum);
        connect(button, &QPushButton::clicked,
                this, &Breadcrumb::button_clicked);

        m_hbox->insertWidget(i - bi + 1, button);
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

void Breadcrumb::button_clicked()
{
    QPushButton *b = dynamic_cast<QPushButton*>(sender());
    this->m_path = b->property("path").toString();
    emit path_changed(this->m_path);
}
