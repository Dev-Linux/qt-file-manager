#include "FileOperationData.h"

FileOperationData::FileOperationData()
{
    FileOperationData("", QStringList());
}

FileOperationData::FileOperationData(const QString &type,
                                     const QStringList &sourceList,
                                     const QString &destination,
                                     const QStringList &destNames,
                                     const QDateTime &dateTime)
{
    set_type(type);
    set_date_time(dateTime);
    set_source_list(sourceList);
    set_dest_names(destNames);
    set_destination(destination);

    any_ops_aborted = false;
    work_total = work_so_far = 0;
    bytes_total = bytes_so_far = 0;
}

FileOperationData::FileOperationData(const FileOperationData &data)
{
    set_type(data.type());
    set_date_time(data.date_time());
    set_source_list(data.source_list());
    set_dest_names(data.dest_names());
    set_destination(data.destination());
    any_ops_aborted = data.any_ops_aborted;
    work_total = data.work_total;
    work_so_far = data.work_so_far;
    bytes_total = data.bytes_total;
    bytes_so_far = data.bytes_so_far;
}

void FileOperationData::set_date_time(const QDateTime &dateTime)
{
    m_date_time = dateTime;
}

const QDateTime& FileOperationData::date_time() const
{
    return m_date_time;
}

void FileOperationData::set_type(const QString &type)
{
    m_type = type;
}

const QString& FileOperationData::type() const
{
    return m_type;
}

void FileOperationData::set_source_list(const QStringList &sourceList)
{
    m_source_list = sourceList;
}

const QStringList& FileOperationData::source_list() const
{
    return m_source_list;
}

void FileOperationData::set_dest_names(const QStringList &destNames)
{
    m_dest_names = destNames;
}

const QStringList& FileOperationData::dest_names() const
{
    return m_dest_names;
}

void FileOperationData::set_destination(const QString &destination)
{
    m_destination = destination;
}

const QString& FileOperationData::destination() const
{
    return m_destination;
}
