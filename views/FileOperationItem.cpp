#include "FileOperationItem.h"

#include "views/MainWindow.h"

#include "models/FileOperationData.h"

FileOperationItem::FileOperationItem(FileOperationData* data,
                                     const QString &str,
                                     QWidget *parent) :
    QWidget(parent)
{
    /* this (hbox) -> leftSide (vbox) -> labelWidget
                                      -> dateLabel
                   -> removeButton
    */

    QWidget *leftSide = new QWidget();
    QHBoxLayout *hbox = new QHBoxLayout();
    QVBoxLayout *vbox = new QVBoxLayout();
    m_label_widget = new QLabel();
    date_label = new QLabel();
    QPushButton *removeButton = new QPushButton("✗");

    leftSide->setLayout(vbox);
    leftSide->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
    setLayout(hbox);
    m_label_widget->setSizePolicy(QSizePolicy::MinimumExpanding,
                               QSizePolicy::MinimumExpanding);
    vbox->setContentsMargins(0, 0, 0, 0);
    hbox->setContentsMargins(0, 0, 0, 0);
    date_label->hide();
    this->setContentsMargins(4, 4, 4, 4);

    /**
      * \bug make it fixed size by not hiding any label. instead, use
      * dateLabel as a status label with it showing date when the operation is
      * finished, or otherwise percents and other status info
      */

    vbox->addWidget(m_label_widget);
    vbox->addWidget(date_label);
    hbox->addWidget(leftSide);
    hbox->addWidget(removeButton);

    if (!str.isEmpty()) {
        set_label(str);
    }
    set_date_time(data->date_time());

    if (data->type() == "copy") verb = "Copying";
    else if (data->type() == "move") verb = "Moving";
    else if (data->type() == "delete") verb = "Deleting";

    this->file_op_data = data;
    update_view();
}

/**
 * @todo FileOperationItem static method to build friendly date-time strings
 * (e.g. 2 minutes ago)
 */

void FileOperationItem::set_label(QString str)
{
    m_label = str;
}

QString FileOperationItem::label()
{
    return m_label;
}

void FileOperationItem::set_date_time(QDateTime dateTime)
{
    m_dateTime = dateTime;
}

QDateTime FileOperationItem::date_time()
{
    return m_dateTime;
}

void FileOperationItem::update_view()
{
    m_label_widget->setText(verb + " " + label());
    date_label->setText("<em>" + date_time().toString() + "</em>");
}

void FileOperationItem::paintEvent(QPaintEvent *event)
{
    /**
     * @note mai pot sa folosesc @q{widgets,QProgressBar} pentru a arata
     * progresul operatiunii:
     * http://qt-project.org/doc/qt-5.0/qtwidgets/qprogressbar.html . Ma inspir
     * si din Firefox
     */

    this->parentWidget()->adjustSize();

    if (file_op_data->work_total > 0 &&
            file_op_data->work_so_far < file_op_data->work_total) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QColor(116, 228, 164));
        painter.setBrush(QBrush(QColor(116, 228, 164)));
        //double itemProgress = (double) fileOperationData->workSoFar / fileOperationData->workTotal;
        double sizeProgress = (double) file_op_data->bytes_so_far / file_op_data->bytes_total;
        double progress;// = (itemProgress + sizeProgress * 3.0) / 4.0;
        progress = double(qRound(sizeProgress * 100.0)) / 100.0;
        painter.drawRect(0, 0, progress * this->width(), this->height());
    }

    QWidget::paintEvent(event);
}
