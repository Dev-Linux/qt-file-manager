#include "BoostGraph.h"

#include <functional>
#include <typeinfo>

/**
 * \class BoostGraph
 * \brief A small wrapper for Boost Graphs.
 *
 * **Small BGL doc**
 * \code
 * // g - Graph, adjacency_list etc.
 *
 * VertexID vid = boost::add_vertex(g); // vertex_descriptor vid;
 * g[vid] // - structura definită de mine în .h
 * g[vid].index = index;
 *
 * // asta o fi accesarea prin indice (vertex_index)
 * a = vertex(0, g);
 * b = vertex(1, g);
 *
 * vertex_iterator vi = boost::vertices(g).first; // început
 * vertices_size_type count = (model->count() - un int sau ceva de acest fel);
 * typename graph_traits<Graph>::vertex_iterator vi_end; // ca și vertex_iterator, cred
 *
 * edge_descriptor ed; // pt. muchii
 * bool ok;
 * boost::tie(ed, ok) = boost::add_edge(a, b, g);
 *
 * boost::tie(vi, vi_end) = vertices(g);
 * // echivalent cu vi = vertices(g).first; vi_end = vertices(g).second
 *
 * for (vertices_size_type i = 0; i < count; ++i, ++vi) {
 *   boost::put(vertex_index, g, *vi, i);
 *   // setez proprietatea vertex_index a elementului *vi din graful g la i
 * }
 *
 * Graph(int x); // ctor - graph with x nodes
 * \endcode
 */

/**
 * @brief Creates a graph with @a vertices vertices.
 * @param vertices Number of vertices of the new graph.
 */
BoostGraph::BoostGraph(int vertices) :
    QObject()
{
    g = new Graph(vertices);
}

/**
 * @brief Sets the topology of the graph layout as the rectangle defined by
 * @a x, @a y, @a width and @a height.
 * @param x
 * @param y
 * @param width
 * @param height
 */
void BoostGraph::setRectangle(qreal x, qreal y,
                              qreal width, qreal height)
{
    topology = new rectangle_topology<>(x, y, width, height);
}

/** \brief Returns the position map of the graph.
 * \returns The position map of the graph.
 *
 * Random info: `include \<typeinfo\>, typeid(variable).name()` = mangled type
 * name. Use \c c++flit to demangle it.
 *
 * Demangled return value of `get(vertex_position, *g)`:
 *
 * \code
 * boost::adj_list_vertex_property_map
 * <boost::adjacency_list
 * <boost::listS, boost::listS, boost::directedS,
 * boost::property<boost::vertex_index_t, int,
 * boost::property<vertex_position_t, boost::convex_topology<2ul>::point,
 * boost::no_property> >, boost::property<boost::edge_weight_t, double,
 * boost::no_property>, boost::no_property, boost::listS>,
 * boost::convex_topology<2ul>::point,
 * boost::convex_topology<2ul>::point&,
 * vertex_position_t>
 * \endcode
 *
 * and so I typedef'd it in position_map
 */
position_map BoostGraph::getPos()
{
    return get(vertex_position, *g);
}

/**
 * @brief Calls gursoy_atun_layout (Boost).
 * @pre @a topology should be set with setRectangle().
 * @param pos The position map in which to store the coordinates.
 */
void BoostGraph::doLayout(position_map pos)
{
    //int radius = (qMin(parentWidget()->width(), parentWidget()->height()) - 100) / 2;
    //circle_graph_layout(g, get(vertex_position, g), radius);

    //print_graph_layout(g, get(vertex_position, g), topology);

    //min_max_coords(g, pos, topology, minx, miny, maxx, maxy);
    //qDebug() << "width" << width() << "height" << height();

    // without T does not use typename outside of template (no T-raits,
    // just types, I guess)
    // BGL_FORALL_VERTICES
    /*BGL_FORALL_EDGES(e, g, Graph) { }*/

    // the coordinates of the file node widget
    //int xx, yy;
    //randomTry(xx, yy);
    // now it doesn't overlap with anything and it can be added to the view at (xx,yy)

    Q_ASSERT(topology != nullptr);

    gursoy_atun_layout(*g, *topology, pos);
}

/**
 * @brief Returns the index of the vertex @a v.
 * @param v A vertex descriptor.
 * @return Index of the vertex @a v.
 */
int BoostGraph::vertexIndex(vertex_descriptor v)
{
    return get(vertex_index, *g, v);
}

/**
 * @brief Sets @a index as the index of the vertex @a v.
 * @param v A vertex descriptor
 * @param index The index to set for @a v.
 */
void BoostGraph::setVertexIndex(vertex_descriptor v, int index)
{
    // setez proprietatea vertex_index a elementului *vi
    // din graful g la i
    boost::put(vertex_index, *g, v, index);
}

/**
 * @brief Returns the vertex descriptor at @a index.
 * @param index The index of the requested vertex descriptor.
 * @return The vertex descriptor at @a index.
 */
vertex_descriptor BoostGraph::descriptorAt(int index)
{
    return boost::vertex(index, *g);
}

/**
 * @brief Returns a reference to a pair of vertex iterators, the beginning and
 * the end of the vertices list.
 * @return A reference to a pair of vertex iterators, the beginning and the end
 * of the vertices list.
 */
std::pair<vertex_iterator, vertex_iterator> &BoostGraph::vertices()
{
    this->iterators = boost::vertices(*g);
    return this->iterators;
}

/**
 * @brief Returns the number of vertices in the graph.
 * @return The number of vertices in the graph.
 */
vertices_size_type BoostGraph::verticesCount()
{
    return boost::num_vertices(*g);
}
