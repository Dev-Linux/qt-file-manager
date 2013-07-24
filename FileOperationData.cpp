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
    setType(type);
    setDateTime(dateTime);
    setSourceList(sourceList);
    setDestNames(destNames);
    setDestination(destination);

    anyOperationsAborted = false;
    workTotal = workSoFar = 0;
    bytesTotal = bytesSoFar = 0;
}

FileOperationData::FileOperationData(const FileOperationData &data)
{
    setType(data.type());
    setDateTime(data.dateTime());
    setSourceList(data.sourceList());
    setDestNames(data.destNames());
    setDestination(data.destination());
    anyOperationsAborted = data.anyOperationsAborted;
    workTotal = data.workTotal;
    workSoFar = data.workSoFar;
    bytesTotal = data.bytesTotal;
    bytesSoFar = data.bytesSoFar;
}

void FileOperationData::setDateTime(const QDateTime &dateTime)
{
    m_dateTime = dateTime;
}

const QDateTime& FileOperationData::dateTime() const
{
    return m_dateTime;
}

void FileOperationData::setType(const QString &type)
{
    m_type = type;
}

const QString& FileOperationData::type() const
{
    return m_type;
}

void FileOperationData::setSourceList(const QStringList &sourceList)
{
    m_sourceList = sourceList;
}

const QStringList& FileOperationData::sourceList() const
{
    return m_sourceList;
}

void FileOperationData::setDestNames(const QStringList &destNames)
{
    m_destNames = destNames;
}

const QStringList& FileOperationData::destNames() const
{
    return m_destNames;
}

void FileOperationData::setDestination(const QString &destination)
{
    m_destination = destination;
}

const QString& FileOperationData::destination() const
{
    return m_destination;
}
