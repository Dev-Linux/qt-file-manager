#pragma once
#ifndef FILEOPERATIONDATA_H
#define FILEOPERATIONDATA_H

#include <QtCore>

class FileOperationData
{
public:
    explicit FileOperationData();
    explicit FileOperationData(const QString &type,
                               const QStringList &source_list,
                               const QString& destination = QString(),
                               const QStringList &dest_names =
                                    QStringList(),
                               const QDateTime &date_time =
                                    QDateTime::currentDateTime());
    explicit FileOperationData(const FileOperationData& data);

    void set_date_time(const QDateTime &date_time);
    const QDateTime& date_time() const;

    void set_type(const QString &type);
    const QString& type() const;

    void set_source_list(const QStringList &source_list);
    const QStringList& source_list() const;

    void set_dest_names(const QStringList &dest_names);
    const QStringList& dest_names() const;

    void set_destination(const QString &destination);
    const QString& destination() const;

    bool any_ops_aborted;

    qint64 work_total;
    qint64 work_so_far;
    qint64 bytes_total;
    qint64 bytes_so_far;

private:
    QString m_type;
    QDateTime m_date_time;
    QStringList m_source_list;
    QStringList m_dest_names;
    QString m_destination;

};

#endif // FILEOPERATIONDATA_H
