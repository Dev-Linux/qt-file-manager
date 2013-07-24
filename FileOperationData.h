#pragma once
#ifndef FILEOPERATIONDATA_H
#define FILEOPERATIONDATA_H

#include <QtCore>

class FileOperationData
{
public:
    explicit FileOperationData();
    explicit FileOperationData(const QString &type,
                               const QStringList &sourceList,
                               const QString& destination = QString(),
                               const QStringList &destNames = QStringList(),
                               const QDateTime &dateTime = QDateTime::currentDateTime());
    explicit FileOperationData(const FileOperationData& data);

    void setDateTime(const QDateTime &dateTime);
    const QDateTime& dateTime() const;

    void setType(const QString &type);
    const QString& type() const;

    void setSourceList(const QStringList &sourceList);
    const QStringList& sourceList() const;

    void setDestNames(const QStringList &destNames);
    const QStringList& destNames() const;

    void setDestination(const QString &destination);
    const QString& destination() const;

    bool anyOperationsAborted;

    qint64 workTotal;
    qint64 workSoFar;
    qint64 bytesTotal;
    qint64 bytesSoFar;

signals:
    
public slots:
    QString m_type;
    QDateTime m_dateTime;
    QStringList m_sourceList;
    QStringList m_destNames;
    QString m_destination;
};

#endif // FILEOPERATIONDATA_H
