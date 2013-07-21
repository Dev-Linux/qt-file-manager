#include "misc.h"

#include <QDesktopServices>

// http://qt-project.org/forums/viewthread/22993/
// https://bugreports.qt-project.org/browse/QTBUG-29331
// http://stackoverflow.com/questions/15056834/qt5-migration-and-boost-if-hpp-macro-argument-mismatch-bug
#ifndef Q_MOC_RUN

#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/point_traits.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>

#endif // Q_MOC_RUN

/**
 * @class COMError
 * @brief A wrapper for COM errors [Windows].
 * @todo Create 1+ Windows-only file(s) containing at least a namespace like
 * @c misc_win and integrate this and other Win-only things in it. Ideally,
 * Headers shouldn't have any conditional compilations. This should be on the
 * implementation part.
 */

#ifdef Q_OS_WIN32
#include <Windows.h>
#include <comdef.h>

/**
 * @brief Constructs the instance and sets its err_str.
 * @param hr The result of the COM call.
 */
COMError::COMError(HRESULT hr) {
    _com_error err(hr);
    err_str = QString::fromWCharArray
            (err.ErrorMessage());
}

/**
  * @brief Allows qDebugging of COMError instances (prints their error string).
  * @param dbg The @q{core,QDebug} instance (returned by calls to qDebug())
  * @param com_err The COMError to output.
  * @return The same QDebug instance @a dbg, for chained calls.
  */
QDebug operator<<(QDebug dbg, const COMError &com_err)
{
    dbg.nospace() << "COMError(" << com_err.err_str << ")";
    return dbg.space();
}
#endif

/**
 * \brief Removes and destroys all widgets and layouts in @a layout.
 * \param layout The QLayout to clear.
 * \post layout->count() == 0
 */
void misc::clearQLayout(QLayout *layout)
{
    QLayoutItem *item;
    while((item = layout->takeAt(0))) {
        if (item->layout()) {
            misc::clearQLayout(item->layout());
            delete item->layout();
        }
        delete item->widget();
    }
}

/**
 * @brief Updates QWidget @a w using QStyle @a s.
 * @param w The widget to update.
 * @param s The style to use when updating.
 */
void misc::updateWithStyle(QWidget *w, QStyle *s)
{
    s->unpolish(w);
    s->polish(w);
    w->update();
}

/**
 * \brief Opens the local file/directory at @a path with the default app for
 * it's type.
 * \param path The path of the local file to open.
 * \retval true @a path was successfully opened.
 * \retval false An error occured.
 * \bug [win] Fișierele DLL nu se deschid (eroare 31 - nu există program
 * care să le deschidă)
 */
bool misc::openLocalFile(QString path)
{
    return QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

/**
 * @brief Pads @a r with @a padding pixels on each side and returns the
 * new rectangle.
 * @param padding The number of pixels to add on each side of @a r.
 * @param r The rectangle to copy and pad.
 * @return The padded rectangle.
 * @see misc::unpadRect()
 */
QRect misc::padRectangle(int padding, const QRect &r)
{
    int x = r.x(), y = r.y(),
        w = r.width(), h = r.height();
    QRect rr(x - padding, y - padding,
             w + 2 * padding, h + 2 * padding);
    return rr;
}

/**
 * @brief Unpads @a r by removing @a padding units from each side and
 * returns the new rectangle.
 * @param padding The number of pixels to remove from each side of @a r.
 * @param r The rectangle to copy and unpad.
 * @return The unpadded rectangle.
 * @see misc::padRectangle()
 */
QRectF misc::unpadRect(const QRectF &r, qreal padding)
{
    qreal x = r.x(), y = r.y(),
        w = r.width(), h = r.height();
    QRectF rr(x + padding, y + padding,
             w - 2 * padding, h - 2 * padding);
    return rr;
}

/**
 * \brief Scans a graph and returns the minimum and maximum x and y coordinates
 * of its vertices.
 * \tparam Graph The type of the boost graph (e.g. adjacency list).
 * \tparam PositionMap The type of the structure used to hold the positions of
 * the vertices.
 * \tparam Topology The type of the topology used to layout the graph.
 * \param g The boost graph (adjacency list).
 * \param position The position map which stores vertices' positions.
 * \param topology The topology which describes vertices' point type.
 * \param[out] minx The minimum x coordinate found.
 * \param[out] miny The minimum y coordinate found.
 * \param[out] maxx The maximum x coordinate found.
 * \param[out] maxy The maximum y coordinate found.
 * \todo Make BoostGraph generic and move this and print_graph_layout() there.
 */
template<typename Graph, typename PositionMap, typename Topology>
void misc::min_max_coords(const Graph& g, PositionMap position, const Topology& topology,
                    int &minx, int &miny, int &maxx, int &maxy) {
    using namespace boost;
    typedef typename Topology::point_type Point;
    // Find min/max ranges
    Point min_point = position[*vertices(g).first], max_point = min_point;
    BGL_FORALL_VERTICES_T(v, g, Graph) {
      min_point = topology.pointwise_min(min_point, position[v]);
      max_point = topology.pointwise_max(max_point, position[v]);
    }
    minx = min_point[0];
    miny = min_point[1];
    maxx = max_point[0];
    maxy = max_point[1];
}

/**
 * \brief Prints a graph layout on the console.
 * \tparam Graph The type of the boost graph (e.g. adjacency list).
 * \tparam PositionMap The type of the structure used to hold the positions of
 * the vertices.
 * \tparam Topology The type of the topology used to layout the graph.
 * \param g The boost graph (adjacency list).
 * \param position The position map which stores vertices' positions.
 * \param topology The topology which describes vertices' point type.
 */
template<typename Graph, typename PositionMap, typename Topology>
void misc::print_graph_layout(const Graph& g, PositionMap position,
                              const Topology& topology)
{
    using namespace boost;
    typedef typename Topology::point_type Point;

    int minx, miny, maxx, maxy;
    min_max_coords(g, position, topology, minx, miny, maxx, maxy);

    qDebug() << "min (" << minx << miny << ")";
    qDebug() << "max (" << maxx << maxy << ")";

    for (int y = miny; y <= maxy; ++y) {
        for (int x = minx; x <= maxx; ++x) {
            typename graph_traits<Graph>::vertex_iterator vi, vi_end;
            // Find vertex at this position
            typename graph_traits<Graph>::vertices_size_type index = 0;
            for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi, ++index) {
                if ((int)position[*vi][0] == x && (int)position[*vi][1] == y)
                    break;
            }

            if (vi == vi_end) std::cout << ' ';
            else std::cout << (char)(index + 'A');
        }
        std::cout << std::endl;
    }
}

/**
 * @brief Escapes special chars (?*[]) in QDir name filters (simple mode
 * QRegExp).
 * @param nf The string to escape.
 * @return The escaped string.
 */
QString misc::escapeQDirNameFilter(const QString &nf)
{
    QString s = nf;
    s.replace("?", "\\?");
    s.replace("*", "\\*");
    s.replace("[", "\\]");
    s.replace("[", "\\]");
    return s;
}

/**
 * @brief Filters a list of QGraphicsItem* by keeping only items which are
 * descendants of item @a anc.
 * @param items The list of QGraphicsItem* to filter.
 * @param anc The ancestor to use as a filter.
 * @return The filtered QList<QGraphicsItem *>
 */
QList<QGraphicsItem *> misc::filterByAncestor(
        const QList<QGraphicsItem *> items, const QGraphicsItem *anc)
{
    QList<QGraphicsItem *> list;
    for (int i = 0; i < items.size(); i++) {
        if (anc->isAncestorOf(items[i])) {
            list << items[i];
        }
    }
    return list;
}

/**
 * \brief Filters a list of FileInfo by absoulte directory path @a absDirPath.
 * \param list The list of FileInfo to filter.
 * \param absDirPath The absolute directory path to use as a filter.
 * \return A list containing only the FileInfo which have \a absDirPath as the
 * absolute path of their directory.
 */
QList<FileInfo> misc::filterByDirAbsPath(QList<FileInfo> list,
                                         const QString &absDirPath)
{
    QList<FileInfo> l;
    for (auto i = list.begin(); i != list.end(); ++i) {
        if (i->absoluteDir().absolutePath() == absDirPath) {
            l << *i;
        }
    }
    return l;
}

/**
 * @brief Parses a function string as given by the compiler and produces a
 * function name (no return type, no parameter list).
 * @todo make this work better for lambdas, if possible
 * @param func_str The function string given by compiler.
 * @return The function name (no return type, no parameter list).
 */
QString misc::func_name_from_func_str(const QString &func_str) {
    QString s = func_str;
    int sp = -1; // index of last space before '('
    int op = -1; // index of first '('
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == ' ') {
            sp = i;
        } else if (s[i] == '(' && op == -1) {
            op = i;
            break;
        }
    }

    if (op != -1) {
        s.truncate(op);
    }
    if (sp != -1) {
        s = s.mid(sp + 1);
    }
    int lpr = -1; // index of last '*' (pointer) or '&' (reference) char
    for (int i = 0; i < s.size(); i++) {
        if (s[i] == '*' || s[i] == '&') {
            lpr = i;
        }
    }
    if (lpr != -1) {
        s = s.mid(lpr + 1);
    }
    return s;
}

QString DebugFilter::filter_func_name;

/**
 * @brief Sets the debug filter as @a func_name. After this, only qDebugs from
 * function/method with function name @a func_name will be shown.
 * @param func_name The function name to set as the debug filter.
 */
void DebugFilter::set(const char *func_name)
{
    filter_func_name =
            misc::func_name_from_func_str(QString::fromUtf8(func_name));
}

/**
 * @brief Clears the debug filter set with DebugFilter::set.
 */
void DebugFilter::clear()
{
    filter_func_name.clear();
}

QString MessageHandler::last_func_name;

/**
 * @brief Custom message handler for Qt's logging system (qDebug etc.).
 * Filters functions by DebugFilter::filter_func_name, if not empty, and doesn't
 * repeatedly output the function name if more qDebug calls are made sequentially
 * from the same function.
 */
void MessageHandler::msgHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &msg) {
    QString s = misc::func_name_from_func_str(context.function);

    if (DebugFilter::filter_func_name.isEmpty() ||
            DebugFilter::filter_func_name == s) {
        if (s != last_func_name) {
            last_func_name = s;
            s = s.rightJustified(40);
        } else {
            s = QString(40, ' ');
        }

        QString output = QString("%1 | %2").arg(s, msg);
        std::cerr << output.toStdString() << std::endl;
    }
    if (type == QtFatalMsg) {
        abort();
    }
}
