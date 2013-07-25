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
    labelWidget = new QLabel();
    dateLabel = new QLabel();
    QPushButton *removeButton = new QPushButton("✗");

    leftSide->setLayout(vbox);
    leftSide->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::MinimumExpanding);
    setLayout(hbox);
    labelWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                               QSizePolicy::MinimumExpanding);
    vbox->setContentsMargins(0, 0, 0, 0);
    hbox->setContentsMargins(0, 0, 0, 0);
    dateLabel->hide();
    this->setContentsMargins(4, 4, 4, 4);

    /**
      * \bug make it fixed size by not hiding any label. instead, use
      * dateLabel as a status label with it showing date when the operation is
      * finished, or otherwise percents and other status info
      */

    vbox->addWidget(labelWidget);
    vbox->addWidget(dateLabel);
    hbox->addWidget(leftSide);
    hbox->addWidget(removeButton);

    if (!str.isEmpty()) {
        setLabel(str);
    }
    setDateTime(data->dateTime());

    if (data->type() == "copy") verb = "Copying";
    else if (data->type() == "move") verb = "Moving";
    else if (data->type() == "delete") verb = "Deleting";

    this->fileOperationData = data;
    updateView();
}

/**
 * @todo FileOperationItem static method to build friendly date-time strings
 * (e.g. 2 minutes ago)
 */

void FileOperationItem::setLabel(QString str)
{
    m_label = str;
}

QString FileOperationItem::label()
{
    return m_label;
}

void FileOperationItem::setDateTime(QDateTime dateTime)
{
    m_dateTime = dateTime;
}

QDateTime FileOperationItem::dateTime()
{
    return m_dateTime;
}

void FileOperationItem::updateView()
{
    labelWidget->setText(verb + " " + label());
    dateLabel->setText("<em>" + dateTime().toString() + "</em>");
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

    if (fileOperationData->workTotal > 0 &&
            fileOperationData->workSoFar < fileOperationData->workTotal) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QColor(116, 228, 164));
        painter.setBrush(QBrush(QColor(116, 228, 164)));
        //double itemProgress = (double) fileOperationData->workSoFar / fileOperationData->workTotal;
        double sizeProgress = (double) fileOperationData->bytesSoFar / fileOperationData->bytesTotal;
        double progress;// = (itemProgress + sizeProgress * 3.0) / 4.0;
        progress = double(qRound(sizeProgress * 100.0)) / 100.0;
        painter.drawRect(0, 0, progress * this->width(), this->height());
    }

    QWidget::paintEvent(event);
}
