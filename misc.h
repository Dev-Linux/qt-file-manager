#pragma once
#ifndef MISC_H
#define MISC_H

#include <QtCore>

#include "FileInfo.h"

#define debugType(x) std::cerr << #x << ": " << \
    typeid(x).name() << std::endl

#define metaDebug(x) qDebug() << #x << "=" << x

#ifdef Q_OS_WIN32
class COMError {
public:
    COMError(HRESULT hr);
    /**
     * @brief A @q{core,QString} describing the error.
     */
    QString err_str;
};

QDebug operator<<(QDebug dbg, const COMError &com_err);
#endif

namespace misc {
    void clearQLayout(QLayout *layout);

    void updateWithStyle(QWidget *w, QStyle *s);

    bool openLocalFile(QString path);

    QRect padRectangle(int padding, const QRect &r);

    QRectF unpadRect(const QRectF &r, qreal padding);

    QString escapeQDirNameFilter(const QString &nf);
    QList<FileInfo> filterByDirAbsPath(const QList<FileInfo> list,
                                       const QString& absDirPath);
    QList<QGraphicsItem *> filterByAncestor(const QList<QGraphicsItem *> items,
                                            const QGraphicsItem *anc);
    QString func_name_from_func_str(const QString &func_str);

    template<typename Graph, typename PositionMap, typename Topology>
    void min_max_coords(const Graph& g, PositionMap position, const Topology& topology,
                        int &minx, int &miny, int &maxx, int &maxy);

    template<typename Graph, typename PositionMap, typename Topology>
    void print_graph_layout(const Graph& g, PositionMap position, const Topology& topology);
}

/**
 * \todo integrate DebugFilter and MessageHandler
 */

class DebugFilter {
public:
    static void set(const char *func_name);
    static void clear();

private:
    DebugFilter() {}
    static QString filter_func_name;
    friend class MessageHandler;
};

class MessageHandler {
public:
    static void msgHandler(QtMsgType type,
                           const QMessageLogContext &context,
                           const QString &msg);
private:
    MessageHandler() {} // prevent creation of instances
    static QString last_func_name;
};

#endif // MISC_H
